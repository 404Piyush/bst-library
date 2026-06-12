# Changelog

All notable changes to bst-library are documented here.
The format follows [Keep a Changelog](https://keepachangelog.com/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [1.0.0] - 2026-06-12

### Added
- Interactive editorial landing page at
  [bst-library.404piyush.me](https://bst-library.404piyush.me)
  with a JavaScript port of the library and a live SVG
  playground.
- `CHANGELOG.md` (this file).
- `CONTRIBUTING.md`.
- `CODE_OF_CONDUCT.md` (Contributor Covenant 2.1).
- `SECURITY.md`.
- `SHOWCASE.md` with worked examples and benchmark notes.
- `.github/ISSUE_TEMPLATE/` (bug report, feature request,
  documentation issue).
- `.github/PULL_REQUEST_TEMPLATE.md`.

### Notes
- Source under MIT.
- 74 assertions, 8 test cases, all passing.
- CI on Linux + macOS × {clang, gcc}, all green.

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
