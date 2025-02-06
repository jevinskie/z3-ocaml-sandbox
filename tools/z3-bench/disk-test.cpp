#include <stdint.h>
#undef NDEBUG
#include <cassert>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <pthread/qos.h>
#include <span>
#include <sys/disk.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/qos.h>
#include <sys/stat.h>
#include <thread>
#include <type_traits>
#include <unistd.h>

#include <BS_thread_pool.hpp>
#include <fmt/format.h>
#include <fmt/std.h>
#include <indicators/block_progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/indeterminate_progress_bar.hpp>
#include <indicators/progress_bar.hpp>
#include <indicators/termcolor.hpp>

// template <class T, class Allocator = std::allocator<T>> using vector_t = folly::fbvector<T, Allocator>;
template <class T, class Allocator = std::allocator<T>> using vector_t = std::vector<T, Allocator>;

// using string_t = folly::fbstring;
using string_t = std::string;

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

using tid_t = uint64_t;

static_assert(sizeof(std::thread::id) == sizeof(tid_t));

static bool has_nonzero(const int fd, const size_t buf_sz, const size_t start_elem_off, size_t cnt,
                        const vector_t<uint8_t *> &thread_bufs) {
    // auto buf= (uint64_t *)(thread_bufs[BS::this_thread().get_index().value()]);
    auto buf                  = (uint64_t *)thread_bufs[0];
    const size_t end_elem_off = start_elem_off + cnt;
    size_t cur_elem_off       = start_elem_off;
    while (cur_elem_off < end_elem_off) {
        const size_t foff           = cur_elem_off * sizeof(uint64_t);
        const size_t num_bytes_left = (end_elem_off - cur_elem_off) * sizeof(uint64_t);
        const ssize_t try_rd_sz     = std::min(buf_sz, num_bytes_left);
        // const auto pread_res = pread(fd, buf, try_rd_sz, foff);
        const auto pread_res = read(fd, buf, try_rd_sz);
        assert(pread_res == try_rd_sz);
        const auto num_elem_read = try_rd_sz / sizeof(uint64_t);
        for (size_t i = 0; i < num_elem_read; ++i) {
            if (buf[i]) {
                return true;
            }
        }
        cur_elem_off += num_elem_read;
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
void search_disk(const int fd, const size_t sz, const size_t buf_sz, BS::thread_pool<OptFlags> &pool) {
    using namespace indicators;
    const uint64_t *const mapping = nullptr;
    const size_t chunk_sz_bytes   = 1024 * 1024 * 1024;
    // const size_t chunk_sz_bytes = 1024 * 1024;

    const auto num_chunks = (sz + chunk_sz_bytes - 1) / chunk_sz_bytes;

    vector_t<uint8_t *> thread_bufs;
    for (size_t i = 0; i < 16; ++i) { // FIXME: HACK
        uint8_t *buf = (uint8_t *)aligned_alloc(16 * 1024, buf_sz);
        assert(buf);
        thread_bufs.push_back(buf);
    }

    vector_t<uint8_t> non_zero_chunks(num_chunks);

    std::atomic<size_t> num_nonzero_chunks{};
    std::atomic<size_t> num_chunks_done{};

    BlockProgressBar bar{// option::BarWidth{80},
                         option::Start{"["}, option::End{"]"}, option::ForegroundColor{Color::yellow},
                         option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}};

    // indicators::show_console_cursor(false);

    for (size_t i = 0; i < num_chunks; ++i) {
        const size_t start_elem_off    = i * (chunk_sz_bytes / sizeof(uint64_t));
        const size_t end_elem_off_full = (i + 1) * (chunk_sz_bytes / sizeof(uint64_t));
        const size_t end_elem_off      = std::min(end_elem_off_full, sz / sizeof(uint64_t));
        const auto slice_cnt_64        = end_elem_off - start_elem_off;
        if (!slice_cnt_64) {
            continue;
        }
        // pool.detach_task([i, fd, buf_sz, start_elem_off, slice_cnt_64, &non_zero_chunks, &num_nonzero_chunks,
        // &num_chunks_done, &bar, num_chunks, &thread_bufs] {
        const auto hnz = has_nonzero(fd, buf_sz, start_elem_off, slice_cnt_64, thread_bufs);
        if (hnz) {
            num_nonzero_chunks.fetch_add(1, std::memory_order_relaxed);
            non_zero_chunks[i] = 1;
        }
        const auto this_chunk = num_chunks_done.fetch_add(1, std::memory_order_relaxed);
        bar.set_progress(double(this_chunk) / (100 * num_chunks));
        fmt::print(" #: {} # tot: {} # non-0: {}", this_chunk, num_chunks,
                   num_nonzero_chunks.load(std::memory_order_relaxed));
        // usleep(10000 * this_chunk);
        // });
    }
    pool.wait();

    bar.mark_as_completed();
    // indicators::show_console_cursor(true);

    for (auto *buf : thread_bufs) { // FIXME: HACK
        free(buf);
    }

    fmt::print("num_chunks: {} num_chunks_done: {}\n", num_chunks, num_chunks_done);
    fmt::print("num_nonzero_chunks: {}\n", num_nonzero_chunks);
}

// using folly::small_vector_policy::policy_size_type;

// folly::fbstring ss;
// folly::small_vector<int, 8, policy_size_type<uint16_t>> sv;

int main(int argc, const char **argv) {
    if (argc != 3) {
        fmt::print(stderr, "usage: disk-test <path to block dev> <decimal block size in KiB>\n");
        return -1;
    }
    set_thread_priority_11();
    const auto disk_path = fs::path{argv[1]};
    const auto buf_sz_kb = atoi(argv[2]);
    assert(buf_sz_kb > 0);
    const auto fd = ::open(disk_path.c_str(), O_RDONLY);
    assert(fd >= 0);

    uint32_t block_sz{};
    uint64_t block_count{};

    struct stat st;
    assert(!::fstat(fd, &st));
    size_t sz = static_cast<size_t>(st.st_size);
    if (!sz) {
        if (ioctl(fd, DKIOCGETBLOCKSIZE, &block_sz) != 0) {
            perror("DKIOCGETBLOCKSIZE");
            ::close(fd);
            return 1;
        }

        if (ioctl(fd, DKIOCGETBLOCKCOUNT, &block_count) == -1) {
            ::perror("DKIOCGETBLOCKCOUNT");
            ::close(fd);
            return 1;
        }
        sz = block_sz * block_count;
        if (!sz) {
            fmt::print(::stderr, "sz is zero\n");
            assert(!::close(fd));
            return 1;
        }
    }

    const auto buf_sz = buf_sz_kb * 1024;
    fmt::print("{} fd: {} sz: {} buf_sz\n", disk_path, fd, sz, buf_sz);
    assert(sz % sizeof(uint64_t) == 0);
    BS::thread_pool tp;
    search_disk(fd, sz, buf_sz, tp);
    assert(!::close(fd));
    return 0;
}
