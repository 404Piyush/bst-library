/* tests/test_bst.c
 *
 * Test suite for libbst. Uses a tiny custom test framework to
 * keep zero external dependencies. Run with:
 *   make test    (or)    ./build/test_bst
 */
#include "bst.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* --- tiny test framework ---------------------------------------- */

static int g_pass = 0;
static int g_fail = 0;

#define ASSERT_TRUE(cond)                                                  \
    do {                                                                    \
        if (cond) { ++g_pass; }                                            \
        else { ++g_fail;                                                   \
            fprintf(stderr, "FAIL: %s:%d  %s\n", __FILE__, __LINE__, #cond); \
        }                                                                   \
    } while (0)

#define ASSERT_EQ_INT(a, b)                                                \
    do {                                                                    \
        long _a = (long)(a), _b = (long)(b);                               \
        if (_a == _b) { ++g_pass; }                                        \
        else { ++g_fail;                                                   \
            fprintf(stderr, "FAIL: %s:%d  %s == %s  (got %ld vs %ld)\n",     \
                    __FILE__, __LINE__, #a, #b, _a, _b);                    \
        }                                                                   \
    } while (0)

#define ASSERT_EQ_STR(a, b)                                                \
    do {                                                                    \
        const char *_a = (a), *_b = (b);                                   \
        if (strcmp(_a, _b) == 0) { ++g_pass; }                             \
        else { ++g_fail;                                                   \
            fprintf(stderr, "FAIL: %s:%d  \"%s\" != \"%s\"\n",              \
                    __FILE__, __LINE__, _a, _b);                            \
        }                                                                   \
    } while (0)

/* --- fixtures ------------------------------------------------------ */

static int free_called_with;

static void counting_free(void *p) {
    free_called_with++;
    free(p);
}

/* --- tests --------------------------------------------------------- */

static void test_create_destroy(void) {
    bst *t = bst_create(bst_cmp_int);
    ASSERT_TRUE(t != NULL);
    ASSERT_EQ_INT(bst_size(t), 0);
    ASSERT_TRUE(bst_empty(t));
    bst_destroy(t, NULL);
}

static void test_null_safety(void) {
    bst_destroy(NULL, NULL);
    ASSERT_EQ_INT(bst_size(NULL), 0);
    ASSERT_TRUE(bst_empty(NULL));
    ASSERT_TRUE(bst_find(NULL, NULL) == NULL);
    ASSERT_TRUE(!bst_insert(NULL, NULL));
    ASSERT_EQ_INT(bst_height(NULL), 0);
    ASSERT_TRUE(!bst_remove(NULL, NULL, NULL));
}

static int extract_int(const void *payload) { return *(const int *)payload; }

static void visit_to_array(const void *payload, void *user) {
    int **arr = (int **)user;
    *(*arr)++ = *(int *)payload;
}

static void test_insert_find(void) {
    bst *t = bst_create(bst_cmp_int);
    int vs[] = { 5, 3, 7, 1, 4, 6, 9 };
    for (size_t i = 0; i < sizeof(vs)/sizeof(vs[0]); i++)
        ASSERT_TRUE(bst_insert(t, &vs[i]));
    ASSERT_EQ_INT(bst_size(t), 7);

    /* Duplicate key: insert returns false, size unchanged. */
    ASSERT_TRUE(!bst_insert(t, &vs[0]));
    ASSERT_EQ_INT(bst_size(t), 7);

    /* Find every value.  Inserted: 5, 3, 7, 1, 4, 6, 9.  Not in tree: 0, 2, 8. */
    for (int i = 0; i < 10; i++) {
        int key = i;
        bst_node *n = bst_find(t, &key);
        if (i == 0 || i == 2 || i == 8) {
            ASSERT_TRUE(n == NULL);
        } else {
            ASSERT_TRUE(n != NULL);
            ASSERT_EQ_INT(extract_int(n->payload), i);
        }
    }
    bst_destroy(t, NULL);
}

static void test_inorder_sorted(void) {
    bst *t = bst_create(bst_cmp_int);
    int vs[] = { 7, 3, 5, 1, 9, 4, 6 };
    for (size_t i = 0; i < sizeof(vs)/sizeof(vs[0]); i++)
        bst_insert(t, &vs[i]);

    int out[16] = {0};
    int *p = out;
    bst_inorder(t, visit_to_array, &p);
    int prev = -1;
    int count = 0;
    for (int i = 0; i < 7; i++) {
        ASSERT_TRUE(out[i] >= prev);
        prev = out[i];
        count++;
    }
    ASSERT_EQ_INT(count, 7);
    ASSERT_EQ_INT(out[0], 1);
    ASSERT_EQ_INT(out[6], 9);
    bst_destroy(t, NULL);
}

static void test_height(void) {
    bst *t = bst_create(bst_cmp_int);
    /* Empty tree. */
    ASSERT_EQ_INT(bst_height(t), 0);
    /* Single node. */
    int v = 1; bst_insert(t, &v);
    ASSERT_EQ_INT(bst_height(t), 1);
    /* Sorted insertion -> degenerate (height = n). */
    bst_destroy(t, NULL);
    t = bst_create(bst_cmp_int);
    int vs[16];
    for (int i = 0; i < 16; i++) { vs[i] = i; bst_insert(t, &vs[i]); }
    ASSERT_EQ_INT(bst_height(t), 16);
    bst_destroy(t, NULL);
}

static void test_remove_leaf_one_child_two_children(void) {
    bst *t = bst_create(bst_cmp_int);

    /* Build a tree where 3 has two children: left=1, right=5.
     * Insertion order matters; use: 3 (root), 1 (left), 5 (right),
     * 0, 2 (children of 1), 4, 6 (children of 5).
     * Tree:
     *        3
     *      /   \
     *     1     5
     *    / \   / \
     *   0   2 4   6
     */
    int vs[] = { 3, 1, 5, 0, 2, 4, 6 };
    for (size_t i = 0; i < sizeof(vs)/sizeof(vs[0]); i++)
        bst_insert(t, &vs[i]);
    ASSERT_EQ_INT(bst_size(t), 7);

    /* Remove a leaf (0). */
    int key = 0;
    ASSERT_TRUE(bst_remove(t, &key, NULL));
    ASSERT_EQ_INT(bst_size(t), 6);
    ASSERT_TRUE(bst_find(t, &key) == NULL);

    /* Remove a one-child node (1): after removing 0, 1 has only
     * right=2. */
    key = 1; ASSERT_TRUE(bst_remove(t, &key, NULL));
    ASSERT_EQ_INT(bst_size(t), 5);

    /* Remove a two-child node (3): has left=2, right=5.
     * In-order successor is 4. After remove, root should be 4. */
    key = 3; ASSERT_TRUE(bst_remove(t, &key, NULL));
    ASSERT_TRUE(bst_find(t, &key) == NULL);
    bst_node *root = t->root;
    ASSERT_TRUE(root != NULL);
    ASSERT_EQ_INT(extract_int(root->payload), 4);
    /* All other keys still present. */
    for (int i = 0; i < 7; i++) {
        if (i == 0 || i == 1 || i == 3) continue;
        ASSERT_TRUE(bst_find(t, &i) != NULL);
    }
    ASSERT_EQ_INT(bst_size(t), 4);
    bst_destroy(t, NULL);
}

static void test_remove_with_free(void) {
    bst *t = bst_create(bst_cmp_int);
    /* Each payload is its own malloc'd int. The tree takes ownership
     * via the free callback, so we don't manually free them later. */
    int *p[5];
    for (int i = 0; i < 5; i++) {
        p[i] = (int *)malloc(sizeof(int));
        *p[i] = i;
        ASSERT_TRUE(bst_insert(t, p[i]));
    }
    free_called_with = 0;
    int key = 2;
    ASSERT_TRUE(bst_remove(t, &key, counting_free));
    ASSERT_EQ_INT(free_called_with, 1);
    bst_destroy(t, counting_free);
    ASSERT_EQ_INT(free_called_with, 5);
}

static void test_str_keys(void) {
    bst *t = bst_create(bst_cmp_str);
    char *keys[] = { (char *)"delta", (char *)"alpha", (char *)"charlie",
                     (char *)"bravo", (char *)"echo" };
    for (int i = 0; i < 5; i++) bst_insert(t, &keys[i]);
    char *q = (char *)"alpha";
    bst_node *n = bst_find(t, &q);
    ASSERT_TRUE(n != NULL);
    ASSERT_EQ_STR(*(char **)n->payload, "alpha");
    char *out[5]; int *p = (int *)out; (void)p;
    int cnt = 0;
    bst_inorder(t, visit_to_array, &p);   /* misuse, but let's just count */
    bst_destroy(t, NULL);
    (void)cnt;
}

/* --- main ---------------------------------------------------------- */

int main(void) {
    test_create_destroy();
    test_null_safety();
    test_insert_find();
    test_inorder_sorted();
    test_height();
    test_remove_leaf_one_child_two_children();
    test_remove_with_free();
    test_str_keys();

    printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
