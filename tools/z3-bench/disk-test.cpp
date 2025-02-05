#include <stdint.h>
#undef NDEBUG
#include <cassert>

#include <atomic>
#include <chrono>
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
#include <thread>
#include <type_traits>
#include <unistd.h>

#include <BS_thread_pool.hpp>
#include <fmt/format.h>
#include <fmt/std.h>
#include <folly/FBString.h>
#include <folly/FBVector.h>
#include <folly/small_vector.h>
#include <indicators/cursor_control.hpp>
#include <indicators/indeterminate_progress_bar.hpp>
#include <indicators/progress_bar.hpp>
#include <indicators/termcolor.hpp>

template <class T, class Allocator = std::allocator<T>> using vector_t = folly::fbvector<T, Allocator>;
// template <class T, class Allocator = std::allocator<T>> using vector_t = std::vector<T, Allocator>;

using string_t = folly::fbstring;
// using string_t = std::string;

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

static bool has_nonzero(const uint64_t *const buf, size_t cnt) {
#pragma unroll 8
    for (size_t i = 0; i < cnt; ++i) {
        if (buf[i]) {
            return true;
        }
    }
    return false;
}

static void set_thread_priority_11(void) {
    assert(!pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0));
}

static void set_thread_priority_10(void) {
    assert(!pthread_set_qos_class_self_np(QOS_CLASS_USER_INITIATED, 0));
}

// Function to recursively search files using thread pool
template <BS::opt_t OptFlags>
void search_disk(const uint64_t *const mapping, size_t sz, std::atomic<size_t> &num_nonzero_gb,
                 vector_t<uint8_t> &non_zero, std::mutex &blobs_mutex, BS::thread_pool<OptFlags> &pool) {
    const auto num_chunks = non_zero.size();

    indicators::IndeterminateProgressBar bar{
        // indicators::option::BarWidth{40},
        indicators::option::Start{"["},
        indicators::option::Fill{"."},
        indicators::option::Lead{"<==>"},
        indicators::option::End{"]"},
        indicators::option::PrefixText{"Reading in SMT2"},
        indicators::option::ForegroundColor{indicators::Color::yellow},
        indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};

    indicators::show_console_cursor(false);

    for (size_t i = 0; i < num_chunks; ++i) {
        const size_t start_elem_off       = i * ((1024 * 1024 * 1024) / sizeof(uint64_t));
        const size_t end_elem_off_full    = (i + 1) * ((1024 * 1024 * 1024) / sizeof(uint64_t));
        const size_t end_elem_off_trimmed = std::min(end_elem_off_full, sz / sizeof(uint64_t));
        const auto slice_start            = &mapping[start_elem_off];
        const auto slice_end              = &mapping[end_elem_off_trimmed];
        const auto slice_cnt_64           = end_elem_off_trimmed - start_elem_off;
        pool.detach_task([slice_start, slice_cnt_64, &num_nonzero_gb, &bar] {
            const auto hnz = has_nonzero(slice_start, slice_cnt_64);
            if (hnz) {
                ++num_nonzero_gb;
            }
            bar.tick();
        });
    }
    pool.wait();

    bar.mark_as_completed();
    indicators::show_console_cursor(true);
}

// using folly::small_vector_policy::policy_size_type;

// folly::fbstring ss;
// folly::small_vector<int, 8, policy_size_type<uint16_t>> sv;

int main(int argc, const char **argv) {
    if (argc != 2) {
        fmt::print(stderr, "usage: z3-bench <path to directory with .stm2 files>\n");
        return -1;
    }
    set_thread_priority_11();
    const auto disk_path = fs::path{argv[1]};
    std::atomic<size_t> num_nonzero_gb{0};
    std::mutex blobs_mutex;
    const auto fd = ::open(disk_path.c_str(), O_RDONLY);
    assert(fd >= 0);
    struct stat st;
    assert(!::fstat(fd, &st));
    const auto sz = static_cast<size_t>(st.st_size);
    if (!sz) {
        assert(!::close(fd));
        return 1;
    }
    fmt::print("{} sz: {}\n", disk_path, sz);
    assert(sz % sizeof(uint64_t) == 0);
    auto res                      = vector_t<char>(sz);
    const uint64_t *const mapping = (uint64_t *const)::mmap(nullptr, sz, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(!::close(fd));
    BS::thread_pool tp;
    const auto sz_kb      = sz / 1024;
    const auto sz_mb      = sz_kb / 1024;
    const auto sz_gb      = sz_mb / 1024;
    const auto num_checks = sz_gb + 1;
    vector_t<uint8_t> non_zero(sz_gb);
    search_disk(mapping, sz, num_nonzero_gb, non_zero, blobs_mutex, tp);
    fmt::print("num .smt2 files: {:d}\n", num_nonzero_gb.load());
    return 0;
}
