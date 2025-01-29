#undef NDEBUG
#include <cassert>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <limits>
#include <memory>
#include <mutex>
#include <pthread/qos.h>
#include <span>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/qos.h>
#include <sys/stat.h>
#include <type_traits>
#include <unistd.h>

#include <BS_thread_pool.hpp>
#include <fmt/format.h>
#include <folly/FBString.h>
#include <folly/FBVector.h>
#include <folly/small_vector.h>
#include <sha2/sha2.hpp>
#include <z3.h>

template <class T, class Allocator = std::allocator<T>> using vector_t = folly::fbvector<T, Allocator>;
using string_t                                                         = folly::fbstring;

#define INLINE [[gnu::always_inline]]

namespace fs = std::filesystem;

typedef enum Z3_mini_lbool {
    Z3_MINI_L_FALSE = -1,
    Z3_MINI_L_UNDEF,
    Z3_MINI_L_TRUE,
    Z3_MINI_L_UNINIT,
} Z3_mini_lbool;

template <typename T>
    requires(std::unsigned_integral<T>)
class AtomicWaiter {
public:
    AtomicWaiter(T count) : m_cnt{count} {};
    void release(T num = 1) {
        m_cnt -= num;
        m_cnt.notify_all();
    }
    void wait() {
        T cur = m_cnt;
        while (cur) {
            m_cnt.wait(cur);
            cur = m_cnt;
        }
    }

private:
    std::atomic<T> m_cnt;
};

template <BS::opt_t OT = BS::none> class ThreadPool : public BS::thread_pool<OT> {
public:
    /**
     * @brief A helper class to divide a range into blocks. Used by `detach_blocks()`, `submit_blocks()`,
     * `detach_loop()`, and `submit_loop()`.
     *
     * @tparam T The type of the indices. Should be a signed or unsigned integer.
     */
    template <typename T> class [[nodiscard]] blocks {
    public:
        /**
         * @brief Construct a `blocks` object with the given specifications.
         *
         * @param first_index_ The first index in the range.
         * @param index_after_last_ The index after the last index in the range.
         * @param num_blocks_ The desired number of blocks to divide the range into.
         */
        blocks(const T first_index_, const T index_after_last_, const std::size_t num_blocks_) noexcept
            : first_index(first_index_), index_after_last(index_after_last_), num_blocks(num_blocks_) {
            if (index_after_last > first_index) {
                const std::size_t total_size = static_cast<std::size_t>(index_after_last - first_index);
                num_blocks                   = std::min(num_blocks, total_size);
                block_size                   = total_size / num_blocks;
                remainder                    = total_size % num_blocks;
                if (block_size == 0) {
                    block_size = 1;
                    num_blocks = (total_size > 1) ? total_size : 1;
                }
            } else {
                num_blocks = 0;
            }
        }

        /**
         * @brief Get the index after the last index of a block.
         *
         * @param block The block number.
         * @return The index after the last index.
         */
        [[nodiscard]] T end(const std::size_t block) const noexcept {
            return (block == num_blocks - 1) ? index_after_last : start(block + 1);
        }

        /**
         * @brief Get the number of blocks. Note that this may be different than the desired number of blocks that was
         * passed to the constructor.
         *
         * @return The number of blocks.
         */
        [[nodiscard]] std::size_t get_num_blocks() const noexcept {
            return num_blocks;
        }

        /**
         * @brief Get the first index of a block.
         *
         * @param block The block number.
         * @return The first index.
         */
        [[nodiscard]] T start(const std::size_t block) const noexcept {
            return first_index + static_cast<T>(block * block_size) +
                   static_cast<T>(block < remainder ? block : remainder);
        }

    private:
        /**
         * @brief The size of each block (except possibly the last block).
         */
        std::size_t block_size = 0;

        /**
         * @brief The first index in the range.
         */
        T first_index = 0;

        /**
         * @brief The index after the last index in the range.
         */
        T index_after_last = 0;

        /**
         * @brief The number of blocks.
         */
        std::size_t num_blocks = 0;

        /**
         * @brief The remainder obtained after dividing the total size by the number of blocks.
         */
        std::size_t remainder = 0;
    }; // class blocks

    INLINE void wait_on_n_tasks(size_t n, const auto &task) {
        AtomicWaiter waiter(n);
        for (size_t i = 0; i < n; ++i) {
            push_task([&, i] {
                task(i);
                waiter.release();
            });
        }
        waiter.wait();
    }

    template <typename F, typename Idx1, typename Idx2, typename Idx = std::common_type_t<Idx1, Idx2>>
    // requires(F(size_t{}, Idx{}, Idx{}))
    std::enable_if_t<std::is_invocable_v<F, size_t, Idx, Idx>>
    parallelize_indexed_loop(const Idx1 first_index, const Idx2 index_after_last, const F &loop,
                             const size_t num_blocks = 0) {
        blocks blks(first_index, index_after_last, num_blocks ? num_blocks : ThreadPool<OT>::get_thread_count());
        const auto num_blks = blks.get_num_blocks();
        AtomicWaiter waiter(num_blks);
        for (size_t i = 0; i < num_blks; ++i) {
            detach_task([&, i] {
                loop(i, blks.start(i), blks.end(i));
                waiter.release();
            });
        }
        waiter.wait();
    }

    template <typename F, typename Idx>
    // requires(F(size_t{}, Idx{}, Idx{}))
    std::enable_if_t<std::is_invocable_v<F, size_t, Idx, Idx>>
    parallelize_indexed_loop(const Idx index_after_last, const F &loop, const size_t num_blocks = 0) {
        parallelize_indexed_loop(Idx{0}, index_after_last, loop, num_blocks);
    }
};

