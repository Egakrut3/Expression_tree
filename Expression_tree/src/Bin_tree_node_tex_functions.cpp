#include "Bin_tree_node.h"

#define FINAL_CODE

static bool need_left_parenthesize(Bin_tree_node const *const node_ptr) {
    assert(node_ptr); assert(node_ptr->data.type == EXPRESSION_OPERATION_TYPE); assert(node_ptr->left);

    size_t parent_order = 0,
           child_order  = 0;

    switch (node_ptr->data.val.operation) {
        #define HANDLE_OPERATION(name, ...) \
            case name ## _OPERATION:        \
                child_order = 0;            \
                break;
        //This include generates cases for all
        //binary and unary functions by applying
        //previously declared macros HANDLE_OPERATION
        //to them
        #include "Tex_operations/Unary_functions.h"
        #include "Tex_operations/Binary_functions.h"
        #undef HANDLE_OPERATION

        #define HANDLE_OPERATION(name, tex_decl, left_order, ...)   \
        case name ## _OPERATION:                                    \
            parent_order = left_order;                              \
            break;
        //This include generates cases for all
        //binary operators by applying previously declared
        //macros HANDLE_OPERATION to them
        #include "Tex_operations/Binary_operators.h"
        #undef HANDLE_OPERATION

        default:
            PRINT_LINE();
            abort();
    }

    if (node_ptr->left->data.type == EXPRESSION_OPERATION_TYPE) {
        switch (node_ptr->left->data.val.operation) {
            #define HANDLE_OPERATION(name, ...) \
            case name ## _OPERATION:            \
                child_order = 0;                \
                break;
            //This include generates cases for all
            //binary and unary functions by applying
            //previously declared macros HANDLE_OPERATION
            //to them
            #include "Tex_operations/Unary_functions.h"
            #include "Tex_operations/Binary_functions.h"
            #undef HANDLE_OPERATION

            #define HANDLE_OPERATION(name, tex_decl, left_order, ...)   \
            case name ## _OPERATION:                                    \
                child_order = left_order;                               \
                break;
            //This include generates cases for all
            //binary operators by applying previously declared
            //macros HANDLE_OPERATION to them
            #include "Tex_operations/Binary_operators.h"
            #undef HANDLE_OPERATION

            default:
                PRINT_LINE();
                abort();
        }
    }
    else { child_order = 0; }

    return parent_order < child_order;
}

static bool need_right_parenthesize(Bin_tree_node const *const node_ptr) {
    assert(node_ptr); assert(node_ptr->data.type == EXPRESSION_OPERATION_TYPE); assert(node_ptr->right);

    size_t parent_order = 0,
           child_order  = 0;

    switch (node_ptr->data.val.operation) {
        #define HANDLE_OPERATION(name, ...) \
        case name ## _OPERATION:            \
            child_order = 0;                \
            break;
        //This include generates cases for all
        //binary and unary functions by applying
        //previously declared macros HANDLE_OPERATION
        //to them
        #include "Tex_operations/Unary_functions.h"
        #include "Tex_operations/Binary_functions.h"
        #undef HANDLE_OPERATION

        #define HANDLE_OPERATION(name, tex_decl, left_order, right_order, ...)  \
        case name ## _OPERATION:                                                \
            parent_order = right_order;                                         \
            break;
        //This include generates cases for all
        //binary operators by applying previously declared
        //macros HANDLE_OPERATION to them
        #include "Tex_operations/Binary_operators.h"
        #undef HANDLE_OPERATION

        default:
            PRINT_LINE();
            abort();
    }

    if (node_ptr->right->data.type == EXPRESSION_OPERATION_TYPE) {
        switch (node_ptr->right->data.val.operation) {
            #define HANDLE_OPERATION(name, ...) \
            case name ## _OPERATION:            \
                child_order = 0;                \
                break;
            //This include generates cases for all
            //binary and unary functions by applying
            //previously declared macros HANDLE_OPERATION
            //to them
            #include "Tex_operations/Unary_functions.h"
            #include "Tex_operations/Binary_functions.h"
            #undef HANDLE_OPERATION

            #define HANDLE_OPERATION(name, tex_decl, right_order, ...)  \
            case name ## _OPERATION:                                    \
                child_order = right_order;                              \
                break;
            //This include generates cases for all
            //binary operators by applying previously declared
            //macros HANDLE_OPERATION to them
            #include "Tex_operations/Binary_operators.h"
            #undef HANDLE_OPERATION

            default:
                PRINT_LINE();
                abort();
        }
    }
    else { child_order = 0; }

    return parent_order < child_order;
}

