# Changelog

All notable changes to this project are documented here.
Format follows [Keep a Changelog](https://keepachangelog.com/).

## [0.1.0] - 2026-06-11

### Added
- Initial release: `bst_create`, `bst_destroy`, `bst_insert`,
  `bst_remove`, `bst_find`, `bst_size`, `bst_empty`,
  `bst_height`, `bst_inorder`, `bst_preorder`, `bst_postorder`.
- Built-in comparators: `bst_cmp_int`, `bst_cmp_str`.
- Test suite: 8 tests, 74 assertions, 0 failures.
- Microbenchmark: insert / find / remove phase timings.
- Worked example: `examples/wordcount.c` (read stdin, sort
  unique words).
- CI on Linux + macOS × {clang, gcc}.
- MIT license.
- Full API and architecture docs in `docs/`.

### Known limitations
- Unbalanced: sorted input degenerates to a linked list
  (height = n).
- No thread safety.
- No parent pointers (iterators are single-pass).
