#include "rbtree.h"

// node 必须是一个红色节点，且已经按二叉搜索树特性插入到root中
void _rb_insert(struct rb_node *node, struct rb_node *root, 
    void (*augment_rotate)(struct rb_node *old, struct rb_node *new))
{
    struct rb_node *parent = rb_red_parent(node), *pparent, *tmp;
    while (true) {
        if (!parent) { // 当node无父节点，表明node就是树根，此时只要简单将node的颜色设为黑色
            rb_set_parent_color(node, NULL, RB_BLACK);
            break;
        }
        if (rb_is_black(parent)) // 如果父是黑色节点，则直接退出
            break;
        
        // parent是红色节点, pparent一定是黑色
        pparent = rb_red_parent(parent);
        if (parent != (tmp = pparent->right)) { // parent == pprent->left
            /*
                    G
                   / \
                  P   U 红色
                 /
                n , n也可能为P.left，不过无所谓，只要n的父节点及叔节点都为红色，则直接将父和叔节点置黑，同时将祖父置红，并向上递归
            */
            if (tmp && rb_is_red(tmp)) { // 右
                rb_set_parent_color(tmp, pparent, RB_BLACK);
                rb_set_parent_color(parent, pparent, RB_BLACK);
                node = pparent;
                parent = rb_parent(node);
                rb_set_parent_color(node, parent, RB_RED); 
                continue;
            }
            
            // 此时n和P均为红色， U为黑色或空， G一定是黑色
            tmp = parent->right;
            if (node == tmp) {
                /* 此时为下面的情况
                     G                          G
                    / \                        / \   
                   P   U 黑色或为空 ->         n   U 
                    \                        /
                     n                      P
                执行左旋
                */
                tmp = node->left; // node为红色，所以tmp如果存在，则一定是黑色
                parent->right = tmp;
                if (tmp)
                    rb_set_parent_color(tmp, parent, RB_BLACK);

                // 如下操作放在Case 3中执行
                // pparent->left = node;
                // rb_set_parent_color(node, pparent, RB_BLACK);

                node->left = parent;
                rb_set_parent_color(parent, node, RB_RED);

                parent = node;
                tmp = node->right;
            }

            /*
               Case 3
                    G                    p
                   / \        ->        / \
                  p   u                n   G
                 /                          \
                n                            u
                向右放置
            */
            pparent->left = tmp;
            if (tmp)
                rb_set_parent_color(tmp, pparent, RB_BLACK);

            parent->parent_color = pparent->parent_color; 
            

            parent->right = pparent;
            rb_set_parent_color(pparent, parent, RB_RED);



        } else {

        }
    }

}
