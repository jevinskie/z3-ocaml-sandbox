TARGETS := bindings bindings-oneshot

LDFLAGS ?=
C_CXX_FLAGS ?=

GMP_ROOT ?= $(HOME)/base/gmp-asan
ifeq ($(shell uname -s),Darwin)
LDFLAGS += $(GMP_ROOT)/lib/libgmp.a
# LDFLAGS += /opt/homebrew/opt/gmp/lib/libgmp.a
endif

Z3_ROOT ?= $(HOME)/base/z3-asan
C_CXX_FLAGS += -isystem $(Z3_ROOT)/include
LDFLAGS += $(Z3_ROOT)/lib/libz3.a

C_CXX_FLAGS += -Wall -Wextra -Wpedantic -Weverything -Warray-bounds -Wno-poison-system-directories -Wno-documentation-unknown-command -Wno-gnu-statement-expression-from-macro-expansion -Wno-gnu-line-marker -Wno-documentation
C_CXX_FLAGS += -Wno-nullability-extension
C_CXX_FLAGS += -fsafe-buffer-usage-suggestions
C_CXX_FLAGS += -Wno-unsafe-buffer-usage
C_CXX_FLAGS += -mcpu=native
CFLAGS := $(C_CXX_FLAGS) -std=gnu2x  -Wno-declaration-after-statement -Wno-pre-c2x-compat
CXXFLAGS := $(C_CXX_FLAGS) -std=gnu++2b -fforce-emit-vtables -Wno-c++98-compat-pedantic -Wno-c++20-compat-pedantic -I 3rdparty/cifra -I 3rdparty/magic_enum/include
# LDFLAGS += -lfmt
ifeq ($(shell uname -s),Darwin)
# CXXFLAGS += -isystem /opt/homebrew/opt/fmt/include
# LDFLAGS += -L/opt/homebrew/opt/fmt/lib
endif
DBG_FLAGS := -fno-omit-frame-pointer -g3 -gfull -glldb -gcolumn-info -gdwarf-aranges -ggnu-pubnames
VERBOSE_FLAGS := -v -Wl,-v
NOOPT_FLAGS := -O0
FAST_FLAGS := -O3 -fvectorize -funroll-loops
SMOL_FLAGS := -Oz
NOOUTLINE_FLAGS := -mno-outline
ASAN_FLAGS := $(NOOPT_FLAGS) $(DBG_FLAGS) -fsanitize=address -fsanitize-address-use-after-return=always -fsanitize-address-use-after-scope -fsanitize=address
UBSAN_FLAGS := $(NOOPT_FLAGS) $(DBG_FLAGS) -fsanitize-recover=all -fsanitize=undefined -fsanitize=implicit-integer-truncation -fsanitize=implicit-integer-arithmetic-value-change -fsanitize=implicit-conversion -fsanitize=integer -fsanitize=nullability -fsanitize=float-divide-by-zero -fsanitize=local-bounds
# UBSAN_FLAGS += -fsanitize=implicit-integer-conversion
ASMFLAGS := -fverbose-asm -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -DSHA1_REDIR_DISABLE

all: $(TARGETS)

.PHONY: clean-targets clean-compile-commands clean compile_commands.json scan tidy run-asan run-ubsan

clean-targets:
	rm -rf *.dSYM/
	rm -f $(TARGETS)
	rm -rf *.o

clean-compile-commands:
	rm -f compile_commands.json

clean: clean-targets clean-compile-commands

bindings: bindings.o
bindings-oneshot: bindings-oneshot.o

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS) $(ASAN_FLAGS)

%: %.o
	$(CXX) -o $@ $^ $(CFLAGS) $(ASAN_FLAGS) $(LDFLAGS)

%.ii: %.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) -E

%.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) $(NOOUTLINE_FLAGS)

%.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) $(NOOUTLINE_FLAGS)

%-no-inline.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) $(NOOUTLINE_FLAGS) -fno-inline

%-no-inline.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) $(NOOUTLINE_FLAGS) -fno-inline

%-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-no-inline-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops

%-no-inline-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops


%-O0.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O0 $(NOOUTLINE_FLAGS)

%-O0.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O0 $(NOOUTLINE_FLAGS)

%-O0-no-inline.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O0 $(NOOUTLINE_FLAGS) -fno-inline

%-O0-no-inline.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O0 $(NOOUTLINE_FLAGS) -fno-inline

%-O0-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O0 $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-O0-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O0 $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-O0-no-inline-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O0 $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops

%-O0-no-inline-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O0 $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops


%-Oz.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Oz $(NOOUTLINE_FLAGS)

%-Oz.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Oz $(NOOUTLINE_FLAGS)

%-Oz-no-inline.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Oz $(NOOUTLINE_FLAGS) -fno-inline

%-Oz-no-inline.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Oz $(NOOUTLINE_FLAGS) -fno-inline

%-Oz-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Oz $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-Oz-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Oz $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-Oz-no-inline-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Oz $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops

%-Oz-no-inline-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Oz $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops


%-Os.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Os $(NOOUTLINE_FLAGS)

%-Os.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Os $(NOOUTLINE_FLAGS)

%-Os-no-inline.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Os $(NOOUTLINE_FLAGS) -fno-inline

%-Os-no-inline.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Os $(NOOUTLINE_FLAGS) -fno-inline

%-Os-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Os $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-Os-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Os $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-Os-no-inline-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Os $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops

%-Os-no-inline-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -Os $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops


%-O2.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O2 $(NOOUTLINE_FLAGS)

%-O2.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O2 $(NOOUTLINE_FLAGS)

%-O2-no-inline.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O2 $(NOOUTLINE_FLAGS) -fno-inline

%-O2-no-inline.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O2 $(NOOUTLINE_FLAGS) -fno-inline

%-O2-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O2 $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-O2-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O2 $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-O2-no-inline-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O2 $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops

%-O2-no-inline-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O2 $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops


%-O3.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O3 $(NOOUTLINE_FLAGS)

%-O3.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O3 $(NOOUTLINE_FLAGS)

%-O3-no-inline.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O3 $(NOOUTLINE_FLAGS) -fno-inline

%-O3-no-inline.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O3 $(NOOUTLINE_FLAGS) -fno-inline

%-O3-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O3 $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-O3-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O3 $(NOOUTLINE_FLAGS) -fno-unroll-loops

%-O3-no-inline-no-unroll.asm: %.cpp
	$(CXX) -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O3 $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops

%-O3-no-inline-no-unroll.ll: %.cpp
	$(CXX) -emit-llvm -S -o $@ $^ $(CXXFLAGS) $(ASMFLAGS) -O3 $(NOOUTLINE_FLAGS) -fno-inline -fno-unroll-loops


%-demangled.asm: %.asm
	c++filt -t < $^ | c++filt -n -t | c++filt -t > $@

%-demangled.ll: %.ll
	c++filt -t < $^ | c++filt -n -t | c++filt -t > $@

compile_commands.json:
	bear -- $(MAKE) -B -f $(MAKEFILE_LIST) RUNNING_BEAR=1
	$(MAKE) -f $(MAKEFILE_LIST) clean-targets

scan:
	scan-build -V $(MAKE) -B -f $(MAKEFILE_LIST)

tidy:
	clang-tidy sha1-arm.cpp
