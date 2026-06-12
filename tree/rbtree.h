#ifndef WSB_TREE_H
#define WSB_TREE_H

#include <stddef.h>

/*
* 因为rb_node是long对齐的，所以其地址的低3位为0，
* 这里由其低3位中的第一位来表示颜色
*/
struct rb_node {
    unsigned long parent_color;
    struct rb_node *left, *right;
} __attribute__((aligned(sizeof(long))));

struct rb_root {
    struct rb_node *rb_node;
};

struct rb_root_cached {
    struct rb_root root;
    struct rb_node *leftmost;
};

#ifndef true
#define true 1
#endif

#define RB_ROOT (struct rb_root) { NULL, }
#define RB_ROOT_CACHED (struct rb_root_cached) { { NULL, }, NULL }

#define RB_RED      0
#define RB_BLACK    1

#define __rb_parent(pc) ((struct rb_node *)((pc) & ~3))
#define __rb_color(pc) ((pc) & 1)
#define __rb_is_black(pc) __rb_color(pc)
#define __rb_is_red(pc) (!__rb_color(pc))

#define rb_parent(rb) __rb_parent((rb)->parent_color)
#define rb_red_parent(rb) ((struct rb_node *)((rb)->parent_color)) // 明确rb是红色节点时，快速获取其父节点
#define rb_color(rb) __rb_color((rb)->parent_color)
#define rb_is_black(rb) __rb_is_black((rb)->parent_color)
#define rb_is_red(rb) __rb_is_red((rb)->parent_color)

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
    rb->parent_color = rb_color(rb) | (unsigned long)p;
}

static inline void rb_set_parent_color(struct rb_node *rb, struct rb_node *p, int color)
{
    rb->parent_color = (unsigned long)p | color;
}


// 增加时

#endif
