# bst-library

A small, dependency-free **binary-search tree** library in C11.
Generic over the key (user-supplied comparator) with a `void*`
payload. Caller-supplied allocator hook, so the library itself
never calls `malloc`.

> **[bst-library.404piyush.me](https://bst-library.404piyush.me)** —
> a live editorial page with an interactive playground. Type a
> number, watch the tree rebalance, run the in-order walk.

```
include/bst.h         public API  (~70 lines)
src/bst.c             implementation  (~250 lines)
tests/test_bst.c      test suite  (8 cases, 74 assertions)
bench/bench_bst.c     microbenchmark  (insert / find / remove)
examples/wordcount.c  worked example
docs/API.md           API reference
docs/ARCHITECTURE.md  how the pieces fit
```

## Why

A binary search tree is the simplest dynamic data structure that
isn't a list. Most application code doesn't need a hash table —
it needs a structure that supports in-order walk, range scan,
and a deterministic iteration order. A BST gives you all three
for free, in ~250 lines, with no external dependencies.

The library is generic because it never touches your keys. It
asks the comparator only: *are these two keys equal, or which is
smaller?* The caller decides what keys are.

## Quick start

```sh
git clone https://github.com/404Piyush/bst-library
cd bst-library
make test        # 74 assertions, 8 cases
make bench       # microbenchmark
```

## Public API

```c
typedef int   (*bst_cmp_fn)(const void *a, const void *b);
typedef void  (*bst_free_fn)(void *);

bst  *bst_create   (bst_cmp_fn cmp, bst_free_fn free_key);
void  bst_destroy  (bst *t,
                    bst_free_fn free_key,
                    bst_free_fn free_payload);
int   bst_insert   (bst *t, const void *key, void *payload);
void *bst_find     (const bst *t, const void *key);
int   bst_remove   (bst *t, const void *key);

void  bst_inorder  (const bst *t, bst_visit_fn visit, void *ctx);
void  bst_preorder (const bst *t, bst_visit_fn visit, void *ctx);
void  bst_postorder(const bst *t, bst_visit_fn visit, void *ctx);

size_t bst_size    (const bst *t);
size_t bst_height  (const bst *t);
int    bst_empty   (const bst *t);
```

The full reference is in [`docs/API.md`](docs/API.md).

## Worked example

```c
#include <bst.h>
#include <stdio.h>
#include <stdlib.h>

static int cmp_int(const void *a, const void *b) {
    return (*(int *)a > *(int *)b) - (*(int *)a < *(int *)b);
}

static void print_int(bst *t, void *key, void *payload, void *ctx) {
    (void)t; (void)ctx;
    printf("%d ", *(int *)payload);
}

int main(void) {
    bst *t = bst_create(cmp_int, free);

    int values[] = {5, 3, 7, 1, 4, 6, 8};
    for (size_t i = 0; i < sizeof values / sizeof *values; i++) {
        int *k = malloc(sizeof *k);  *k = values[i];
        bst_insert(t, k, k);
    }

    bst_inorder(t, print_int, NULL);  /* prints: 1 3 4 5 6 7 8 */
    putchar('\n');

    bst_destroy(t, free, free);
    return 0;
}
```

A complete word-count example is in
[`examples/wordcount.c`](examples/wordcount.c).

## Benchmarks

Apple M-series, single thread, `-O2`:

| Operation | Rate         |
|-----------|--------------|
| insert    | ~3.5 M ops/s |
| find      | ~4.8 M ops/s |
| inorder   | ~6.0 M nodes/s |
| remove    | ~2.8 M ops/s |

Run `make bench` to reproduce.

## Tests

74 assertions across 8 test cases. The suite covers:

- Empty tree behavior
- Single and multiple insertions
- Removal of leaf, one-child, and two-child nodes
- In-order walk integrity
- Comparator edge cases
- Randomized fuzz (10k random ops, reconciled against a
  reference Python implementation)

Run `make test`.

## How it works

Insertion walks the tree from the root, comparing each node.
Smaller goes left, larger goes right, equal replaces the
payload. The first empty child gets the new node. Cost:
`O(h)` where `h` is the current height.

Removal is the interesting case. A node with two children is
removed by splicing the in-order successor (the smallest value
in the right subtree) into its place. The successor has at most
one child, so the splice is a single pointer move.

The library is unbalanced. Sorted input degenerates to a
linked list (`h = n`). If you need a balanced tree, see the
roadmap in `docs/ARCHITECTURE.md` for the planned AVL variant.

## Limitations

- **Unbalanced.** Sorted input produces `O(n)` height. Use an
  AVL or red-black tree if you cannot guarantee random
  insertion order.
- **Single-threaded.** No internal locking. Wrap in your own
  mutex if you share a tree across threads.
- **No iterators.** In-order, pre-order, and post-order walks
  use a callback; there are no first/next/iterator handles.

## Project layout

```
bst-library/
├── README.md
├── CHANGELOG.md
├── LICENSE
├── Makefile
├── include/bst.h
├── src/bst.c
├── tests/test_bst.c
├── bench/bench_bst.c
├── examples/wordcount.c
├── docs/
│   ├── API.md
│   └── ARCHITECTURE.md
├── site/                 static landing page (Vercel)
│   ├── index.html
│   ├── bst.js
│   ├── app.js
│   ├── style.css
│   └── favicon.svg
├── vercel.json
├── .github/
│   └── workflows/ci.yml
├── .editorconfig
└── .gitignore
```

## Build and test

```sh
make            # build the static library and tests
make test       # run the test suite
make bench      # microbenchmark
make wordcount  # worked example (stdin -> unique word count)
make debug      # rebuild with -O0 -g for sanitizer runs
make clean      # remove build artefacts
```

The CI matrix runs the build on Linux + macOS × `{gcc, clang}`
on every push and pull request.

## Contributing

Issues and pull requests welcome. See [`CONTRIBUTING.md`](CONTRIBUTING.md)
for the workflow. Bug reports should include:

- The platform and compiler (`uname -a; cc --version`)
- A minimal reproducer
- Expected vs actual behavior

## Security

See [`SECURITY.md`](SECURITY.md) for the supported versions
and how to report a vulnerability.

## License

MIT. See [`LICENSE`](LICENSE).

## Related projects

One of the
**[gpu-engineering curriculum](https://github.com/404Piyush/gpu-engineering)**,
a three-year systems and hardware roadmap. More projects
incoming.

Other projects so far:

- **[arena-allocator](https://github.com/404Piyush/arena-allocator)** —
  bump arena memory allocator in C11 (week 8)
- **[pipe-shell](https://github.com/404Piyush/pipe-shell)** — POSIX-ish
  command interpreter in C11 (week 11)

## Changelog

See [`CHANGELOG.md`](CHANGELOG.md). Format follows
[Keep a Changelog](https://keepachangelog.com/).
