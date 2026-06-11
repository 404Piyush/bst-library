# API reference

This document describes every public function in `include/bst.h`.

## Conventions

* **Ownership.** The tree owns its `bst_node` memory. The user owns
  the `payload` memory unless they pass a `payload_free` callback
  to `bst_remove` or `bst_destroy`.
* **Thread-safety.** None. Concurrent calls must be serialised.
* **Iterator validity.** A `bst_visit_fn` callback may not call any
  `bst_*` function that mutates the tree.

---

## bst_create

```c
bst *bst_create(bst_cmp_fn cmp);
```

Allocate a new empty BST. `cmp` is required. Returns `NULL` on
allocation failure or if `cmp` is `NULL`.

## bst_destroy

```c
void bst_destroy(bst *t, bst_free_fn payload_free);
```

Free all node memory and the `bst` itself. If `payload_free` is
non-`NULL`, it is called on every remaining payload (e.g., pass
`free` for heap-allocated payloads). Safe to call with `t == NULL`.

## bst_insert

```c
bool bst_insert(bst *t, void *payload);
```

Insert a new node with `payload`. The `payload` itself is used as
the key — your comparator is called with two payloads, so the
"key" is whatever field(s) you choose to compare. If a node with
an *equal* key (per `cmp`) already exists, the existing node is
left untouched and `false` is returned. Returns `true` on success.

## bst_remove

```c
bool bst_remove(bst *t, const void *key, bst_free_fn payload_free);
```

Remove the node with the given key. If `payload_free` is non-`NULL`,
the removed payload is passed to it. Returns `true` if a node was
removed, `false` if no matching key was found.

## bst_find

```c
bst_node *bst_find(const bst *t, const void *key);
```

Return the node whose payload compares equal to `key`, or `NULL`
if no such node exists. The returned pointer is valid until the
next mutating call on the tree.

## bst_size, bst_empty, bst_height

```c
size_t bst_size  (const bst *t);   // number of nodes
bool   bst_empty (const bst *t);   // size == 0
int    bst_height(const bst *t);   // 0 for empty; 1 for one node
```

`bst_height` is the longest root-to-leaf path in nodes.

## bst_inorder / bst_preorder / bst_postorder

```c
void bst_inorder (const bst *t, bst_visit_fn visit, void *user);
void bst_preorder(const bst *t, bst_visit_fn visit, void *user);
void bst_postorder(const bst *t, bst_visit_fn visit, void *user);
```

Walk the tree, calling `visit(payload, user)` for each node. The
`user` pointer is passed through unchanged — pass whatever context
you need (an output file, a counter, etc.).

## bst_cmp_int, bst_cmp_str

```c
int bst_cmp_int(const void *a, const void *b);   // *(int*)a ? *(int*)b
int bst_cmp_str(const void *a, const void *b);   // strcmp(*(char**)a, *(char**)b)
```

Built-in comparators. Use them directly or as a model for your own.

## Complexity

| Operation        | Average  | Worst (sorted input) |
|------------------|----------|----------------------|
| `bst_insert`     | O(log n) | O(n)                 |
| `bst_remove`     | O(log n) | O(n)                 |
| `bst_find`       | O(log n) | O(n)                 |
| `bst_inorder`    | O(n)     | O(n)                 |
| `bst_height`     | O(n)     | O(n)                 |
| `bst_destroy`    | O(n)     | O(n)                 |
