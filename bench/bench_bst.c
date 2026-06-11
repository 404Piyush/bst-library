/* bench/bench_bst.c
 *
 * Microbenchmark: insert N keys, look up each, then delete each (in
 * a different shuffled order), and time each phase.
 *
 * Pass N as argv[1] (default 100000). Reports ops/sec for each phase
 * and the final tree height.
 */
#include "bst.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static int cmp_int(const void *a, const void *b) {
    int ai = *(const int *)a, bi = *(const int *)b;
    return (ai > bi) - (ai < bi);
}

/* Fisher-Yates shuffle with an explicit seed. */
static void shuffle(int *a, int n, unsigned int seed) {
    srand(seed);
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

int main(int argc, char **argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 100000;

    int *keys = (int *)malloc((size_t)n * sizeof(int));
    for (int i = 0; i < n; i++) keys[i] = i + 1;
    shuffle(keys, n, 42);

    bst *t = bst_create(cmp_int);
    if (!t) { perror("bst_create"); return 1; }

    double t0 = now_sec();
    for (int i = 0; i < n; i++) bst_insert(t, &keys[i]);
    double t1 = now_sec();
    printf("insert  %d : %7.3f s  (%9.0f ops/s)  height=%d\n",
           n, t1 - t0, n / (t1 - t0), bst_height(t));

    int hits = 0;
    double t2 = now_sec();
    for (int i = 0; i < n; i++) if (bst_find(t, &keys[i])) hits++;
    double t3 = now_sec();
    printf("find    %d : %7.3f s  (%9.0f ops/s)  hits=%d\n",
           n, t3 - t2, n / (t3 - t2), hits);

    /* Use a SEPARATE shuffled order for removal. */
    int *rm_keys = (int *)malloc((size_t)n * sizeof(int));
    memcpy(rm_keys, keys, (size_t)n * sizeof(int));
    shuffle(rm_keys, n, 43);  /* different seed -> different order */
    int removed = 0;
    double t4 = now_sec();
    for (int i = 0; i < n; i++) {
        if (bst_remove(t, &rm_keys[i], NULL)) removed++;
    }
    double t5 = now_sec();
    printf("remove  %d : %7.3f s  (%9.0f ops/s)  removed=%d\n",
           n, t5 - t4, n / (t5 - t4), removed);

    printf("size after all phases: %zu (expected 0)\n", bst_size(t));
    bst_destroy(t, NULL);
    free(keys);
    free(rm_keys);
    return 0;
}
