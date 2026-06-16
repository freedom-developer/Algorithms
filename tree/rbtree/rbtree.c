#include "rbtree.h"

// n 子树的最左边的节点，通常也是最小的节点
inline struct rb_node *rb_leftmost_of_node(struct rb_node *n)
{
    if (!n) 
        return NULL;
    while (n->left) 
        n = n->left;
    return n;
}

// n 子树的最右边的节点，通常也是最大的节点
inline struct rb_node *rb_rightmost_of_node(struct rb_node *n)
{
    if (!n) 
        return NULL;
    while (n->right)
        n = n->right;
    return ;
}

inline struct rb_node *rb_leftmost(struct rb_root *root)
{
    return rb_leftmost_of_node(root->rb_node);
}

inline struct rb_node *rb_rightmost(struct rb_root *root)
{
    return rb_rightmost_of_node(root->rb_node);
}

// 与rb_leftmost 同义
inline struct rb_node *rb_first(const struct rb_root *root)
{
    return rb_leftmost(root);
}

// 与rb_rightmost 同义
inline struct rb_node *rb_last(const struct rb_root *root)
{
    return rb_rightmost(root);
}

// 后继：大于 node 的最小的节点
struct rb_node *rb_next(const struct rb_node *node)
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
struct rb_node *rb_prev(const struct rb_node *node)
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


/* 替换节点: 不改变节点颜色，不作 扩展信息 调整
 *  将new 子树 替换 old 子树
 */
inline void _rb_replace_node(struct rb_root *root, struct rb_node *old, struct rb_node *new)
{
    struct rb_node *parent = rb_parent(old);

    rb_set_parent(new, parent);
    if (parent) {
        if (old == parent->left)
            parent->left = new;
        else
            parent->right = new;
    } else
        root->rb_node = new;
}

/* 左旋 n，不改变任何节点的颜色, 不作 扩展信息 调整
 *        n                   nr
 *       / \                 / \
 *      nl  nr     ->       n   nrr
 *         / \             / \
 *       nrl  nrr         nl  nrl
 * 要求：n 必须有 right
 */
void _rb_left_rotate(struct rb_root *root, struct rb_node *n)
{
    struct rb_node *nr = n->right;

    _rb_replace_node(root, n, nr);

    n->right = nr->left;
    if (nr->left)
        rb_set_parent(nr->left, n);

    nr->left = n;
    rb_set_parent(n, nr);
}

/* 右旋 n, 不改变颜色，不作 扩展信息 调整
 *        n                         nl
 *       / \                       /  \
 *      nl  nr        ->         nll   n
 *     / \                            / \
 *   nll nlr                        nlr  nr
 * 要求：n 必须有 left
 */
void _rb_right_rotate(struct rb_root *root, struct rb_node *n)
{
    struct rb_node *nl = n->left;

    _rb_replace_node(root, n, nl);

    n->left = nl->right;
    if (nl->right)
        rb_set_parent(nl->right, n);
    
    nl->right = n;
    rb_set_parent(n, nl);
}

