#ifndef BST_CONFIG_H
#define BST_CONFIG_H

/* libbst is a header-only configuration of inlining behaviour.
 * If BST_HEADER_ONLY is defined, bst.c is not compiled; instead all
 * functions are `static inline` in the header. This is useful for
 * single-translation-unit projects. Otherwise compile src/bst.c.
 */
#ifdef BST_HEADER_ONLY
#define BST_INLINE static inline
#else
#define BST_INLINE
#endif

#endif
