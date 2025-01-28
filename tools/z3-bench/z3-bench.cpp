#undef NDEBUG
#include <cassert>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <pthread/qos.h>
#include <span>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/qos.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

#include <BS_thread_pool.hpp>
#include <fmt/format.h>
#include <z3.h>

typedef enum Z3_mini_lbool {
    Z3_MINI_L_FALSE = -1,
    Z3_MINI_L_UNDEF,
    Z3_MINI_L_TRUE,
    Z3_MINI_L_UNINIT,
} Z3_mini_lbool;

static std::vector<char> slurp_file(const fs::path &path) {
    const auto fd = ::open(path.c_str(), O_RDONLY);
    assert(fd >= 0);
    struct stat st;
    assert(!::fstat(fd, &st));
    const auto sz = static_cast<size_t>(st.st_size);
    if (!sz) {
        assert(!::close(fd));
        return {};
    }
    auto res = std::vector<char>(sz);
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
                                 std::vector<std::vector<char>> &blobs, std::mutex &blobs_mutex) {
    auto content = slurp_file(smt2_path);
    {
        std::lock_guard lock{blobs_mutex};
        blobs.push_back(content);
    }
    ++num_files;
}

// Function to recursively search files using thread pool
template <BS::opt_t OptFlags>
void search_directory(const fs::path &root, std::atomic<size_t> &num_files, std::vector<std::vector<char>> &blobs,
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

int main(int argc, const char **argv) {
    if (argc != 2) {
        fmt::print(stderr, "usage: z3-bench <path to directory with .stm2 files>\n");
        return -1;
    }
    set_thread_priority_11();
    const auto dir_path = fs::path{argv[1]};
    std::atomic<size_t> num_files{0};
    std::mutex blobs_mutex;
    std::vector<std::vector<char>> blobs;
    BS::thread_pool tp;
    search_directory(dir_path, num_files, blobs, blobs_mutex, tp);
    tp.wait();
    fmt::print("num .smt2 files: {:d}\n", num_files.load());
    return 0;
}
