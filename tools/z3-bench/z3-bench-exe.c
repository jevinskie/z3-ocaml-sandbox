extern int z3_bench_main(int argc, const char **argv, const char **envp, const char **apple);

_Thread_local void *tlpz;

void *get_tlpz(void) {
    return tlpz;
}

void set_tlpz(void *p) {
    tlpz = p;
}

int main(int argc, const char **argv, const char **envp, const char **apple) {
    tlpz = (void *)(0);
    return z3_bench_main(argc, argv, envp, apple);
}
