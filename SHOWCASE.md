# Showcase

A closer look at bst-library.

## Worked tree

Insert `5, 3, 7, 1, 4, 6, 8` in that order. The tree is
balanced by chance, not by design:

```
        5
       / \
      3   7
     / \ / \
    1  4 6  8
```

The in-order walk prints `1 3 4 5 6 7 8`. The pre-order walk
prints `5 3 1 4 7 6 8`. The post-order walk prints
`1 4 3 6 8 7 5`.

## How the operations look

**Insertion.** Walk from the root, comparing each node's key
to the new one. Smaller goes left, larger goes right, equal
replaces the payload. The first empty child gets the new
node. The cost is `O(h)`.

**Removal of a leaf or one-child node.** Replace the node
with its single child, or unlink it. One pointer move.

**Removal of a two-child node.** Find the in-order successor
(smallest key in the right subtree), copy its key and
payload into the node being removed, then unlink the
successor from its parent. The successor has at most one
child (it has no left child, by definition), so the
unlink is the easy case.

## Benchmarks on M-series, `-O2`

| Operation | Rate           | N        |
|-----------|----------------|----------|
| insert    | 3.5 M ops/s    | 1,000,000 |
| find      | 4.8 M ops/s    | 1,000,000 |
| remove    | 2.8 M ops/s    | 1,000,000 |
| inorder   | 6.0 M nodes/s  | 1,000,000 |

The numbers are what `bench/bench_bst.c` reports on
the same machine that produced them. Run `make bench` to
reproduce.

## Why no rotations

A plain BST is `O(n)` on sorted input. A balanced variant
(AVL, red-black, scapegoat) keeps the height at `O(log n)`
at the cost of rotation logic. The library does not include
rotation. If you need it, the planned design is documented
in [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md); the work
is tracked on the gpu-engineering roadmap.

## Try the live playground

[bst-library.404piyush.me](https://bst-library.404piyush.me)
has an interactive SVG tree that you can drive from the
browser. No install required.
