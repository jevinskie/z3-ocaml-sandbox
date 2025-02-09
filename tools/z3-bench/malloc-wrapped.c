#include "malloc-wrapped.h"
#include "alloc.h"
#include "noalloc-stdio.h"

extern typeof(&printing_malloc) orig_malloc;
extern typeof(&printing_free) orig_free;

void *printing_malloc(jsize_t sz) {
    // void *p    = orig_malloc(sz);
    void *p    = malloc(sz);
    char buf[] = "printing_malloc(0xDEADBEEFDEADBEEF) => 0xDEADBEEFDEADBEEF";
    write_ptr_to_strbuf((void *)sz, &buf[16]);
    write_ptr_to_strbuf(p, &buf[39]);
    puts_str(buf);
    return p;
}

void printing_free(void *p) {
    char buf[] = "printing_free(0xDEADBEEFDEADBEEF)";
    write_ptr_to_strbuf(p, &buf[14]);
    puts_str(buf);
    // orig_free(p);
    free(p);
}
