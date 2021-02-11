#include <assert.h>
#include <stdint.h>
#include "cmagic/utils.h"
#include "avl_tree.h"

#ifndef NDEBUG
static const int_least32_t AVL_TREE_MAGIC_VALUE = 'T' << 24 | 'R' << 16 | 'E' << 8 | 'E';
#endif

typedef struct tree_node {
    const void *key;
    void *value;
    struct tree_node *parent;
    struct tree_node *left_kid;
    struct tree_node *right_kid;
    int subtree_height;
} tree_node_t;

typedef struct {
#ifndef NDEBUG
    int_least32_t magic_value;
#endif
    cmagic_avl_tree_key_comparator_t key_comparator;
    const cmagic_memory_alloc_packet_t *alloc_packet;
    tree_node_t *root;
} tree_descriptor_t;

void **
cmagic_avl_tree_new(cmagic_avl_tree_key_comparator_t key_comparator,
                    const cmagic_memory_alloc_packet_t *alloc_packet) {
    assert(key_comparator);
    assert(alloc_packet);

    tree_descriptor_t *tree_descriptor =
        (tree_descriptor_t *) alloc_packet->malloc_function(sizeof(tree_descriptor_t));
    if (!tree_descriptor) {
        return NULL;
    }

    *tree_descriptor = (tree_descriptor_t) {
#ifndef NDEBUG
        .magic_value = AVL_TREE_MAGIC_VALUE,
#endif
        .key_comparator = key_comparator,
        .alloc_packet = alloc_packet,
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

static int _get_height(const tree_node_t *node) {
    return node ? node->subtree_height : 0;
}

static tree_node_t *_new_node(tree_descriptor_t *tree, tree_node_t *parent, const void *key,
                              void *value) {
    assert(tree);
    assert(key);
    
    tree_node_t *new_node = (tree_node_t *)tree->alloc_packet->malloc_function(sizeof(tree_node_t));
    if (!new_node) {
        return NULL;
    }

    *new_node = (tree_node_t) {
        .key = key,
        .value = value,
        .parent = parent,
        .left_kid = NULL,
        .right_kid = NULL,
        .subtree_height = 1
    };

    return new_node;
}

/*
 *       y                x
 *      / \              /  \
 *     x   T3  ------>  T1   y
 *    / \                   / \
 *   T1  T2               T2  T3
 */
static void _rotate_right(tree_node_t **y_ptr) {
    tree_node_t *y = *y_ptr;
    tree_node_t *x = y->left_kid;
    tree_node_t *T2 = x->right_kid;

    x->right_kid = y;
    x->parent = y->parent;
    y->left_kid = T2;
    y->parent = x;
    *y_ptr = x;
    T2->parent = y;

    y->subtree_height = CMAGIC_UTILS_MAX(_get_height(y->left_kid), _get_height(y->right_kid)) + 1;
    x->subtree_height = CMAGIC_UTILS_MAX(_get_height(x->left_kid), _get_height(x->right_kid)) + 1;
}

/*
 *     x                    y
 *    /  \                 / \
 *   T1   y    ------>    x   T3
 *       / \             / \
 *     T2  T3           T1  T2
 */
static void _rotate_left(tree_node_t **x_ptr) {
    tree_node_t *x = *x_ptr;
    tree_node_t *y = x->right_kid;
    tree_node_t *T2 = y->left_kid;

    y->left_kid = x;
    y->parent = x->parent;
    x->right_kid = T2;
    x->parent = y;
    *x_ptr = y;
    T2->parent = x;

    y->subtree_height = CMAGIC_UTILS_MAX(_get_height(y->left_kid), _get_height(y->right_kid)) + 1;
    x->subtree_height = CMAGIC_UTILS_MAX(_get_height(x->left_kid), _get_height(x->right_kid)) + 1;
}

static int _get_balance(const tree_node_t *node) {
    return node ? _get_height(node->left_kid) - _get_height(node->right_kid) : 0;
}

static bool _internal_insert(tree_descriptor_t *tree, tree_node_t *node_parent,
                             tree_node_t **node_ptr, const void *key, void *value) {
    assert(tree);
    assert(node_ptr);

    if (!*node_ptr) {
        *node_ptr = _new_node(tree, node_parent, key, value);
        return (bool)(uintptr_t)(*node_ptr);
    }

    tree_node_t *node = *node_ptr;
    int comparison_result = tree->key_comparator(key, node->key);
    if (comparison_result == 0 ||
        (comparison_result < 0 && !_internal_insert(tree, node, &node->left_kid, key, value)) ||
        (comparison_result > 0 && !_internal_insert(tree, node, &node->right_kid, key, value))) {
        return false;
    }

    node->subtree_height = 1 + CMAGIC_UTILS_MAX(_get_height(node->left_kid),
                                                _get_height(node->right_kid));

    // Handle balance violation cases, see https://en.wikipedia.org/wiki/AVL_tree#Rebalancing
    int balance = _get_balance(node);

    /* Left-Left case
     *         z
     *         / \
     *        y   T4
     *       / \
     *      x   T3
     *     / \
     *   T1   T2
     */
    if (balance > 1 && tree->key_comparator(key, node->left_kid->key) < 0) {
        _rotate_right(node_ptr);
        return true;
    }

    /* Right-Right case
     *     z
     *    /  \
     *   T1   y
     *       /  \
     *      T2   x
     *          / \
     *        T3  T4
     */
    if (balance < -1 && tree->key_comparator(key, node->right_kid->key) > 0) {
        _rotate_left(node_ptr);
        return true;
    }

    /* Left-Right case
     *        z
     *       / \
     *      y   T4
     *     / \
     *   T1   x
     *       / \
     *     T2   T3
     */
    if (balance > 1 && tree->key_comparator(key, node->left_kid->key) > 0) {
        _rotate_left(&node->left_kid);
        _rotate_right(node_ptr);
        return true;
    }

    /* Right-Left case
     *      z
     *     / \
     *   T1   y
     *       / \
     *      x   T4
     *     / \
     *   T2   T3
     */
    if (balance < -1 && tree->key_comparator(key, node->right_kid->key) < 0) {
        _rotate_right(&node->right_kid);
        _rotate_left(node_ptr);
        return true;
    }

    // Already balanced
    return true;
}

bool
cmagic_avl_tree_insert(void **avl_tree, const void *key, void *value) {
    tree_descriptor_t *tree = _get_avl_tree_descriptor(avl_tree);
    return _internal_insert(tree, NULL, &tree->root, key, value);
}

static void _internal_free(tree_descriptor_t *tree, tree_node_t *node) {
    assert(tree);
    if (!node) {
        return;
    }

    _internal_free(tree, node->left_kid);
    _internal_free(tree, node->right_kid);
    tree->alloc_packet->free_function(node);
}

void
cmagic_avl_tree_free(void **avl_tree) {
    tree_descriptor_t *tree = _get_avl_tree_descriptor(avl_tree);
    _internal_free(tree, tree->root);
}

cmagic_avl_tree_iterator_t
cmagic_avl_tree_begin(void **avl_tree) {
    tree_descriptor_t *tree = _get_avl_tree_descriptor(avl_tree);
    if (!tree->root) {
        return NULL;
    }

    tree_node_t *node = tree->root;
    while (node->left_kid) {
        node = node->left_kid;
    }

    return (cmagic_avl_tree_iterator_t)node;
}

cmagic_avl_tree_iterator_t
cmagic_avl_tree_iterator_next(cmagic_avl_tree_iterator_t iterator) {
    if (!iterator) {
        return NULL;
    }

    tree_node_t *node = (tree_node_t *) iterator;
    if (node->right_kid) {
        node = node->right_kid;
        while (node->left_kid) {
            node = node->left_kid;
        }
        return (cmagic_avl_tree_iterator_t)node;
    }

    while (node->parent && node->parent->right_kid == node) {
        node = node->parent;
    }
    return (cmagic_avl_tree_iterator_t)node->parent;
}
