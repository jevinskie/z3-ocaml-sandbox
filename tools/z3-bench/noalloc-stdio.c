#include "noalloc-stdio.h"
#include "misc-decls.h"
#include "types-bare.h"

// #include <sys/syscall.h>
#ifndef SYS_write
#define SYS_write 4
#endif

static jsize_t my_strlen(const char *s) {
    jsize_t sz = 0;
    while (*s++) {
        ++sz;
    }
    return sz;
}

static void *my_memcpy(void *dst, const void *src, jsize_t sz) {
    const char *ip = (const char *)src;
    char *op       = (char *)dst;
    for (jsize_t i = 0; i < sz; ++i) {
        *op++ = *ip++;
    }
    return op;
}

static void write_raw(int fd, const void *buf, jsize_t sz) {
    register int _fd_and_res __asm("w0") = fd;
    register juintptr_t _buf __asm("x1") = (juintptr_t)buf;
    register jsize_t _sz __asm("x2")     = sz;

    __asm__ __volatile("mov x16, %[syscall_num]\n\t"
                       "svc #0x80"
                       : "+r"(_fd_and_res)
                       : [syscall_num] "I"(SYS_write), "r"(_fd_and_res), "r"(_buf), "r"(_sz)
                       : "cc");
}

SYM_INT jsize_t puts_str(const char *s) {
    const jsize_t sz = my_strlen(s);
    write_raw(STDOUT_FILENO, s, sz);
    write_raw(STDOUT_FILENO, "\n", 1);
    return sz;
}

SYM_INT char nibble_to_ascii_hex(const juint8_t nib) {
    if (nib <= 9) {
        return nib + '0';
    } else if (nib >= 0xA && nib <= 0xF) {
        return nib - 0xA + 'a';
    } else {
        abort();
    }
}

SYM_INT void write_size_to_strbuf(juintptr_t v, char *buf, const jsize_t sz) {
    jsize_t idx = sz - 1;
    char *os    = buf + sz - 1;
    _Bool done  = 0;
    for (jsize_t i = 0; i < sz; ++i) {
        *os-- = done ? ' ' : (v % 10) + '0';
        v /= 10;
        done |= v == 0;
    }
    if (!done) {
        abort();
    }
}

SYM_INT void write_ptr_to_strbuf(const void *p, char *buf) {
    buf[0]        = '0';
    buf[1]        = 'x';
    char *os      = &buf[2];
    juintptr_t ip = (juintptr_t)p;
    juint8_t ipb[sizeof(ip)];
    my_memcpy(ipb, &ip, sizeof(ip));
    // assumes little endian
    for (jsize_t i = 0; i < sizeof(p); ++i) {
        jsize_t j   = sizeof(p) - 1 - i;
        juint8_t b  = ipb[j];
        juint8_t bh = (b >> 4) & 0xf;
        juint8_t bl = b & 0xf;
        os[0]       = nibble_to_ascii_hex(bh);
        os[1]       = nibble_to_ascii_hex(bl);
        os += 2;
    }
}

SYM_INT int puts_ptr(const void *p) {
    char buf[2 + sizeof(p) * 2 + 1];
    write_ptr_to_strbuf(p, buf);
    buf[sizeof(buf) - 1] = '\0';
    return puts_str(buf);
}
