# bst-library

A small, dependency-free **binary search tree** library written in C11.
Generic over the key (via a user-supplied comparator) with a `void*`
payload. Comes with a real test suite, a microbenchmark, a worked
example, and CI on Linux + macOS.

## Features

* Public API in `include/bst.h` — clean, documented, no globals
* Generic over key type: bring your own comparator (`bst_cmp_int`,
  `bst_cmp_str` included; one for `int` and one for `char*`)
* Standard BST operations: `insert`, `find`, `remove`, `inorder`,
  `preorder`, `postorder`, `height`, `size`, `empty`
* C11, ~250 LOC, no malloc-after-init leaks (every API has a clear
  ownership rule)
* Test suite with 8 test cases (lives in `tests/`)
* Microbenchmark with phase timings (lives in `bench/`)
* Worked example: `wordcount` reads stdin, prints unique sorted words
* CI on Linux + macOS × {clang, gcc}

## Quick start

```bash
make          # build libbst.a, test_bst, bench_bst
make test     # run the test suite
make bench    # run the benchmark (default 100k keys)
make wordcount # build + run the wordcount example
make clean
```

## API

```c
#include <bst.h>

bst *bst_create(bst_cmp_fn cmp);
void bst_destroy(bst *t, bst_free_fn payload_free);

bool      bst_insert (bst *t, void *payload);
bool      bst_remove (bst *t, const void *key, bst_free_fn payload_free);
bst_node *bst_find   (const bst *t, const void *key);

size_t bst_size  (const bst *t);
bool   bst_empty (const bst *t);
int    bst_height(const bst *t);

void bst_inorder  (const bst *t, bst_visit_fn visit, void *user);
void bst_preorder (const bst *t, bst_visit_fn visit, void *user);
void bst_postorder(const bst *t, bst_visit_fn visit, void *user);

/* built-in comparators */
int bst_cmp_int(const void *a, const void *b);
int bst_cmp_str(const void *a, const void *b);
```

See [`docs/API.md`](./docs/API.md) for the full reference, including
complexity notes and ownership rules.

## Example

```c
#include <bst.h>
#include <stdio.h>

int main(void) {
    bst *t = bst_create(bst_cmp_int);
    int vs[] = { 5, 3, 7, 1, 4, 6, 9 };
    for (size_t i = 0; i < sizeof(vs)/sizeof(vs[0]); i++)
        bst_insert(t, &vs[i]);
    bst_inorder(t, (bst_visit_fn)puts_int, NULL);  // 1 3 4 5 6 7 9
    bst_destroy(t, NULL);
}
```

The `wordcount` example in `examples/wordcount.c` does this with
strings via stdin.

## Performance

`make bench` on an Apple M-series (numbers vary by machine, see
`bench/RESULTS.md` for the captured run):

```
insert  100000 :   0.034 s  (  2.94 M ops/s)  height=37
find    100000 :   0.028 s  (  3.57 M ops/s)  hits=100000
remove  100000 :   0.044 s  (  2.27 M ops/s)  removed=100000
```

Because the tree is **unbalanced**, sorted-input insertion produces
a degenerate tree (height = n). If you need log-height guarantees
without sorting your data first, see the *Caveats* section below.

## Caveats

* No balancing. For pathological inputs (already-sorted data) the
  tree degenerates to a linked list.
* No parent pointers, so iterators are single-pass.
* Not thread-safe. Wrap calls in a mutex if you need it.

## Project layout

```
include/         public API
src/             implementation
tests/           test suite
bench/           microbenchmark + RESULTS.md
examples/        wordcount
docs/            API.md, ARCHITECTURE.md
.github/         CI configuration
Makefile         build system
LICENSE          MIT
CHANGELOG.md     release notes
```

## License

MIT — see [`LICENSE`](./LICENSE).
