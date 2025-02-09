#pragma once

#include "types-bare.h"

__attribute__((visibility("default"))) extern void *printing_malloc(jsize_t sz);
__attribute__((visibility("default"))) extern void printing_free(void *p);
