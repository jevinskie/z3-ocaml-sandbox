#pragma once

typedef __SIZE_TYPE__ jsize_t;
typedef __PTRDIFF_TYPE__ jssize_t;
typedef __UINT8_TYPE__ juint8_t;
typedef __UINT16_TYPE__ juint16_t;
typedef __UINT32_TYPE__ juint32_t;
typedef __UINT64_TYPE__ juint64_t;
typedef __INT8_TYPE__ jint8_t;
typedef __INT16_TYPE__ jint16_t;
typedef __INT32_TYPE__ jint32_t;
typedef __INT64_TYPE__ jint64_t;
typedef __UINTPTR_TYPE__ juintptr_t;
typedef struct _opaque_pthread_t *pthread_t;
typedef struct _opaque_pthread_attr_t pthread_attr_t;
typedef juint32_t jmach_port_t;

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

struct _libpthread_functions {
    unsigned long version;
    void (*exit)(int);        // added with version=1
    void *(*malloc)(jsize_t); // added with version=2
    void (*free)(void *);     // added with version=2
};

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
extern int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*func)(void *arg), void *arg);
extern int pthread_join(pthread_t thread, void *arg);
extern juint32_t _dyld_launch_mode(void);
extern void _pthread_start(pthread_t self, jmach_port_t kport, void *(*fun)(void *), void *arg, jsize_t stacksize,
                           unsigned int pflags);
extern int __pthread_init(struct _libpthread_functions *pthread_funcs, const char *envp[], const char *apple[]);
extern int __bsdthread_register(void *threadstart, void *wqthread, int pthsize, void *pthread_init_data,
                                jint32_t *pthread_init_data_size, juint64_t dispatchqueue_offset);
extern __attribute__((noreturn)) void thread_start(pthread_t self, jmach_port_t kport, void *(*fun)(void *), void *arg,
                                                   jsize_t stacksize,
                                                   unsigned int flags); // trampoline into _pthread_start
extern __attribute__((noreturn)) void _pthread_start(pthread_t thread, jmach_port_t kport, void *(*fun)(void *),
                                                     void *arg, jsize_t stacksize, unsigned int flags);
extern void usleep(juint32_t microseconds);
