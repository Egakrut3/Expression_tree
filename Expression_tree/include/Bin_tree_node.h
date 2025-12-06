#ifndef BIN_TREE_NODE_H
#define BIN_TREE_NODE_H

#include "Common.h"

enum Expression_type {
    EXPRESSION_LITERAL_TYPE,
    EXPRESSION_OPERATION_TYPE,
    EXPRESSION_NAME_TYPE,
};

enum Expression_operation {
    #define HANDLE_OPERATION(name, ...) \
    name ## _OPERATION,
    //This includes generates enum-states for all
    //operations by applying previously declared
    //macros HANDLE_OPERATION to them
    #include "Text_operations/Unary_functions.h"
    #include "Text_operations/Binary_functions.h"
    #include "Text_operations/Binary_operators.h"
    #undef HANDLE_OPERATION
};

union Expression_val {
    double               val;
    Expression_operation operation;
    char                 *name;
};

struct Expression_token {
    Expression_type type;
    Expression_val  val;
};

struct Bin_tree_node {
    Bin_tree_node    *left,
                     *right;

    Expression_token data;

    bool             is_valid;
    bool             verify_used; //I don't want to use hash-map in verify, because it is hard
};

errno_t Bin_tree_node_Ctor(Bin_tree_node *node_ptr,
                           Bin_tree_node *left, Bin_tree_node *right,
                           Expression_token data);

errno_t new_Bin_tree_node(Bin_tree_node **dest,
                          Bin_tree_node *left, Bin_tree_node *right,
                          Expression_token data);

Bin_tree_node *DSL_new_Bin_tree_node(Bin_tree_node *left, Bin_tree_node *right,
                                     Expression_token data,
                                     errno_t *err_ptr); //TODO -

errno_t Bin_tree_node_Dtor(Bin_tree_node *node_ptr);

#define TREE_NODE_INVALID     0B10'000'000'000
#define TREE_NODE_VERIFY_USED 0B100'000'000'000
errno_t Bin_tree_node_verify(Bin_tree_node const *node_ptr, errno_t *err_ptr);

errno_t subtree_Dtor(Bin_tree_node *node_ptr);

#define TREE_INVALID_STRUCTURE 0B1'000'000'000'000
errno_t subtree_verify(Bin_tree_node *node_ptr, errno_t *err_ptr);

errno_t subtree_dot_dump(FILE *out_stream, Bin_tree_node const *node_ptr);

#define INCORRECT_TREE_INPUT 1'000
errno_t str_prefix_read_subtree(Bin_tree_node **dest, char const *buffer);
errno_t str_infix_read_subtree(Bin_tree_node **dest, char const *buffer);

errno_t prefix_write_subtree(FILE *out_stream, Bin_tree_node *src);
errno_t tex_write_subtree(FILE *out_stream, Bin_tree_node const *node_ptr);

Bin_tree_node *copy_subtree(Bin_tree_node const *src, errno_t *err_ptr);
Bin_tree_node *differentiate_subtree(Bin_tree_node const *src, errno_t *err_ptr);
errno_t simplify_subtree(Bin_tree_node **dest, Bin_tree_node const *src);

#endif