// n 必须是一个红色节点，且已经按二叉搜索树特性插入到root中，调整红黑颜色属性及其 扩展信息
static void _rb_insert_color_augmented(struct rb_root *root, struct rb_node *n,
    void (*augment_rotate)(struct rb_node *old, struct rb_node *new))
{
    struct rb_node *p = rb_red_parent(n), *g, *u;
    while (true) {
        if (!p) {
            /*
             * n 为树根节点时，将 n 设为黑色，完成
             */
            rb_set_parent_color(n, NULL, RB_BLACK);
            break;
        }

        /*
         * n 为红色，parent 为黑色，完成
         */
        if (rb_is_black(p))
            break;
        
        g = rb_red_parent(p);
        if (p != g->right) {
            u = g->right;
            if ( u && rb_is_red(u)) {
                /* Case 1: parent 为pparent 的左节点
                 *         gB                 gR <- n
                 *        /  \               /  \
                 *      pR    uR   ->       pB  uB
                 *     /                   /
                 *   nR                   nR
                 */

                rb_set_color(p, RB_BLACK);
                rb_set_color(u, RB_BLACK);
                rb_set_color(g, RB_RED);

                n = g;
                p = rb_parent(n);
                continue;
            }
            
            if (n == p->right) {
                /* case 2:
                 *    gB                 gB                 
                 *   /  \               /  \         
                 *  pR   u     ->      nR   u
                 *    \               /     
                 *     nR            pR   
                 * u 为 NULL 或 黑   
                 */

                _rb_left_rotate(root, p);

                if (augment_rotate)
                    augment_rotate(p, n);

                p = n;
            }

            /*
             *  Case 3:
             *       gB                    pB           
             *      /  \                  /  \
             *     pR   u     ->         nR  gR
             *    /                            \
             *   nR                             u
             * u 为 NULL 或 黑
             */
            rb_set_color(p, RB_BLACK);
            rb_set_color(g, RB_RED);

            _rb_right_rotate(root, g);
            
            if (augment_rotate)
                augment_rotate(g, p);
            break;
        } else {
             u = g->left;
            if (u && rb_is_red(u)) { // Case 1
                /* Case 1:
                 *      gB                gR
                 *     /  \              /  \
                 *   uR    pR   ->     uB    pB
                 *           \                 \
                 *            nR               nR
                 * n 可能为 p.right
                 */

                rb_set_color(p, RB_BLACK);
                rb_set_color(u, RB_BLACK);
                rb_set_color(g, RB_RED);

                n = g;
                p = rb_parent(n);
                continue;
            }

            if (n == p->left) {
                /* Case 2:
                 *       gB                 gB
                 *      /  \               /  \
                 *     u    pR   ->       u    nR
                 *         /                     \
                 *       nR                       pR
                 * u 为 NULL 或 黑
                 */
                _rb_right_rotate(root, p);

                if (augment_rotate)
                    augment_rotate(p, n);

                p = n;
            }

            /* Case 3:
             *       gB                      pB
             *      /  \                    /  \
             *     u    pR      ->        gR    nR
             *            \              / 
             *             nR           u
             * u 为 NULL 或 黑
             */
            
            rb_set_color(g, RB_RED);
            rb_set_color(p, RB_BLACK);

            _rb_left_rotate(root, g);

            if (augment_rotate)
                augment_rotate(g, p);
            break;
        }
    }
}

// 不调整 扩展信息
void rb_insert_color(struct rb_root *root, struct rb_node *node)
{
    _rb_insert_color_augmented(root, node, NULL);
}

void _rb_insert_augmented(struct rb_root *root, struct rb_node *node, 
    void (*augment_rotate)(struct rb_node *old, struct rb_node *new))
{
    _rb_erase_color_augmented(root, node, augment_rotate);
}

void rb_insert_augmented(struct rb_root *root, struct rb_node *node, struct rb_augment_callbacks *augment)
{
    _rb_insert_augmented(root, node, augment->rotate);
}

// 删除节点，并返回需要调整颜色的黑色节点
static struct rb_node *_rb_erase_augmented(struct rb_root *root, struct rb_node *n, struct rb_augment_callbacks *augment)
{
    struct rb_node *nl = n->left;
    struct rb_node *nr = n->right;
    struct rb_node *rebalance; // 需要调整颜色的节点，它必须是一个黑色节点

