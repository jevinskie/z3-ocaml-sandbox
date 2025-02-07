extern int z3_bench_main(int argc, const char **argv, const char **envp, const char **apple);
extern int printf(const char *fmt, ...);
extern void *malloc(unsigned long sz);
extern void free(void *p);

int main(int argc, const char **argv, const char **envp, const char **apple) {
    printf("exe malloc: %p\n", malloc);
    void *p = malloc(4);
    printf("exe malloc(4) = %p\n", p);
    free(p);
    return z3_bench_main(argc, argv, envp, apple);
}
