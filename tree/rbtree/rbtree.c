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
    return n;
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

    if (new)
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
    struct rb_node *rebalance = NULL; // 需要调整颜色的节点，它必须是一个黑色节点

    /* 重点：
     *  如果 n 只有一个 子节点，则 子节点 一定是红色，n 一定是黑色。
     *  因为在红黑树中, 空节点 NIL 被认为是黑色的，如果 n 只有一个子节点,
     *  且该子节点是黑色的，那么从 n 到该子节点的入下的 NIL 节点一定比 n 往另一侧往下的 NIL 节点的黑高多1.
     *  所以可以得出结论：如果 n只有一个子节点，则该子节点一定是红色，n 一定是黑色，并且该子节点一定没有子节点
     */
    if (!nl) {
        /* 情况一：被删节点最多只有一个子节点，这里假设左子节点为空，则情形如下   
         */
        _rb_replace_node(root, n, nr);
        if (nr) {
            /*       n(B)               nr(B)
             *       /  \               /  \
             *      N  nr(R) ->        N    N
             *            / \
             *           N   N
             * 注意：此情形，将 nr 置黑后，不需要调整节点颜色
             */
            rb_set_color(nr, RB_BLACK);
        } else {
            /* 分两种情形：n 为红色和黑色
             *    情形一: n为红色
             *         p(B)                        p(B)
             *        /   \                        / \
             *      n(R)   s(R or N)     ->       N  s(R or N)
             *      / \
             *     N   N
             * 此情形只需要删除 n 节点即可，不需要调整颜色
             *    情形二：n 为黑色
             *            p
             *      /         \
             *    n(B)        s(B)
             *   /  \      /       \ 
             *  N   N  sl(R or N)  sr: (R or N)
             *         / \         / \
             *        N   N       N   N
             * 此情形需要调整 n 的父节点的颜色, 即待调整颜色的节点情形如下：
             *        p
             *       / \
             *      N  s(B)
             *         /  \
             *       sl    sr    <- sl, sr 均(R or N)
             *      / \   / \
             *     N  N  N  N
             */
            rebalance = rb_is_black(n) ? rb_parent(n) : NULL;
        }

        if (augment && augment->propagate)
            augment->propagate(rb_parent(n), NULL);
        
    } else if (!nr) {
        /* 仍然属于情形一的一种情况：左非空，右空
         *         n(B)          nl(B)
         *        /  \    ->    /  \
         *      nl(R) N        N    N
         *     / \
         *    N   N
         */
        _rb_replace_node(root, n, nl);
        rb_set_color(nl, RB_BLACK);

        if (augment && augment->propagate)
            augment->propagate(rb_parent(n), NULL);
    } else {
        /* 左右均非空 */
        struct rb_node *successor;
        if (!nr->left) {
            /*         n
             *        / \
             *       nl  nr
             *            \
             *           nrr(R or N)
             */
            unsigned long pc = n->parent_color;
            unsigned long nr_color = rb_color(nr);
            nr->parent_color = pc; // 如 nr_color 为黑色，则 nr 的右子树的黑高减1，需要调整 nr 的的颜色
            nr->left = nl;
            rb_set_parent(nl, nr);

            if (nr->right) {
                /*        n                          nr(color of n)
                 *       / \                        /  \
                 *     nl  nr(B)        ->        nl    nrr(B)
                 *         /  \                 
                 *        N   nrr(R)                  
                 * 无须调整颜色
                 */
                rb_set_color(nr->right, RB_BLACK);
            } else {
                /*         n                     nr(color of n)
                 *        / \                   /  \
                 *      nl   nr        ->     nl    N
                 *           / \
                 *          N   N
                 * 当nr 为黑时，nr 的右侧子树的黑高减1，需要调整 nr 的颜色
                 */
                rebalance = (nr_color & RB_BLACK) ? nr : NULL;
            }

            if (augment && augment->copy)
                augment->copy(n, nr);

            if (augment && augment->propagate)
                augment->propagate(nr, NULL);
            
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

            _rb_replace_node(root, n, successor);
            successor->left = nl;
            rb_set_parent(nl, successor);
            successor->right = nr;
            rb_set_parent(nr, successor);
            rb_set_color(successor, rb_color(n));

            sp->left = sr;
            if (sr) {
                rb_set_parent_color(sr, sp, RB_BLACK);
                rebalance = NULL;
            } else {
                rebalance = (s_color & RB_BLACK) ? sp : NULL;
            }
            
            if (augment && augment->copy)
                augment->copy(n, successor);
            if (augment && augment->propagate)
                augment->propagate(sp, NULL);
        }
    }

    return rebalance;
}

