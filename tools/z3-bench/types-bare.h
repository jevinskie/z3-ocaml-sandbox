#pragma once

typedef __SIZE_TYPE__ jsize_t;
typedef __UINT8_TYPE__ juint8_t;
typedef __UINTPTR_TYPE__ juintptr_t;

#ifndef NULL
#define NULL ((void *)0)
#endif

__attribute__((noreturn)) extern void abort(void);
extern int printf(const char *fmt, ...);
extern void *memcpy(void *dst, const void *src, jsize_t sz);
