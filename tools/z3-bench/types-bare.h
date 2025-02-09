#pragma once

typedef __SIZE_TYPE__ jsize_t;
typedef __UINT8_TYPE__ juint8_t;
typedef __UINT16_TYPE__ juint16_t;
typedef __UINT32_TYPE__ juint32_t;
typedef __UINT64_TYPE__ juint64_t;
typedef __UINTPTR_TYPE__ juintptr_t;
typedef struct pthread_s *pthread_t;

#ifndef NULL
#define NULL ((void *)0)
#endif

__attribute__((noreturn)) extern void abort(void);
extern int printf(const char *fmt, ...);
extern void *memcpy(void *dst, const void *src, jsize_t sz);
extern char *strdup(const char *s);
extern int pthread_threadid_np(pthread_t thread, juint64_t *thread_id);
extern pthread_t pthread_self(void);
extern juint64_t __thread_selfid(void);
