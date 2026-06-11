/* examples/wordcount.c
 *
 * Read words from stdin (one per line), insert into a BST keyed
 * by the word, then walk in-order and print one word per line.
 * Demonstrates the public bst.h API with the built-in bst_cmp_str.
 */
#include "bst.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void print_word(const void *payload, void *user) {
    (void)user;
    const char *word = *(const char *const *)payload;
    puts(word);
}

int main(void) {
    bst *t = bst_create(bst_cmp_str);
    if (!t) { perror("bst_create"); return 1; }

    char  line[1024];
    char *heap_line;
    while (fgets(line, sizeof(line), stdin)) {
        size_t n = strlen(line);
        while (n > 0 && (line[n-1] == '\n' || line[n-1] == '\r')) line[--n] = '\0';
        if (n == 0) continue;
        heap_line = strdup(line);
        if (!heap_line) { perror("strdup"); break; }
        if (!bst_insert(t, &heap_line)) free(heap_line);   /* dup key */
    }

    printf("--- %zu unique words (sorted) ---\n", bst_size(t));
    bst_inorder(t, print_word, NULL);
    bst_destroy(t, NULL);   /* t owns the strings, free() them */
    return 0;
}
