#include "noalloc-stdio.h"
#include "types-bare.h"

static char nibble_to_ascii_hex(const juint8_t nib) {
    if (nib <= 9) {
        return nib + '0';
    } else if (nib >= 0xA && nib <= 0xF) {
        return nib - 0xA + 'a';
    } else {
        abort();
    }
}

void puts_ptr(const void *p) {
    char buf[2 + sizeof(p) * 2 + 1] = {'0', 'x'};
    char *os                        = &buf[2];
    juintptr_t ip                   = (juintptr_t)p;
    juint8_t ipb[sizeof(ip)];
    memcpy(ipb, &ip, sizeof(ip));
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
    puts(buf);
}
