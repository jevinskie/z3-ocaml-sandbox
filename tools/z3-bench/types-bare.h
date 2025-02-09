#pragma once

typedef __SIZE_TYPE__ jsize_t;
typedef __PTRDIFF_TYPE__ jssize_t;
typedef __UINT8_TYPE__ juint8_t;
typedef __UINT16_TYPE__ juint16_t;
typedef __UINT32_TYPE__ juint32_t;
typedef __UINT64_TYPE__ juint64_t;
typedef __UINTPTR_TYPE__ juintptr_t;
typedef struct _opaque_pthread_t *pthread_t;

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

__attribute__((noreturn)) extern void abort(void);
extern int printf(const char *fmt, ...);
#ifndef memcpy
extern void *memcpy(void *dst, const void *src, jsize_t sz);
#endif
extern char *strdup(const char *s);
extern int pthread_threadid_np(pthread_t thread, juint64_t *thread_id);
extern pthread_t pthread_self(void);
extern juint64_t __thread_selfid(void);
extern jsize_t write(int fd, const void *buf, jsize_t sz);
extern jsize_t strlen(const char *p);