/* 因为n有一个为黑色的叶子节点被移除了，所以要调整该n */
static void _rb_erase_color(struct rb_root *root, struct rb_node *parent, 
    void (*augment_rotate(struct rb_node *old, struct rb_node *new)))
{
    struct rb_node *node = NULL, *sibling;
    while (true) {
        if (node != parent->right) { // node = parent.left, 可能为NIL或黑色
            sibling = parent->right; // parent.right一定不为空
            if (rb_is_red(sibling)) { // parent, sibling子节点均为黑
                /* Case 1:
                 *       p                   s(p)
                 *      / \                 /   \
                 *     N  s(R)      ->    p(R)  sr(B)
                 *        /  \            / \
                 *    sl(B)  sr(B)       N  sl(B)
                 *  sl, sr一定不为空, 且为黑
                 */
                _rb_left_rotate(root, parent);
                rb_set_color(sibling, rb_color(parent));
                rb_set_color(parent, RB_RED);

                if (augment_rotate)
                    augment_rotate(parent, sibling);
                sibling = parent->right;
            }
            // sibling 为黑
            struct rb_node *sr = sibling->right;
            if (!sr || rb_is_black(sr)) {
                struct rb_node *sl = sibling->left;
                if (!sl || rb_is_black(sl)) {
                    /* Case 2:
                     *        p                    p      <- node
                     *       / \                  / \
                     *      N  s(B)      ->      N  s(R)
                     *         /  \                 /  \
                     *        sl  sr               sl  sr
                     * 注意：
                     *  - sl, sr要么都为空，要么都为黑。如果sl为空，sr不为空，则sr一定为红
                     *  - 如果 s 为黑，则 N 一定不为空
                     */
                    rb_set_color(sibling, RB_RED);
                    if (rb_is_red(parent)) {
                        rb_set_color(parent, RB_BLACK);
                    } else {
                        node = parent;
                        parent = rb_parent(node);
                        if (parent)
                            continue;
                    }
                    break;
                }

                /* Case 3:
                 *          p                  p
                 *        /  \                / \
                 *       N   s(B)    ->      N  sl(R)
                 *          /  \                /  \
                 *       sl(R)  sr             N   s(B)
                 *        /  \                     / \
                 *       N    N                   N   sr
                 * Note:       
                 *  - sr要么为空，要么为黑, sl存在且一定为红
                 *  
                 */
                struct rb_node *slr = sl->right;
                 _rb_right_rotate(root, sibling);

                if (augment_rotate)
                    augment_rotate(sibling, sl);
                sr = sibling;
                sibling =  sl;
            }

            /* Case 4:
             *          p                         s(p)
             *        /   \                      /   \
             *       N    s(R)        ->       p(B)   sr(B)
             *           /    \                /  \
             *       sl(R)    sr(B)           N   sl(R)
             *
             */
            _rb_left_rotate(root, parent);
            rb_set_color(sibling, rb_color(parent));
            rb_set_color(parent, RB_BLACK);

            if (augment_rotate)
                augment_rotate(parent, sibling);

            break;
        } else {
            sibling = parent->left;
            if (rb_is_red(sibling)) {
                /* Case 1:
                 *           p(B)                   s(p)
                 *           / \                    /  \
                 *        s(R)  N      ->        sl(B) p(R)
                 *        /  \                        /   \
                 *     sl(B) sr(B)             s->  sr(B)  N
                 */
                struct rb_node *sr = sibling->right;
                _rb_right_rotate(root, parent);
                rb_set_color(sibling, rb_color(parent));
                rb_set_color(parent, RB_RED);
                
                if (augment_rotate)
                    augment_rotate(parent, sibling);

                sibling = parent->left;
            }
            
            struct rb_node *sl = sibling->left;
            if (!sl || rb_is_black(sl)) {
                struct rb_node *sr = sibling->right;
                if (!sr || rb_is_black(sr)) {
                    /* Case 2:
                    *            p                    p   <- node
                    *           / \                  / \
                    *         s(B) N     ->       s(R)  N
                    *         / \                 / \
                    *       sl  sr               sl  sr
                    * Note:
                    *   sl, sr 要么都空，要么都为黑
                    */
                    rb_set_color(sibling, RB_RED);
                    if (rb_is_red(parent)) {
                        rb_set_color(parent, RB_BLACK);
                    } else {
                        node = parent;
                        parent = rb_parent(node);
                        if (parent)
                            continue;
                    }
                    break;
                }

                /* Case 3:
                 *            p                        p
                 *          /   \                    /   \
                 *        s(B)   N     ->          sr(R)  N
                 *        /  \                     / \
                 *       sl  sr(R)               s(B) srr(B)
                 *                               / \
                 *                              sl  srl
                 * Note:
                 *  sl为空或黑
                 */

                _rb_left_rotate(root, sibling);
                if (augment_rotate)
                    augment_rotate(sibling, sr);
                
                sl = sibling;
                sibling = sr;
            }

            /* Case 4:
             *         p                        s(p)
             *       /   \                     /   \
             *     s(B)   N   ->            sl(R)  p(B)
             *     / \                            /  \
             *  sl(R) sr                        sr    N
             * Note:
             *  sl 非空且红
             */
            _rb_right_rotate(root, parent);
            rb_set_color(sibling, rb_color(parent));
            rb_set_color(parent, RB_BLACK);


        }
    }
}

void rb_erase_augmented(struct rb_root *root, struct rb_node *n, struct rb_augment_callbacks *augment)
{
    struct rb_node *rebalance = _rb_erase_augmented(root, n, augment);
    if (rebalance)
        _rb_erase_color(root, rebalance,  augment ? augment->rotate : NULL);
}

