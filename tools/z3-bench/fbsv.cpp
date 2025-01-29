#include "folly/container/small_vector.h"
#include <cstdint>
#include <utility>

#if 0
static void assert(...) {

}

#include <jemalloc/jemalloc.h>
#include <new>
#endif

// #include <folly/FBString.h>
#include <folly/small_vector.h>

using folly::small_vector_policy::policy_size_type;

// folly::fbstring ss;
template class folly::small_vector<int, 8, policy_size_type<uint16_t>>;

template <typename T, size_t InlineCapacity, typename SizeT = uint16_t> class CoolVec final {
public:
    // template<typename... Args> CoolVec(Args&& args) : m_u(std::forward<Args>(args)...) {};
    CoolVec(auto &&...args) : m_u(std::forward<decltype(args)>(args)...) {};

private:
    typedef struct [[gnu::packed]] {
        SizeT sz;
        bool is_cool;
    } discrim_t;
    static_assert(sizeof(discrim_t) <= sizeof(void *));
    typedef union {
        // folly::small_vector<T, InlineCapacity, policy_size_type<SizeT>> sv;
        discrim_t discrim;
    } u_t;
    u_t m_u;
};

folly::small_vector<int, 8, policy_size_type<uint16_t>> sv;
CoolVec<int, 8> cv;

int main(void) {
    puts("hi");
    // ss = "123";
    // return sv.size() + ss.size();
    sv.push_back(42);
    cv.push_back(243);
    return sv.size() + cv.size();
}
