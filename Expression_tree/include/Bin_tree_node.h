#ifndef BIN_TREE_NODE_H
#define BIN_TREE_NODE_H

#include "Common.h"
#include "math.h"

enum expression_type {
    EXPRESSION_LITERAL_TYPE,
    EXPRESSION_OPERATION_TYPE,
    EXPRESSION_VARIABLE_TYPE,
};

#define HANDLE_OPERATION(name, ...) \
name ## _OPERATION,
enum expression_operation {
    //This include generates enum-states for all
    //operations by applying previously declared
    //macros HANDLE_OPERATION to them
    #include "Operation_list.h"
};
#undef HANDLE_OPERATION

union expression_val {
    double               val;
    expression_operation operation;
    char                 *name;
};

#define TREE_ELEM_STR "expression_val"

struct Bin_tree_node {
    Bin_tree_node   *left,
                    *right;

    expression_type type;
    expression_val  val;

    bool            is_valid;
    bool            verify_used; //I don't want to use hash-map in verify, because it is hard
};

errno_t Bin_tree_node_Ctor(Bin_tree_node *node_ptr,
                           Bin_tree_node *left, Bin_tree_node *right,
                           expression_type type, expression_val val);

//TODO -
/*
errno_t get_new_Bin_tree_node(Bin_tree_node **dest,
                              Bin_tree_node *left, Bin_tree_node *right,
                              expression_type type, expression_val val);
*/

Bin_tree_node *get_new_Bin_tree_node(Bin_tree_node *left, Bin_tree_node *right,
                                     expression_type type, expression_val val,
                                     errno_t *err_ptr);

errno_t Bin_tree_node_Dtor(Bin_tree_node *node_ptr);

#define TREE_NODE_INVALID     0B10'000'000'000
#define TREE_NODE_VERIFY_USED 0B100'000'000'000
errno_t Bin_tree_node_verify(Bin_tree_node const *node_ptr, errno_t *err_ptr);

errno_t Bin_subtree_Dtor(Bin_tree_node *node_ptr);

#define TREE_INVALID_STRUCTURE 0B1'000'000'000'000
errno_t Bin_subtree_verify(Bin_tree_node *node_ptr, errno_t *err_ptr);

errno_t Bin_subtree_dot_dump(FILE *out_stream, Bin_tree_node const *node_ptr);
errno_t subtree_tex_dump(FILE *out_stream, Bin_tree_node const *cur_node);

#define INCORRECT_TREE_INPUT 1'000
errno_t str_prefix_read_subtree(Bin_tree_node **dest, char const *buffer);
errno_t str_infix_read_subtree(Bin_tree_node **dest, char const *buffer);

errno_t prefix_write_subtree(Bin_tree_node *src, FILE *out_stream);

Bin_tree_node *copy_subtree(Bin_tree_node const *src, errno_t *err_ptr);
Bin_tree_node *differentiate_subtree(Bin_tree_node const *src, errno_t *err_ptr);
errno_t simplify_subtree(Bin_tree_node **dest, Bin_tree_node const *src);

#endif
