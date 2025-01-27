#undef NDEBUG
#include <cassert>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <pthread/qos.h>
#include <sys/qos.h>
#include <vector>

namespace fs = std::filesystem;

#include <fmt/format.h>

static void set_thread_priority_11(void) {
    assert(!pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0));
}

static void set_thread_priority_10(void) {
    assert(!pthread_set_qos_class_self_np(QOS_CLASS_USER_INITIATED, 0));
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fmt::print(stderr, "usage: z3-bench <path to directory with .stm2 files>\n");
        return -1;
    }
    return 0;
}
