#ifndef BIN_TREE_NODE_H
#define BIN_TREE_NODE_H

#include "Common.h"

typedef char const * tree_elem_t;
#define TREE_ELEM_STR     "string"
#define TREE_ELEM_OUT_FRM "%s"

#define TREE_ELEM_COPY(dest, src)       \
do {                                    \
    CHECK_FUNC(My_strdup, &dest, src);  \
} while (false)

#define TREE_ELEM_DTOR(src) \
do {                        \
    free((char *)src);      \
    src = nullptr;          \
} while (false)

#define TREE_ELEM_EQUAL(a, b)   \
(!strcmp(a, b))

struct Bin_tree_node {
    Bin_tree_node *left,
                  *right;
    tree_elem_t   val;
    bool          need_copy;

    bool          is_valid;
    bool          verify_used; //I don't want to use hash-map in verify, because it is hard
};

errno_t Bin_tree_node_Ctor(Bin_tree_node *node_ptr,
                           Bin_tree_node *left, Bin_tree_node *right,
                           tree_elem_t val, bool need_copy);

errno_t get_new_Bin_tree_node(Bin_tree_node **dest,
                              Bin_tree_node *left, Bin_tree_node *right,
                              tree_elem_t val, bool need_copy);

errno_t Bin_tree_node_Dtor(Bin_tree_node *node_ptr);

#define TREE_NODE_INVALID     0B10'000'000'000
#define TREE_NODE_VERIFY_USED 0B100'000'000'000
errno_t Bin_tree_node_verify(Bin_tree_node const *node_ptr, errno_t *err_ptr);

errno_t Bin_subtree_Dtor(Bin_tree_node *node_ptr);

#define TREE_INVALID_STRUCTURE 0B1'000'000'000'000
errno_t Bin_subtree_verify(Bin_tree_node *node_ptr, errno_t *err_ptr);

errno_t Bin_subtree_dot_dump(FILE *out_stream, Bin_tree_node const *node_ptr);

errno_t split_node(Bin_tree_node *node_ptr,
                   tree_elem_t left_val, tree_elem_t right_val,
                   bool left_need_copy, bool right_need_copy);

#define INCORRECT_TREE_INPUT 1'000
errno_t str_read_subtree(Bin_tree_node **dest, char *buffer);
errno_t str_copy_read_subtree(Bin_tree_node **dest, char *buffer);
errno_t read_subtree(Bin_tree_node **dest, FILE *in_stream);

errno_t write_subtree(Bin_tree_node *src, FILE *out_stream);

#endif
