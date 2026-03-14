#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lftest.h"
#include "lfbinarytree.h"

static int destroy_count;

static void counting_destroy(void *data) {
    (void)data;
    destroy_count++;
}

/* Traversal helper: append int values to a static buffer */
static int traversal_buf[32];
static int traversal_idx;

static void collect_int(void *data) {
    traversal_buf[traversal_idx++] = *(int *)data;
}

int main() {
    /* Basic insertion and data access */
    BinTree *tree = malloc(sizeof(BinTree));
    bintree_init(tree, NULL);

    int root = 0;
    int l1 = 1;
    int l2 = 2;
    int r1 = 12;
    int r2 = 200;

    ASSERT_EQ(bintree_ins_left(tree, NULL, &root), 0);
    ASSERT_NOT_NULL(tree->root);
    ASSERT_EQ(tree->size, 1);

    ASSERT_EQ(bintree_ins_left(tree, tree->root, &l1), 0);
    ASSERT_EQ(bintree_ins_left(tree, tree->root->left, &l2), 0);
    ASSERT_EQ(bintree_ins_right(tree, tree->root->left, &r2), 0);
    ASSERT_EQ(bintree_ins_right(tree, tree->root, &r1), 0);
    ASSERT_EQ(bintree_ins_right(tree, tree->root->right, &r2), 0);
    ASSERT_EQ(bintree_ins_left(tree, tree->root->right, &l1), 0);
    ASSERT_EQ(tree->size, 7);

    ASSERT_EQ(*(int *)tree->root->data, 0);
    ASSERT_EQ(*(int *)tree->root->left->data, 1);
    ASSERT_EQ(*(int *)tree->root->right->data, 12);

    /* Error: insert root when tree already has one */
    ASSERT_EQ(bintree_ins_left(tree, NULL, &root), -1);
    ASSERT_EQ(bintree_ins_right(tree, NULL, &root), -1);

    /* Error: insert left/right where child already exists */
    ASSERT_EQ(bintree_ins_left(tree, tree->root, &l1), -1);
    ASSERT_EQ(bintree_ins_right(tree, tree->root, &r1), -1);

    /* Macros */
    ASSERT_FALSE(bintree_is_eob(tree->root));
    ASSERT_TRUE(bintree_is_eob(NULL));
    ASSERT_FALSE(bintree_is_leaf(tree->root));
    ASSERT_TRUE(bintree_is_leaf(tree->root->left->left));

    bintree_destroy(tree);
    ASSERT_EQ(tree->size, 0);
    ASSERT_NULL(tree->root);
    free(tree);

    /* Destroy callback */
    destroy_count = 0;
    BinTree dt;
    bintree_init(&dt, counting_destroy);

    int a = 10, b = 20, c = 30;
    bintree_ins_left(&dt, NULL, &a);
    bintree_ins_left(&dt, dt.root, &b);
    bintree_ins_right(&dt, dt.root, &c);
    ASSERT_EQ(dt.size, 3);

    bintree_destroy(&dt);
    ASSERT_EQ(destroy_count, 3);

    /* Partial removal with destroy callback */
    destroy_count = 0;
    BinTree dt2;
    bintree_init(&dt2, counting_destroy);
    bintree_ins_left(&dt2, NULL, &a);
    bintree_ins_left(&dt2, dt2.root, &b);
    bintree_ins_right(&dt2, dt2.root, &c);
    bintree_rem_left(&dt2, dt2.root);
    ASSERT_EQ(destroy_count, 1);
    ASSERT_EQ(dt2.size, 2);
    ASSERT_NULL(dt2.root->left);
    bintree_destroy(&dt2);
    ASSERT_EQ(destroy_count, 3);

    /* Traversal: build tree
     *       1
     *      / \
     *     2   3
     *    / \
     *   4   5
     */
    BinTree tt;
    bintree_init(&tt, NULL);
    int v1 = 1, v2 = 2, v3 = 3, v4 = 4, v5 = 5;
    bintree_ins_left(&tt, NULL, &v1);
    bintree_ins_left(&tt, tt.root, &v2);
    bintree_ins_right(&tt, tt.root, &v3);
    bintree_ins_left(&tt, tt.root->left, &v4);
    bintree_ins_right(&tt, tt.root->left, &v5);

    /* Preorder: 1 2 4 5 3 */
    traversal_idx = 0;
    bintree_traverse(&tt, tt.root, BINTREE_PREORDER, collect_int);
    ASSERT_EQ(traversal_idx, 5);
    ASSERT_EQ(traversal_buf[0], 1);
    ASSERT_EQ(traversal_buf[1], 2);
    ASSERT_EQ(traversal_buf[2], 4);
    ASSERT_EQ(traversal_buf[3], 5);
    ASSERT_EQ(traversal_buf[4], 3);

    /* Inorder: 4 2 5 1 3 */
    traversal_idx = 0;
    bintree_traverse(&tt, tt.root, BINTREE_INORDER, collect_int);
    ASSERT_EQ(traversal_idx, 5);
    ASSERT_EQ(traversal_buf[0], 4);
    ASSERT_EQ(traversal_buf[1], 2);
    ASSERT_EQ(traversal_buf[2], 5);
    ASSERT_EQ(traversal_buf[3], 1);
    ASSERT_EQ(traversal_buf[4], 3);

    /* Postorder: 4 5 2 3 1 */
    traversal_idx = 0;
    bintree_traverse(&tt, tt.root, BINTREE_POSTORDER, collect_int);
    ASSERT_EQ(traversal_idx, 5);
    ASSERT_EQ(traversal_buf[0], 4);
    ASSERT_EQ(traversal_buf[1], 5);
    ASSERT_EQ(traversal_buf[2], 2);
    ASSERT_EQ(traversal_buf[3], 3);
    ASSERT_EQ(traversal_buf[4], 1);

    /* Traversal on NULL node is a no-op */
    traversal_idx = 0;
    bintree_traverse(&tt, NULL, BINTREE_INORDER, collect_int);
    ASSERT_EQ(traversal_idx, 0);

    bintree_destroy(&tt);

    /* Merge */
    BinTree ml, mr, mm;
    int mroot = 100, mlv = 10, mrv = 20;
    bintree_init(&ml, NULL);
    bintree_init(&mr, NULL);
    bintree_ins_left(&ml, NULL, &mlv);
    bintree_ins_left(&mr, NULL, &mrv);

    ASSERT_EQ(bintree_merge(&mm, &ml, &mr, &mroot), 0);
    ASSERT_EQ(mm.size, 3);
    ASSERT_EQ(*(int *)mm.root->data, 100);
    ASSERT_EQ(*(int *)mm.root->left->data, 10);
    ASSERT_EQ(*(int *)mm.root->right->data, 20);
    ASSERT_EQ(ml.size, 0);
    ASSERT_NULL(ml.root);
    ASSERT_EQ(mr.size, 0);
    ASSERT_NULL(mr.root);
    bintree_destroy(&mm);

    /* Merge: left == right should fail */
    BinTree self;
    bintree_init(&self, NULL);
    bintree_ins_left(&self, NULL, &a);
    BinTree merged;
    ASSERT_EQ(bintree_merge(&merged, &self, &self, &b), -1);
    bintree_destroy(&self);

    /* Size tracking through insertions and removals */
    BinTree st;
    bintree_init(&st, NULL);
    ASSERT_EQ(st.size, 0);
    bintree_ins_left(&st, NULL, &a);
    ASSERT_EQ(st.size, 1);
    bintree_ins_left(&st, st.root, &b);
    ASSERT_EQ(st.size, 2);
    bintree_ins_right(&st, st.root, &c);
    ASSERT_EQ(st.size, 3);
    bintree_rem_right(&st, st.root);
    ASSERT_EQ(st.size, 2);
    bintree_destroy(&st);

    TEST_REPORT();
}
