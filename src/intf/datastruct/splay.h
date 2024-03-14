#pragma once
#include <stdint.h>

struct splay_tree_node
{
    struct splay_tree_node* left;
    struct splay_tree_node* right;
};

typedef int (*splay_tree_compare)(uintptr_t, uintptr_t);
typedef uintptr_t (*splay_tree_get_key)(struct splay_tree_node*);

struct splay_tree
{
    struct splay_tree_node* root;

    splay_tree_get_key get_key;
    splay_tree_compare compare;
};

static inline void splay_rotl(
    struct splay_tree_node** grandparent,
    struct splay_tree_node* parent,
    struct splay_tree_node* node)
{
    struct splay_tree_node* temp;
    temp = node->right;
    node->right = parent;
    parent->left = temp;
    *grandparent = node;
}

static inline void splay_rotr(
    struct splay_tree_node** grandparent,
    struct splay_tree_node* parent,
    struct splay_tree_node* node)
{
    struct splay_tree_node* temp;
    temp = node->right;
    node->left = parent;
    parent->right = temp;
    *grandparent = node;
}

static inline void splay_tree_splay(struct splay_tree* stree, uintptr_t ref)
{
    if(!stree->root)
        return;

    do
    {
        int comp1, comp2;
        struct splay_tree_node *n, *c;

        n = stree->root;
        comp1 = stree->compare(ref, stree->get_key(n));

        if(comp1 == 0)
            return;

        if(comp1 < 0)
            c = n->left;
        else    
            c = n->right;

        if(!c)
            return;

        comp2 = stree->compare(ref, stree->get_key(c));

        if(!comp2 || (comp2 < 0 && !c->left) || (comp2 > 0 && !c->right))
        {
            if(comp1 < 0)
                splay_rotl(&stree->root, n, c);
            else
                splay_rotr(&stree->root, n, c);
            return;
        }

        if(comp1 < 0 && comp2 < 0)
        {
            splay_rotl(&n->left, c, c->left);
            splay_rotl(&stree->root, n, n->left);
        }
        else if(comp1 > 0 && comp2 > 0)
        {
            splay_rotr(&n->right, c, c->right);
            splay_rotr(&stree->root, n, n->right);
        }
        else if(comp1 < 0 && comp2 > 0)
        {
            splay_rotr(&n->left, c, c->right);
            splay_rotl(&stree->root, n, n->left);
        }
        else if(comp1 > 0 && comp2 < 0)
        {
            splay_rotl(&n->right, c, c->right);
            splay_rotr(&stree->root, n, n->right);
        }
    } 
    while (1);

}

static inline void splay_tree_insert(struct splay_tree* stree, struct splay_tree_node* item)
{
    int comp = 0;
    splay_tree_splay(stree, stree->get_key(item));

    if(stree->root)
        comp = stree->compare(stree->get_key(stree->root), stree->get_key(item));

    if(stree->root && comp == 0)
        __builtin_unreachable();
    else
    {
        if(!stree->root)
            item->left = item->right = 0;
        else if(comp < 0)
        {
            item->left = stree->root;
            item->right = item->left->right;
            item->left->right = 0;
        }
        else
        {
            item->right = stree->root;
            item->left = item->right->left;
            item->right->left = 0;
        }

        stree->root = item;
    }
}


static inline void splay_tree_remove(struct splay_tree* stree, struct splay_tree_node* item)
{
    splay_tree_splay(stree, stree->get_key(item));

    if(stree->root && stree->compare(stree->get_key(stree->root), stree->get_key(item)) == 0)
    {
        struct splay_tree_node *left, *right;
        left = stree->root->left;
        right = stree->root->right;

        if(left)
        {
            stree->root = left;

            if(right)
            {
                while(left->right)
                    left = left->right;
                left->right = right;
            }
        }
        else
        {
            stree->root = right;
        }
    }
}

static inline struct splay_tree_node* splay_tree_find(struct splay_tree* stree, uintptr_t key)
{
    splay_tree_splay(stree, key);

    if(stree->root && stree->compare(stree->get_key(stree->root), key) == 0)
        return stree->root;
    else
        return 0;
}
