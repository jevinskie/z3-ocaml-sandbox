#pragma once

#include "types-bare.h"

extern int puts(const char *s);
extern int puts_ptr(const void *p);
extern void write_size_to_strbuf(juintptr_t v, char *buf, const jsize_t sz);
extern char nibble_to_ascii_hex(const juint8_t nib);
extern void write_ptr_to_strbuf(const void *p, char *buf);
