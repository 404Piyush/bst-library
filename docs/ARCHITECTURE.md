# Architecture

A short tour of the design choices in libbst.

## Why generic over a comparator, not templates?

C doesn't have templates. The two standard ways to do generic
containers in C are:

1. `void*` + user-supplied function pointers (what we chose).
2. Heavy macro-based "templates" that expand to type-specific
   copies (what GLib and libsodium do).

We picked (1) because:

* the public surface stays small (one set of function names, one
  type per concept — `bst`, `bst_node`),
* it composes well with C++ — wrap a header and `extern "C"` and
  the BST is usable from C++ with no changes,
* the implementation stays compact (~250 LOC), which makes the
  library auditable in one sitting.

The cost: you have to write (or copy) a 3-line comparator per
key type. We provide `bst_cmp_int` and `bst_cmp_str` as templates.

## Why a single allocation per node?

A `bst_node` is one `malloc` that holds both the node and the
`void*` payload pointer. We could have used flexible-array
members to put the key bytes inline, but that would have meant
either:

* a separate API for "fixed-size keys" vs "heap keys", or
* a `memcpy` of the key into the node on insert, breaking the
  "the tree owns node memory, you own payload" contract.

We chose the simple contract. The trade-off is one extra
indirection per comparison (load the payload pointer, then load
the key), which is hidden by the L1 cache in the common case.

## Why no parent pointers?

Parent pointers would let us implement an O(1) `bst_next` (in-order
successor) and an O(1) "resume from where you left off" iterator.
We left them out because:

* `bst_inorder` already gives the user the full sorted sequence in
  O(n) — there is no "resume" semantic that's worth the memory
  cost,
* the four-extra-bytes-per-node in modern pointer-heavy code
  (cache lines are 64 bytes) costs us measurable density, and
* the `bst_remove` algorithm uses the in-order successor trick
  without needing parent pointers.

If you need streaming iteration over very large trees, you can
add parent pointers in a fork of the library and the rest of the
API will continue to work.

## Why no balancing?

Balancing (AVL, red-black, scapegoat, treap) adds ~3× the
implementation complexity for a *guaranteed* log-height tree.
That complexity is worth it for a production library; for a
teaching library it isn't. The doc explicitly notes the caveat
and the *workarounds* (sort the input, or wrap with a balancing
library).
