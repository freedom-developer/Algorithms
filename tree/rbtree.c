#include "rbtree.h"

// node 必须是一个红色节点，且已经按二叉搜索树特性插入到root中
static void _rb_insert(struct rb_node *me, struct rb_root *root, 
    void (*augment_rotate)(struct rb_node *old, struct rb_node *new))
{
    struct rb_node *parent = rb_red_parent(me), *pparent, *uncle, *brother;
    while (true) {
        if (!parent) { 
            /*
             * me 为树根节点时，将 me 设为黑色，完成
             */
            rb_set_parent_color(me, NULL, RB_BLACK);
            break;
        }

        /*
         * me 为红色，parent 为黑色，完成
         */
        if (rb_is_black(parent))
            break;
        
        pparent = rb_red_parent(parent);
        if (parent != (uncle = pparent->right)) {
            /* p 为 g 的 left:
             *        g
             *       /
             *      p
             */
            if ( uncle && rb_is_red(uncle)) {
                /* Case 1: parent 为pparent 的左节点
                 *        g
                 *       / \
                 *      p   u
                 *     /
                 *    m 
                 * 颜色分析：
                 *  m 为红色，p 为红色, u 为红色, g为黑色
                 * 操作步骤：     
                 *  将p, u 置黑，g 置红；然后 将 m 设为g并重新计算p, 继续下一轮
                 */
                rb_set_color(parent, RB_BLACK);
                rb_set_color(uncle, RB_BLACK);
                rb_set_color(pparent, RB_RED);

                me = pparent;
                parent = rb_parent(me);
                continue;
            }
            
            if (me == (brother = parent->right)) {
                /* case 2:
                 *    g                          
                 *   / \                         
                 *  p   u         
                 *   \                     
                 *    m                    
                 * 颜色分析：
                 *  m, p 为红色，g 为黑色，u 为null 或 黑色
                 * 操作步骤：
                 *  左旋转 p，生成如下树状
                 *       g
                 *      /
                 *     m
                 *    /
                 *   p
                 *  各个节点的颜色不变，只改变父子关系：
                 *  左旋后：
                 *      p.right = m.left;
                 *      if (m.left) m.left.parent = p
                 *      p.parent = m; m.left = p
                 *      
                 * 因为如下关系会被 Case 3 打破，所以这里可以不做这步操作
                 *      m.parent = pp; pp.left = m
                 *  
                 * 增强树的调整：
                 *   因为 p 和 m 的位置变换了，对于某个增强树来说，
                 *   需要调整节点额外的成员，即调用调整函数 augment_rotate(parent, me)
                 * 
                 * 为 Case 3作准备：
                 *   将 me 切换成 parent 位置
                 */
                parent->right = me->left;
                if (me->left)
                    rb_set_parent(me->left, parent);
                
                rb_set_parent(parent, me);
                me->left = parent;

                // rb_set_parent(me, pparent);
                // pparent->left = me;

                if (augment_rotate)
                    augment_rotate(parent, me);

                parent = me;
                brother = parent->right;
            }

            /*
             *  Case 3:
             *       g                    
             *      / \          
             *     p   u                   
             *    /                          
             *   m                            
             * 颜色分布：
             *  m, p 为红色, g 为黑色，u 为黑色为 null
             * 操作步骤：
             *  1. p置黑色, g置红色
             *  2. 对g 进行右旋转
             *      g.left = p.right; if (p.right)p.right.parent = g
             *      p.parent = g.parent; g.parent.[left|right] = parent
             *      p.right = g; g.parent = p
             *  3. 因为旋转高的了 p, g 的位置，所以要调用增强调整函数
             *  4. 函数结束
             * 右旋结果：
             *       p
             *      / \
             *     m   g
             */
            rb_set_color(parent, RB_BLACK);
            rb_set_color(pparent, RB_RED);

            pparent->left = brother;
            if (brother)
                rb_set_parent(brother, pparent);
            
            struct rb_node *ppparent = rb_parent(pparent);
            rb_set_parent(parent, ppparent);

            if (ppparent) {
                if (pparent == ppparent->left)
                    ppparent->left = parent;
                else
                    ppparent->right = parent;
            } else
                root->rb_node = parent;

            parent->right = pparent;
            rb_set_parent(pparent, parent);

            if (augment_rotate)
                augment_rotate(pparent, parent);
            break;
        } else {
            uncle = pparent->left;
            if (uncle && rb_is_red(uncle)) { // Case 1
                /* Case 1:
                 *      g
                 *     / \
                 *    u   p
                 *         \
                 *          m
                 */
                rb_set_color(parent, RB_BLACK);
                rb_set_color(uncle, RB_BLACK);
                rb_set_color(pparent, RB_RED);

                me = pparent;
                parent = rb_parent(me);
                continue;
            }
            if (me == (brother = parent->left)) {
                /* Case 2:
                 *       g
                 *      / \
                 *     u   p
                 *        /
                 *       m
                 */
                parent->left = me->right;
                if (me->right)
                    rb_set_parent(me->right, parent);
                
                me->right = parent;
                rb_set_parent(parent, me);

                // pparent->right = me;
                // rb_set_parent(me, pparent);

                if (augment_rotate)
                    augment_rotate(parent, me);

                parent = me;
            }

            /* Case 3:
             *       g
             *      / \
             *     u   p
             *          \
             *           m
             */
            
            rb_set_color(pparent, RB_RED);
            rb_set_color(parent, RB_BLACK);

            pparent->right = parent->left;
            if (parent->left)
                rb_set_parent(parent->left, pparent);
            
            struct rb_node *ppparent = rb_parent(pparent);
            rb_set_parent(parent, ppparent);
            if (ppparent) {
                if (pparent == ppparent->left)
                    ppparent->left = parent;
                else
                    ppparent->right = parent;
            } else 
                root->rb_node = parent;

            parent->left = pparent;
            rb_set_parent(pparent, parent);

            if (augment_rotate)
                augment_rotate(pparent, parent);
            break;
        }
    }

}


void rb_insert_color(struct rb_node *node, struct rb_root *root)
{
    _rb_insert(node, root, NULL);
}

struct rb_node *rb_first(const struct rb_root *root)
{
    struct rb_node *n = root->rb_node;
    if (!n) return NULL;
    while (n->left)
        n = n->left;
    return n;
}

struct rb_node *rb_last(const struct rb_root *root)
{
    struct rb_node *n = root->rb_node;
    if (!n) return NULL;
    while (n->right)
        n = n->right;
    return ;
}

// 后继：大于 node 的最小的那个节点
struct rb_node *rb_next(const struct rb_node *node)
{
    if (RB_EMPTY_NODE(node)) return NULL;
    if (node->right) {
        node = node->right;
        while (node->left)
            node = node->left;
        return node;
    }
    struct rb_node *parent;
    while ((parent = rb_parent(node)) && node == parent->right)
        node = parent;
    return parent;
}

struct rb_node *rb_prev(const struct rb_node *node)
{
    struct rb_node *parent;
    if (RB_EMPTY_NODE(node)) 
        return NULL;
    if (node->left) {
        node = node->left;
        while (node->right)
            node = node->right;
        return node;
    }
    while ((parent = rb_parent(node)) && node == parent->left)
        node = parent;
    return parent;
}

