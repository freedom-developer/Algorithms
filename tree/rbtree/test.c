#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>

struct test_node {
    int key;
    struct rb_node rb;
    int val, augmented;
};

static struct rb_root_cached root = RB_ROOT_CACHED;

static void insert(struct rb_root_cached *root, struct test_node *node)
{
    struct rb_node **link = &root->root.rb_node, *parent = NULL;
    while (*link) {
        parent = *link;
        if (node->key < (rb_entry(parent, struct test_node, rb))->key) 
            link = &parent->left;
        else 
            link = &parent->right;
    }

    rb_link_node(&node->rb, parent, link);
    rb_insert_color(&root->root, &node->rb);
}






struct test_node *new_node(int key)
{
    struct test_node *node = (struct test_node *)malloc(sizeof(*node));
    if (!node)
        return NULL;
    node->key = key;

    return node;
}

// 前序遍历
void qianxu(struct rb_node *node)
{
    if (!node) return;
    if (node->left) qianxu(node->left);
    struct test_node *tn = rb_entry(node, struct test_node, rb);
    printf("%d ", tn->key);
    if (node->right) qianxu(node->right);
}

static void print_tree_node(struct rb_node *node, int depth, const char *edge)
{
    int i;
    struct test_node *tn;

    if (!node)
        return;

    for (i = 0; i < depth; i++)
        printf("    ");

    tn = rb_entry(node, struct test_node, rb);
    printf("%s-- %d(%c)\n", edge, tn->key, rb_is_red(node) ? 'R' : 'B');

    print_tree_node(node->left, depth + 1, "L");
    print_tree_node(node->right, depth + 1, "R");
}

void print_tree(struct rb_root *root)
{
    if (!root || !root->rb_node) {
        printf("(empty)\n");
        return;
    }

    print_tree_node(root->rb_node, 0, "root");
}

int main(int argc, char **argv)
{
    int i;

    srand(time(NULL));

    for (i = 0; i < 10; i++) {
        struct test_node *tmp = new_node(rand() % 100);
        insert(&root, tmp);
    }

    qianxu(root.root.rb_node);
    printf("\n\n");
    print_tree(&root.root);

    struct rb_node *erase_node = root.root.rb_node;
    rb_erase(&root.root, erase_node);
    free(rb_entry(erase_node, struct test_node, rb));
    
    qianxu(root.root.rb_node);
    printf("\n\n");
    print_tree(&root.root);

    return 0;
}
