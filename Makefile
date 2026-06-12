CC      ?= clang
CFLAGS  ?= -O2 -Wall -Wextra -Wpedantic -std=c11 -Iinclude
DEBUG_FLAGS ?= -O0 -g -fno-omit-frame-pointer

# On macOS, build for the host arch (x86_64 or arm64).
# On Linux, no -arch flag.
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    ARCH := -arch $(shell uname -m | sed 's/arm64/arm64/;s/x86_64/x86_64/')
else
    ARCH :=
endif

BUILD   := build
LIB     := $(BUILD)/libbst.a

SRCS    := $(wildcard src/*.c)
OBJS    := $(patsubst src/%.c,$(BUILD)/%.o,$(SRCS))
HEADERS := $(wildcard include/*.h)

# ---- library --------------------------------------------------------

all: $(LIB) $(BUILD)/test_bst $(BUILD)/bench_bst

$(BUILD):
	@mkdir -p $@

$(BUILD)/%.o: src/%.c $(HEADERS) | $(BUILD)
	$(CC) $(ARCH) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJS)
	ar rcs $@ $^

# ---- tests ----------------------------------------------------------

$(BUILD)/test_bst: tests/test_bst.c $(LIB) $(HEADERS) | $(BUILD)
	$(CC) $(ARCH) $(CFLAGS) -Itests tests/test_bst.c $(LIB) -o $@

test: $(BUILD)/test_bst
	./$(BUILD)/test_bst

# ---- benchmark ------------------------------------------------------

$(BUILD)/bench_bst: bench/bench_bst.c $(LIB) $(HEADERS) | $(BUILD)
	$(CC) $(ARCH) $(CFLAGS) -Ibench bench/bench_bst.c $(LIB) -o $@

bench: $(BUILD)/bench_bst
	./$(BUILD)/bench_bst $(N)

# ---- examples -------------------------------------------------------

$(BUILD)/wordcount: examples/wordcount.c $(LIB) $(HEADERS) | $(BUILD)
	$(CC) $(ARCH) $(CFLAGS) -Iexamples examples/wordcount.c $(LIB) -o $@

wordcount: $(BUILD)/wordcount
	@echo "Try:  ./$(BUILD)/wordcount < examples/words.txt"
	@if [ -f examples/words.txt ]; then ./$(BUILD)/wordcount < examples/words.txt; else echo "(no examples/words.txt)"; fi

# ---- debug build ----------------------------------------------------

debug: CFLAGS := $(DEBUG_FLAGS) -Wall -Wextra -Wpedantic -std=c11 -Iinclude
debug: clean all

# ---- housekeeping ---------------------------------------------------

clean:
	rm -rf $(BUILD)

.PHONY: all test bench wordcount debug clean