static char nibble_to_ascii_hex(const uint8_t chr) {
    if (chr <= 9) {
        return chr + '0';
    } else if (chr >= 0xA && chr <= 0xF) {
        return chr - 0xA + 'a';
    } else {
        std::abort();
    }
}

static string_t to_fbstring(const sha2::sha256_hash &h) {
    string_t ds;
    ds.resize(2 * 256 / 8);
    for (size_t i = 0; i < sizeof(h); ++i) {
        ds[2 * i]     = nibble_to_ascii_hex((h[i] >> 4) & 0xF);
        ds[2 * i + 1] = nibble_to_ascii_hex(h[i] & 0xF);
    }
    return ds;
}

static vector_t<char> slurp_file(const fs::path &path) {
    const auto fd = ::open(path.c_str(), O_RDONLY);
    assert(fd >= 0);
    struct stat st;
    assert(!::fstat(fd, &st));
    const auto sz = static_cast<size_t>(st.st_size);
    if (!sz) {
        assert(!::close(fd));
        return {};
    }
    auto res = vector_t<char>(sz);
    assert(sz == ::read(fd, res.data(), res.size()));
    assert(!::close(fd));
    return res;
}

static void set_thread_priority_11(void) {
    assert(!pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0));
}

static void set_thread_priority_10(void) {
    assert(!pthread_set_qos_class_self_np(QOS_CLASS_USER_INITIATED, 0));
}

// Thread-safe function to search a file and print matches
// FIXME: why was this noinline from the other junk code I copied it from?
// maybe for stack traces for profiling
[[gnu::noinline]] void read_smt2(const fs::path &smt2_path, std::atomic<size_t> &num_files,
                                 vector_t<vector_t<char>> &blobs, std::mutex &blobs_mutex) {
    auto content = slurp_file(smt2_path);
    {
        std::lock_guard lock{blobs_mutex};
        blobs.push_back(content);
    }
    ++num_files;
}

// Function to recursively search files using thread pool
template <BS::opt_t OptFlags>
void search_directory(const fs::path &root, std::atomic<size_t> &num_files, vector_t<vector_t<char>> &blobs,
                      std::mutex &blobs_mutex, BS::thread_pool<OptFlags> &pool) {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file()) {
            const auto smt2_path = entry.path();
            if (!smt2_path.string().ends_with(".smt2")) {
                continue;
            }
            pool.detach_task([smt2_path, &num_files, &blobs, &blobs_mutex] {
                read_smt2(smt2_path, num_files, blobs, blobs_mutex);
            });
        }
    }
}

using folly::small_vector_policy::policy_size_type;

folly::fbstring ss;
folly::small_vector<int, 8, policy_size_type<uint16_t>> sv;

int main(int argc, const char **argv) {
    if (argc != 2) {
        fmt::print(stderr, "usage: z3-bench <path to directory with .stm2 files>\n");
        return -1;
    }
    sv.set_thread_priority_11();
    const auto dir_path = fs::path{argv[1]};
    std::atomic<size_t> num_files{0};
    std::mutex blobs_mutex;
    vector_t<vector_t<char>> blobs;
    BS::thread_pool tp;
    search_directory(dir_path, num_files, blobs, blobs_mutex, tp);
    tp.wait();
    fmt::print("num .smt2 files: {:d}\n", num_files.load());
    return 0;
}