    // 重点：如果 n 只有一个 子节点，则 子节点 一定是红色，n 一定是黑色
    if (!nl) { // 左空，用右代替 n
        _rb_replace_node(root, n, nr);
        if (nr) { 
            /*          nB
             *         /  \
             *       nil   nrR
             * 操作步骤：
             *  - 右替 n 且置黑
             *  - 从 n 的 parent 开始至树根 往上调整 扩展信息
             */
            _rb_replace_node(root, n, nr);
            rb_set_color(nr, RB_BLACK);

            rebalance = NULL; // 无须调整任何节点颜色
            
            if (augment && augment->propagate)
                augment->propagate(rb_parent(n), NULL);
        } else {
            /*       n
             *      / \
             *    nil  nil
             * 操作步骤：
             *  - 如 n 黑，则 需要调整 n 的 parent 颜色，否则无须调整颜色
             *  - 从 n 的 parent 开始至树根 往上调整 扩展信息
             */
            rebalance = rb_is_black(n) ? rb_parent(nr) : NULL;
            if (augment && augment->propagate)
                augment->propagate(rb_parent(n), NULL);
        }
    } else if (!nr) {
        /*         nB
         *        /  \
         *      nlR  nil
         * 操作步骤：
         *  - 左替 n 且置黑
         *  - 从 n 的 parent 开始至树根 往上调整 扩展信息
         */
        _rb_replace_node(root, n, nl);
        rb_set_color(nl, RB_BLACK);

        rebalance = NULL;

        if (augment && augment->propagate)
            augment->propagate(rb_parent(n), NULL);
    } else {
        /* 左右均非空 */
        struct rb_node *successor;
        if (!nr->left) {
            /*         n
             *        / \
             *       nl  nrB
             *            \
             *           nrrR or nil
             * 如 nr 无左，nr.right 为红，nr 为黑：
             *  - 用 nr 代替 n, 且颜色为 n 的颜色
             *  - 将 n 的 扩展信息 复制到 nr 上，并从 nr 往上调整扩展信息
             *  - 如果 nrr 为空且 nr 为黑，则从 nr 调整颜色
             *  - 如果nrr 不为空，则为红，将它置黑即可
             */
            _rb_replace_node(root, n, nr);            
            nr->left = nl;
            rb_set_parent(nr->left, nr);
            unsigned long nr_color = rb_color(nr);
            rb_set_color(nr, rb_color(n));

            if (augment && augment->copy)
                augment->copy(n, nr);
            if (augment && augment->propagate)
                augment->propagate(nr, NULL);

            if (nr->right) {
                rebalance = NULL;
                rb_set_color(nr->right, RB_BLACK);
            } else {
                rebalance = (nr_color & RB_BLACK) ? nr : NULL;
            }
        } else {
            /*           n
             *          / \
             *         nl  nr
             *            /  \
             *          nrl  nrr
             *            \
             *           nrlr : 红
             * 操作步骤：
             *  - nrl 代替 n ，及其颜色
             *  - nrl 扩展信息 为 n
             *  - nrlr 不为空，代替 nrl, 且置黑
             *  - nrlr 为空，如 nrl 为黑，则调整 nr 的颜色
             *  - 从 nr 至根 调整扩展信息 
             */
            struct rb_node *successor = rb_leftmost_of_node(nr);
            struct rb_node *sr = successor->right;
            struct rb_node *sp = rb_parent(successor);
            unsigned long s_color = rb_color(successor);
            unsigned long nr_color = rb_color(nr);

            _rb_replace_node(root, n, successor);
            successor->left = nl;
            rb_set_parent(nl, successor);
            successor->right = nr;
            rb_set_parent(nr, successor);
            rb_set_color(successor, rb_color(n));
            
            if (augment && augment->copy)
                augment->copy(n, successor);
            if (augment && augment->propagate)
                augment->propagate(sp, NULL);

            if (sr) {
                sp->left = sr;
                rb_set_parent_color(sr, sp, RB_BLACK);
                rebalance = NULL;
            } else {
                rebalance = (s_color & RB_BLACK) ? sp : NULL;
            }
        }
    }

    return rebalance;
}

/* 调整经过 n 的所有叶结点的黑节点数加1
 */
static void _rb_erase_color(struct rb_root *root, struct rb_node *n, 
    void (*augment_rotate(struct rb_node *old, struct rb_node *new)))
{
    while (true) {

    }
}

void rb_erase_augmented(struct rb_root *root, struct rb_node *n, struct rb_augment_callbacks *augment)
{
    struct rb_node *rebalance = _rb_erase_augmented(root, n, augment);
    if (rebalance)
        _rb_erase_color(root, rebalance,  augment ? augment->rotate : NULL);
}

