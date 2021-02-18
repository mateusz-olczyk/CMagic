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
    size_t tree_size;
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
        .tree_size = 0,
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
    if (T2) {
        T2->parent = y;
    }

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
    if (T2) {
        T2->parent = x;
    }

    y->subtree_height = CMAGIC_UTILS_MAX(_get_height(y->left_kid), _get_height(y->right_kid)) + 1;
    x->subtree_height = CMAGIC_UTILS_MAX(_get_height(x->left_kid), _get_height(x->right_kid)) + 1;
}

static int _get_balance(const tree_node_t *node) {
    return node ? _get_height(node->left_kid) - _get_height(node->right_kid) : 0;
}

static cmagic_avl_tree_insert_result_t _internal_insert(tree_descriptor_t *tree,
                                                        tree_node_t *node_parent,
                                                        tree_node_t **node_ptr,
                                                        const void *key, void *value) {
    assert(tree);
    assert(node_ptr);
    assert(key);

    if (!*node_ptr) {
        *node_ptr = _new_node(tree, node_parent, key, value);
        tree->tree_size++;
        return (cmagic_avl_tree_insert_result_t) {
            .inserted_or_existing = (cmagic_avl_tree_iterator_t)*node_ptr,
            .already_exists = false
        };
    }

    tree_node_t *node = *node_ptr;
    int comparison_result = tree->key_comparator(key, node->key);
    if (comparison_result == 0) {
        return (cmagic_avl_tree_insert_result_t) {
            .inserted_or_existing = (cmagic_avl_tree_iterator_t)node,
            .already_exists = true
        };
    }

    cmagic_avl_tree_insert_result_t insert_result = comparison_result < 0
        ? _internal_insert(tree, node, &node->left_kid, key, value)
        : _internal_insert(tree, node, &node->right_kid, key, value);
    if (!insert_result.inserted_or_existing || insert_result.already_exists) {
        return insert_result;
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
        return insert_result;
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
        return insert_result;
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
        return insert_result;
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
        return insert_result;
    }

    // Already balanced
    return insert_result;
}

cmagic_avl_tree_insert_result_t
cmagic_avl_tree_insert(void **avl_tree, const void *key, void *value) {
    assert(key);
    tree_descriptor_t *tree = _get_avl_tree_descriptor(avl_tree);
    return _internal_insert(tree, NULL, &tree->root, key, value);
}

static void _internal_erase(tree_descriptor_t *tree, tree_node_t **node_ptr, const void *key) {
    assert(tree);
    assert(node_ptr);
    assert(key);

    if (!*node_ptr) {
        return;
    }

    tree_node_t *node = *node_ptr;
    int comparison_result = tree->key_comparator(key, node->key);
    if (comparison_result < 0) {
        _internal_erase(tree, &node->left_kid, key);
    } else if (comparison_result > 0) {
        _internal_erase(tree, &node->right_kid, key);
    } else {
        if (node->left_kid && node->right_kid) {
            tree_node_t *successor =
                (tree_node_t *)cmagic_avl_tree_iterator_next((cmagic_avl_tree_iterator_t)node);
            assert(successor);
            assert(!successor->left_kid);
            successor->parent = node->parent;
            successor->left_kid = node->left_kid;
            *node_ptr = successor;
            tree->alloc_packet->free_function(node);
            tree->tree_size--;
        } else {
            tree_node_t *kid = node->left_kid ? node->left_kid : node->right_kid;
            if (kid) {
                kid->parent = node->parent;
            }
            *node_ptr = kid;
            tree->alloc_packet->free_function(node);
            tree->tree_size--;
        }
    }

    node = *node_ptr;
    if (!node) {
        return;
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
        return;
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
        return;
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
        return;
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
        return;
    }

    // Already balanced
}

void
cmagic_avl_tree_erase(void **avl_tree, const void *key) {
    assert(key);
    tree_descriptor_t *tree = _get_avl_tree_descriptor(avl_tree);
    _internal_erase(tree, &tree->root, key);
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
cmagic_avl_tree_clear(void **avl_tree) {
    tree_descriptor_t *tree = _get_avl_tree_descriptor(avl_tree);
    _internal_free(tree, tree->root);
    tree->root = NULL;
    tree->tree_size = 0;
}

size_t
cmagic_avl_tree_size(void **avl_tree) {
    tree_descriptor_t *tree = _get_avl_tree_descriptor(avl_tree);
    return tree->tree_size;
}

void
cmagic_avl_tree_free(void **avl_tree) {
    tree_descriptor_t *tree = _get_avl_tree_descriptor(avl_tree);
    _internal_free(tree, tree->root);
    tree->alloc_packet->free_function(tree);
}

cmagic_avl_tree_iterator_t
cmagic_avl_tree_first(void **avl_tree) {
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
cmagic_avl_tree_last(void **avl_tree) {
    tree_descriptor_t *tree = _get_avl_tree_descriptor(avl_tree);
    if (!tree->root) {
        return NULL;
    }

    tree_node_t *node = tree->root;
    while (node->right_kid) {
        node = node->right_kid;
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

cmagic_avl_tree_iterator_t
cmagic_avl_tree_iterator_prev(cmagic_avl_tree_iterator_t iterator) {
    if (!iterator) {
        return NULL;
    }

    tree_node_t *node = (tree_node_t *) iterator;
    if (node->left_kid) {
        node = node->left_kid;
        while (node->right_kid) {
            node = node->right_kid;
        }
        return (cmagic_avl_tree_iterator_t)node;
    }

    while (node->parent && node->parent->left_kid == node) {
        node = node->parent;
    }
    return (cmagic_avl_tree_iterator_t)node->parent;
}

cmagic_avl_tree_iterator_t
cmagic_avl_tree_find(void **avl_tree, const void *key) {
    assert(key);
    tree_descriptor_t *tree = _get_avl_tree_descriptor(avl_tree);
    tree_node_t *node = tree->root;

    while (node) {
        int comparison_result = tree->key_comparator(key, node->key);
        if (comparison_result < 0) {
            node = node->left_kid;
        } else if (comparison_result > 0) {
            node = node->right_kid;
        } else {
            return (cmagic_avl_tree_iterator_t)node;
        }
    }

    return NULL;
}
