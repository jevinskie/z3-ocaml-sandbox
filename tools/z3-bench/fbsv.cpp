#include <cstdint>
#include <cstdio>
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

template <typename T, size_t InlineCapacity, typename SizeT = uint16_t,
          typename SVT = folly::small_vector<T, InlineCapacity, policy_size_type<SizeT>>>
class alignas(SVT) CoolVec final {
public:
    using sv_t = SVT;

    // template<typename... Args> CoolVec(Args&& args) : m_u(std::forward<Args>(args)...) {};
    CoolVec(auto &&...args) {
        SVT *tptr = new (m_u.buf) SVT(std::forward<decltype(args)>(args)...);
    }

    sv_t &v() {
        return *reinterpret_cast<sv_t *>(m_u.buf);
    }

    bool &discrim() {
        return m_u.discrim.is_cool;
    }

private:
    typedef struct [[gnu::packed]] {
        SizeT sz;
        bool is_cool;
    } discrim_t;
    static_assert(sizeof(discrim_t) <= alignof(T));
    typedef union alignas(SVT) {
        char buf[sizeof(sv_t)];
        discrim_t discrim;
    } u_t;
    u_t m_u;
};

folly::small_vector<int, 8, policy_size_type<uint16_t>> sv;
CoolVec<int, 8, uint16_t> cv;

int main(void) {
    puts("hi");
    printf("sizeof(cv): %zu\n", sizeof(cv));
    printf("&cv: %p\n", &cv);
    printf("&cv.v(): %p\n", &cv.v());
    printf("&cv.discrim(): %p\n", &cv.discrim());
    printf("cv.v().data(): %p\n", cv.v().data());
    // ss = "123";
    // return sv.size() + ss.size();
    sv.push_back(42);
    cv.v().push_back(243);
    return sv.size() + cv.v().size();
}
