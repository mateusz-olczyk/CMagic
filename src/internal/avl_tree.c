#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "cmagic/utils.h"
#include "avl_tree.h"

#ifndef NDEBUG
static const int_least32_t AVL_TREE_MAGIC_VALUE = 'T' << 24 | 'R' << 16 | 'E' << 8 | 'E';
#endif

typedef struct tree_node {
    void *key;
    void *value;
    struct tree_node *left_kid;
    struct tree_node *right_kid;
    int subtree_height;
} tree_node_t;

typedef struct {
#ifndef NDEBUG
    int_least32_t magic_value;
#endif
    const cmagic_memory_alloc_packet_t *alloc_packet;
    size_t key_size;
    size_t value_size;
    tree_node_t *root;
} tree_descriptor_t;

void **
cmagic_avl_tree_new(size_t key_size, size_t value_size,
                    const cmagic_memory_alloc_packet_t *alloc_packet) {
    tree_descriptor_t *tree_descriptor =
        (tree_descriptor_t *) alloc_packet->malloc_function(sizeof(tree_descriptor));
    if (!tree_descriptor) {
        return NULL;
    }

    *tree_descriptor = (tree_descriptor_t) {
#ifndef NDEBUG
        .magic_value = AVL_TREE_MAGIC_VALUE,
#endif
        .alloc_packet = alloc_packet,
        .key_size = key_size,
        .value_size = value_size,
        .root = NULL
    };

    return (void **)&tree_descriptor->root;
}

static tree_descriptor_t *_get_avl_tree_descriptor(void **tree_ptr) {
    assert(tree_ptr);
    tree_descriptor_t *result = (tree_descriptor_t *)(
        (const char *)tree_ptr - offsetof(tree_descriptor_t, root));
    assert(result->magic_value == AVL_TREE_MAGIC_VALUE);
    return result;
}

static int _get_height(tree_node_t *node) {
    return node ? node->subtree_height : 0;
}

static tree_node_t *_allocate_node(tree_descriptor_t *tree_descriptor) {
    tree_node_t *new_node =
        (tree_node_t *) tree_descriptor->alloc_packet->malloc_function(sizeof(tree_node_t));
    if (!new_node) {
        return NULL;
    }

    memset(new_node, 0, sizeof(tree_node_t));
    return new_node;
}

/*
 *       y                x
 *      / \              /  \
 *     x   T3  ------>  T1   y
 *    / \                   / \
 *   T1  T2               T2  T3
 */
static tree_node_t *_rotate_right(tree_node_t *y) {
    tree_node_t *x = y->left_kid;
    tree_node_t *T2 = x->right_kid;
    x->right_kid = y; 
    y->left_kid = T2; 
    y->subtree_height = CMAGIC_UTILS_MAX(_get_height(y->left_kid), _get_height(y->right_kid)) + 1;
    x->subtree_height = CMAGIC_UTILS_MAX(_get_height(x->left_kid), _get_height(x->right_kid)) + 1;
    return x;
}

/*
 *     x                    y
 *    /  \                 / \
 *   T1   y    ------>    x   T3
 *       / \             / \
 *     T2  T3           T1  T2
 */
static tree_node_t *_rotate_left(tree_node_t *x) {
    tree_node_t *y = x->right_kid;
    tree_node_t *T2 = y->left_kid;
    y->left_kid = x;
    x->right_kid = T2;
    y->subtree_height = CMAGIC_UTILS_MAX(_get_height(y->left_kid), _get_height(y->right_kid)) + 1;
    x->subtree_height = CMAGIC_UTILS_MAX(_get_height(x->left_kid), _get_height(x->right_kid)) + 1;
    return y;
}

static int _get_balance(tree_node_t *node) {
    return node ? _get_height(node->left_kid) - _get_height(node->right_kid) : 0;
}
