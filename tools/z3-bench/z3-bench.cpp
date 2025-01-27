#undef NDEBUG
#include <cassert>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <pthread/qos.h>
#include <span>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/qos.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

#include <BS_thread_pool.hpp>
#include <fmt/format.h>
#include <z3.h>

class mmaped_file {
public:
    mmaped_file(const fs::path &path);
    ~mmaped_file();
    const uint8_t *data() const noexcept {
        return reinterpret_cast<const uint8_t *>(m_mapping);
    }
    size_t size() const noexcept {
        return m_size;
    }
    std::span<const uint8_t> span() const noexcept {
        return std::span<const uint8_t>(data(), data() + size());
    }

private:
    const void *m_mapping{nullptr};
    size_t m_size{0};
};

mmaped_file::mmaped_file(const fs::path &path) {
    const auto fd = open(path.c_str(), O_RDONLY);
    assert(fd >= 0);
    const auto st = fstat(fd);
    const auto sz = static_cast<size_t>(st.st_size);
    if (sz) {
        m_mapping = checked_mmap(nullptr, sz, PROT_READ, MAP_PRIVATE, fd, 0);
    }
    checked_close(fd);
    m_size = sz;
}
mmaped_file::~mmaped_file() {
    if (size()) {
        checked_munmap(data(), size());
    }
}

static void set_thread_priority_11(void) {
    assert(!pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0));
}

static void set_thread_priority_10(void) {
    assert(!pthread_set_qos_class_self_np(QOS_CLASS_USER_INITIATED, 0));
}

// Thread-safe function to search a file and print matches
[[gnu::noinline]] Z3_lbool check_sat(const fs::path &smt2_path) {
    auto file          = mmaped_file{file_path};
    const auto matches = find_immediate_in_binary(file.span(), imm16, aligned_32, elf_only);
    if (!matches.empty()) {
        num_matches += matches.size();
        std::lock_guard lock(print_mutex);
        fmt::print("Matches in file: {:g}\n", file_path);
        for (const auto off : matches) {
            fmt::print("Offset: {:#x}\n", off);
        }
    }
}

// Function to recursively search files using thread pool
void search_directory(const fs::path &root, const uint16_t imm16, const bool aligned_32, const bool elf_only,
                      std::atomic<size_t> &num_matches, BS::thread_pool &pool, std::mutex &print_mutex) {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file()) {
            const auto file_path = entry.path();
            pool.detach_task([file_path, imm16, aligned_32, elf_only, &num_matches, &print_mutex] {
                search_file(file_path, imm16, aligned_32, elf_only, num_matches, print_mutex);
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
    return 0;
}
