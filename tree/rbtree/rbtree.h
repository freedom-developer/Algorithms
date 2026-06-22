#ifndef WSB_TREE_H
#define WSB_TREE_H

#include <stddef.h>
#include <sys/types.h>

#ifndef true
#define true 1
#endif

/*
 * 因为rb_node是long对齐的，所以其地址的低3位为0，
 * 这里由其低3位中的第一位来表示颜色
 */
struct rb_node {
    unsigned long parent_color;
    struct rb_node *left, *right;
} __attribute__((aligned(sizeof(long))));
#define RB_RED      0
#define RB_BLACK    1
#define rb_entry(node, type, member) (type *)((unsigned long)(node) - (unsigned long)(&((type *)0)->member))

// 节点置空操作： parent为其自身
#define RB_EMPTY_NODE(node) ((node)->parent_color == (unsigned long)node)
#define RB_CLEAR_NODE(node) ((node)->parent_color = (unsigned long)node)

struct rb_root {
    struct rb_node *rb_node;
};
#define RB_ROOT (struct rb_root) { NULL, }

struct rb_root_cached {
    struct rb_root root;
    struct rb_node *leftmost;
};
#define RB_ROOT_CACHED (struct rb_root_cached) { { NULL, }, NULL }

#define __rb_parent(pc) ((struct rb_node *)((pc) & ~3UL))
#define __rb_color(pc) ((pc) & 1UL)
#define __rb_is_black(pc) __rb_color(pc)
#define __rb_is_red(pc) (!__rb_color(pc))

#define rb_parent(rb) __rb_parent((rb)->parent_color)
#define rb_red_parent(rb) ((struct rb_node *)((rb)->parent_color)) // 明确rb是红色节点时，快速获取其父节点
#define rb_color(rb) __rb_color((rb)->parent_color)
#define rb_is_black(rb) __rb_is_black((rb)->parent_color)
#define rb_is_red(rb) __rb_is_red((rb)->parent_color)



// 不改变颜色，设置其父节点
static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
    rb->parent_color = rb_color(rb) | (unsigned long)p;
}

static inline void rb_set_color(struct rb_node *rb, unsigned long color)
{
    rb->parent_color = (rb->parent_color & ~3UL) | (color & 1UL);
}

// 设置其父节点及其颜色
static inline void rb_set_parent_color(struct rb_node *rb, struct rb_node *p, int color)
{
    rb->parent_color = (unsigned long)p | color;
}

// n 子树的最左边的节点，通常也是最小的节点
static inline struct rb_node *rb_leftmost_of_node(struct rb_node *n)
{
    if (!n) 
        return NULL;
    while (n->left) 
        n = n->left;
    return n;
}

// n 子树的最右边的节点，通常也是最大的节点
static inline struct rb_node *rb_rightmost_of_node(struct rb_node *n)
{
    if (!n) 
        return NULL;
    while (n->right)
        n = n->right;
    return n;
}

static inline struct rb_node *rb_leftmost(struct rb_root *root)
{
    return rb_leftmost_of_node(root->rb_node);
}

static inline struct rb_node *rb_rightmost(struct rb_root *root)
{
    return rb_rightmost_of_node(root->rb_node);
}

// 与rb_leftmost 同义
static inline struct rb_node *rb_first(struct rb_root *root)
{
    return rb_leftmost(root);
}

// 与rb_rightmost 同义
static inline struct rb_node *rb_last(struct rb_root *root)
{
    return rb_rightmost(root);
}

// 后继：大于 node 的最小的节点
static inline struct rb_node *rb_next(struct rb_node *node)
{
    if (RB_EMPTY_NODE(node)) 
        return NULL;
    if (node->right) 
        return rb_leftmost_of_node(node->right);

    struct rb_node *parent;
    while ((parent = rb_parent(node)) && node == parent->right)
        node = parent;
    return parent;
}

// 前驱：小于 node 的最大的节点
static inline struct rb_node *rb_prev(struct rb_node *node)
{
    struct rb_node *parent;
    if (RB_EMPTY_NODE(node)) 
        return NULL;
    if (node->left) 
        return rb_rightmost_of_node(node->left);
    
    while ((parent = rb_parent(node)) && node == parent->left)
        node = parent;
    return parent;
}


struct rb_augment_callbacks {
    // 往 node 往上重新计算 扩展信息 , 直到 stop
    void (*propagate)(struct rb_node *node, struct rb_node *stop);
    // 当 new 代替 old 树位置时，把 old 的 扩展信息 复制到 new 中
    void (*copy)(struct rb_node *old, struct rb_node *newn);
    // 红黑树旋转更新受影响的节点 扩展信息
    void (*rotate)(struct rb_node *old, struct rb_node *newn);
};

void rb_insert_augmented(struct rb_root *root, struct rb_node *node, struct rb_augment_callbacks *augment);

static inline void rb_insert_color(struct rb_root *root, struct rb_node *node)
{
    rb_insert_augmented(root, node, NULL);
}

static inline void rb_insert_color_cached(struct rb_root_cached *root, struct rb_node *node, int leftmost)
{
    if (leftmost)
        root->leftmost = node;
    rb_insert_color(&root->root, node);
}

static inline void rb_link_node(struct rb_node *node, struct rb_node *parent, struct rb_node **link)
{
    *link = node;
    node->left = node->right = NULL;
    node->parent_color = (unsigned long)parent; // 默认是红色
}


void rb_erase_augmented(struct rb_root *root, struct rb_node *n, struct rb_augment_callbacks *augment);

static inline void rb_erase(struct rb_root *root, struct rb_node *n)
{
    rb_erase_augmented(root, n, NULL);
}

static inline struct rb_node *rb_erase_cached(struct rb_root_cached *root, struct rb_node *node)
{
    struct rb_node *leftmost = NULL;
    if (root->leftmost == node)
        leftmost = root->leftmost = rb_next(node);
    rb_erase(&root->root, node);
    return leftmost;
}

#endif
