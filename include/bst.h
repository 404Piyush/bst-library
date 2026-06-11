/* bst.h - Public API of libbst
 *
 * A small, dependency-free binary search tree over integer keys with
 * a generic void* payload. The tree owns the node memory; the user
 * owns the payload memory (or arranges for the tree to do so via
 * the `payload_free` callback passed to bst_destroy).
 *
 * Thread-safety: none. Wrap calls in a mutex if you need it.
 * Iterator-safety: iterators are valid until the next bst_* call
 * that mutates the tree.
 *
 * Complexity:
 *   - bst_insert / bst_find / bst_remove:  O(h)
 *   - bst_destroy:                          O(n)
 *   - bst_first / bst_last / bst_next:     O(h)
 * where h is the tree height. For a balanced tree h = O(log n);
 * for a degenerate (sorted-input) tree h = O(n).
 */
#ifndef BST_H
#define BST_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int   (*bst_cmp_fn)  (const void *a, const void *b);
typedef void  (*bst_free_fn) (void *payload);
typedef void  (*bst_visit_fn)(const void *payload, void *user);

typedef struct bst_node {
    void             *payload;     /* user-owned data */
    struct bst_node  *left;
    struct bst_node  *right;
} bst_node;

typedef struct bst {
    bst_node  *root;
    bst_cmp_fn cmp;
    size_t     size;
} bst;

/* --- lifecycle ----------------------------------------------------- */

bst *bst_create(bst_cmp_fn cmp);
void bst_destroy(bst *t, bst_free_fn payload_free);

/* --- mutators ------------------------------------------------------ */

/* Returns true if a new node was inserted, false if the key already
 * existed (in which case the existing payload is left untouched). */
bool bst_insert(bst *t, void *payload);

bool bst_remove(bst *t, const void *key, bst_free_fn payload_free);

/* --- queries ------------------------------------------------------- */

size_t       bst_size  (const bst *t);
bool         bst_empty (const bst *t);
bst_node    *bst_find  (const bst *t, const void *key);
int          bst_height(const bst *t);
void         bst_inorder (const bst *t, bst_visit_fn visit, void *user);
void         bst_preorder(const bst *t, bst_visit_fn visit, void *user);
void         bst_postorder(const bst *t, bst_visit_fn visit, void *user);

/* --- built-in comparators ------------------------------------------ */

int bst_cmp_int   (const void *a, const void *b);
int bst_cmp_str   (const void *a, const void *b);

#ifdef __cplusplus
}
#endif

#endif /* BST_H */