static errno_t following_tex_write_subtree(FILE *const out_stream, Bin_tree_node const *const cur_node) {
    assert(out_stream);

    if (!cur_node) { return 0; }

    switch (cur_node->data.type) {
        case EXPRESSION_LITERAL_TYPE:
            assert(!cur_node->left); assert(!cur_node->right);
            fprintf_s(out_stream, "%lG", cur_node->data.val.val);
            break;

        case EXPRESSION_OPERATION_TYPE:
            switch (cur_node->data.val.operation) {
                #define HANDLE_OPERATION(name, tex_decl, ...)                               \
                case name ##_OPERATION:                                                     \
                    fprintf_s(out_stream, tex_decl "(");                                    \
                    CHECK_FUNC(following_tex_write_subtree, out_stream, cur_node->right);   \
                    fprintf_s(out_stream, ")");                                             \
                    break;
                //This include generates cases for all
                //unary functions by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Tex_operations/Unary_functions.h"
                #undef HANDLE_OPERATION

                #define HANDLE_OPERATION(name, tex_decl, ...)                               \
                case name ## _OPERATION:                                                    \
                    fprintf_s(out_stream, tex_decl);                                        \
                    fprintf_s(out_stream, "{");                                             \
                    CHECK_FUNC(following_tex_write_subtree, out_stream, cur_node->left);    \
                    fprintf_s(out_stream, "}");                                             \
                    fprintf_s(out_stream, "{");                                             \
                    CHECK_FUNC(following_tex_write_subtree, out_stream, cur_node->right);   \
                    fprintf_s(out_stream, "}");                                             \
                    break;
                //This include generates cases for all
                //binary functions by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Tex_operations/Binary_functions.h"
                #undef HANDLE_OPERATION

                #define HANDLE_OPERATION(name, tex_decl, ...)                                   \
                case name ## _OPERATION:                                                        \
                    fprintf_s(out_stream, "{");                                                 \
                    if (need_left_parenthesize(cur_node)) {                                     \
                        fprintf_s(out_stream, "(");                                             \
                        CHECK_FUNC(following_tex_write_subtree, out_stream, cur_node->left);    \
                        fprintf_s(out_stream, ")");                                             \
                    }                                                                           \
                    else {                                                                      \
                        CHECK_FUNC(following_tex_write_subtree, out_stream, cur_node->left);    \
                    }                                                                           \
                    fprintf_s(out_stream, "}");                                                 \
                    fprintf_s(out_stream, " " tex_decl " ");                                    \
                    fprintf_s(out_stream, "{");                                                 \
                    if (need_right_parenthesize(cur_node)) {                                    \
                        fprintf_s(out_stream, "(");                                             \
                        CHECK_FUNC(following_tex_write_subtree, out_stream, cur_node->right);   \
                        fprintf_s(out_stream, ")");                                             \
                    }                                                                           \
                    else {                                                                      \
                        CHECK_FUNC(following_tex_write_subtree, out_stream, cur_node->right);   \
                    }                                                                           \
                    fprintf_s(out_stream, "}");                                                 \
                    break;
                //This include generates cases for all
                //binary operators by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Tex_operations/Binary_operators.h"
                #undef HANDLE_OPERATION

                default:
                    PRINT_LINE();
                    abort();
            }
            break;

        case EXPRESSION_NAME_TYPE:
            assert(!cur_node->left); assert(!cur_node->right);
            fprintf_s(out_stream, "%s", cur_node->data.val.name);
            break;

        default:
            PRINT_LINE();
            abort();
    }

    return 0;
}

errno_t tex_write_subtree(FILE *const out_stream, Bin_tree_node const *const node_ptr) { //TODO - 
    assert(out_stream);

    fprintf_s(out_stream, "\\documentclass{article}\n\\begin{document}\n$");
    CHECK_FUNC(following_tex_write_subtree, out_stream, node_ptr);
    fprintf_s(out_stream, "$\n\\end{document}\n");

    return 0;
}

#undef FINAL_CODE
