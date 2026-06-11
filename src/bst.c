/* bst.c - Implementation of the public bst.h API
 *
 * The tree is a classic unbalanced binary search tree. Insertions
 * are recursive; removals use the standard in-order successor
 * trick. The tree is intentionally simple — no balancing, no
 * parent pointers, no colouring.  The goal is clarity, not
 * performance.
 */
#include "bst.h"

#include <stdlib.h>
#include <string.h>

struct bst_key {
    const void *key;
};

static struct bst_key make_key(const void *k) {
    struct bst_key r = { .key = k };
    return r;
}

bst *bst_create(bst_cmp_fn cmp) {
    if (!cmp) return NULL;
    bst *t = (bst *)calloc(1, sizeof(*t));
    if (!t) return NULL;
    t->cmp  = cmp;
    t->root = NULL;
    t->size = 0;
    return t;
}

static void destroy_rec(bst_node *n, bst_free_fn payload_free) {
    if (!n) return;
    destroy_rec(n->left,  payload_free);
    destroy_rec(n->right, payload_free);
    if (payload_free) payload_free(n->payload);
    free(n);
}

void bst_destroy(bst *t, bst_free_fn payload_free) {
    if (!t) return;
    destroy_rec(t->root, payload_free);
    free(t);
}

/* The "key" argument is interpreted as a payload pointer whose
 * first `sizeof(struct bst_key)` bytes contain the actual key.
 * The wrapper functions below handle that transparently for ints
 * and strings.
 */
static bst_node **find_slot(bst_node **root, const void *key, bst_cmp_fn cmp) {
    while (*root) {
        int c = cmp((*root)->payload, key);
        if      (c == 0) return root;
        else if (c >  0) root = &(*root)->left;
        else             root = &(*root)->right;
    }
    return root;          /* NULL slot where a new node goes */
}

bool bst_insert(bst *t, void *payload) {
    if (!t || !payload) return false;
    bst_node **slot = find_slot(&t->root, payload, t->cmp);
    if (*slot) {
        /* Key already present; do not replace. */
        return false;
    }
    bst_node *n = (bst_node *)calloc(1, sizeof(*n));
    if (!n) return false;
    n->payload = payload;
    *slot = n;
    t->size++;
    return true;
}

bst_node *bst_find(const bst *t, const void *key) {
    if (!t) return NULL;
    bst_node *n = t->root;
    while (n) {
        int c = t->cmp(n->payload, key);
        if      (c == 0) return n;
        else if (c >  0) n = n->left;
        else             n = n->right;
    }
    return NULL;
}

bool bst_remove(bst *t, const void *key, bst_free_fn payload_free) {
    if (!t) return false;
    bst_node **parent_link = &t->root;
    while (*parent_link) {
        int c = t->cmp((*parent_link)->payload, key);
        if (c == 0) break;
        parent_link = (c > 0) ? &(*parent_link)->left
                              : &(*parent_link)->right;
    }
    if (!*parent_link) return false;        /* not found */

    bst_node *victim = *parent_link;
    if (!victim->left || !victim->right) {
        /* Zero or one child: easy case. */
        bst_node *child = victim->left ? victim->left : victim->right;
        if (payload_free) payload_free(victim->payload);
        free(victim);
        *parent_link = child;
    } else {
        /* Two children: replace with in-order successor (leftmost
         * node of the right subtree). */
        bst_node **succ_link = &victim->right;
        while ((*succ_link)->left) succ_link = &(*succ_link)->left;
        bst_node *succ = *succ_link;

        /* Move successor's payload into victim, then splice out succ. */
        victim->payload = succ->payload;
        bst_node *succ_child = succ->right;
        if (payload_free && succ != victim) {
            /* Caller wanted to free the removed payload; we are not
             * removing succ's payload, just splicing the node. */
        }
        *succ_link = succ_child;
        free(succ);
    }
    t->size--;
    return true;
}

size_t bst_size(const bst *t)  { return t ? t->size : 0; }
bool   bst_empty(const bst *t) { return !t || t->size == 0; }

static int height_rec(const bst_node *n) {
    if (!n) return 0;
    int l = height_rec(n->left);
    int r = height_rec(n->right);
    return 1 + (l > r ? l : r);
}

int bst_height(const bst *t) { return t ? height_rec(t->root) : 0; }

static void inorder_rec (const bst_node *n, bst_visit_fn f, void *u) {
    if (!n) return;
    inorder_rec (n->left,  f, u);
    f(n->payload, u);
    inorder_rec (n->right, f, u);
}
static void preorder_rec(const bst_node *n, bst_visit_fn f, void *u) {
    if (!n) return;
    f(n->payload, u);
    preorder_rec(n->left,  f, u);
    preorder_rec(n->right, f, u);
}
static void postorder_rec(const bst_node *n, bst_visit_fn f, void *u) {
    if (!n) return;
    postorder_rec(n->left,  f, u);
    postorder_rec(n->right, f, u);
    f(n->payload, u);
}

void bst_inorder (const bst *t, bst_visit_fn f, void *u) { if (t) inorder_rec (t->root, f, u); }
void bst_preorder(const bst *t, bst_visit_fn f, void *u) { if (t) preorder_rec(t->root, f, u); }
void bst_postorder(const bst *t, bst_visit_fn f, void *u) { if (t) postorder_rec(t->root, f, u); }

int bst_cmp_int(const void *a, const void *b) {
    int ai = *(const int *)a;
    int bi = *(const int *)b;
    return (ai > bi) - (ai < bi);
}

int bst_cmp_str(const void *a, const void *b) {
    const char *sa = *(const char *const *)a;
    const char *sb = *(const char *const *)b;
    return strcmp(sa, sb);
}
