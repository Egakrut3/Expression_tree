#include "Bin_tree_node.h"
#include <string.h>

#define FINAL_CODE

static size_t get_left_order(Expression_tree_data const cur_data) {
    if (cur_data.type != EXPRESSION_TREE_OPERATION_TYPE) { return 0; }

    switch (cur_data.val.operation) {
        #define HANDLE_OPERATION(name, ...) \
            case name ## _OPERATION:        \
                return 0;
        //This include generates cases for all
        //binary and unary functions by applying
        //previously declared macros HANDLE_OPERATION
        //to them
        #include "Tex_operations/Unary_functions.h"
        #include "Tex_operations/Binary_functions.h"
        #undef HANDLE_OPERATION

        #define HANDLE_OPERATION(name, tex_decl, left_order, ...)   \
        case name ## _OPERATION:                                    \
            return left_order;
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

static size_t get_right_order(Expression_tree_data const cur_data) {
    if (cur_data.type != EXPRESSION_TREE_OPERATION_TYPE) { return 0; }
    switch (cur_data.val.operation) {
        #define HANDLE_OPERATION(name, ...) \
            case name ## _OPERATION:        \
                return 0;
        //This include generates cases for all
        //binary and unary functions by applying
        //previously declared macros HANDLE_OPERATION
        //to them
        #include "Tex_operations/Unary_functions.h"
        #include "Tex_operations/Binary_functions.h"
        #undef HANDLE_OPERATION

        #define HANDLE_OPERATION(name, tex_decl, left_order, right_order, ...)  \
        case name ## _OPERATION:                                                \
            return right_order;
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

static bool need_left_parenthesize(Expression_tree_data const parent_data,
                                   Expression_tree_data const child_data) {
    return get_left_order(parent_data) < get_left_order(child_data);
}

static bool need_right_parenthesize(Expression_tree_data const parent_data,
                                   Expression_tree_data const child_data) {
    return get_right_order(parent_data) < get_right_order(child_data);
}

errno_t tex_write_subtree(FILE *const out_stream, Bin_tree_node const *const cur_node) {
    assert(out_stream);

    if (!cur_node) { return 0; }

    switch (cur_node->data.type) {
        case EXPRESSION_TREE_LITERAL_TYPE:
            assert(!cur_node->left); assert(!cur_node->right);
            fprintf_s(out_stream, "%lG", cur_node->data.val.val);
            break;

        case EXPRESSION_TREE_OPERATION_TYPE:
            switch (cur_node->data.val.operation) {
                #define HANDLE_OPERATION(name, tex_decl, ...)                   \
                case name ##_OPERATION:                                         \
                    fprintf_s(out_stream, tex_decl);                            \
                    fprintf_s(out_stream, "\\left(");                           \
                    CHECK_FUNC(tex_write_subtree, out_stream, cur_node->right); \
                    fprintf_s(out_stream, "\\right)");                          \
                    break;
                //This include generates cases for all
                //unary functions by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Tex_operations/Unary_functions.h"
                #undef HANDLE_OPERATION

                #define HANDLE_OPERATION(name, tex_decl, ...)                   \
                case name ## _OPERATION:                                        \
                    fprintf_s(out_stream, tex_decl);                            \
                    fprintf_s(out_stream, "{");                                 \
                    CHECK_FUNC(tex_write_subtree, out_stream, cur_node->left);  \
                    fprintf_s(out_stream, "}");                                 \
                    fprintf_s(out_stream, "{");                                 \
                    CHECK_FUNC(tex_write_subtree, out_stream, cur_node->right); \
                    fprintf_s(out_stream, "}");                                 \
                    break;
                //This include generates cases for all
                //binary functions by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Tex_operations/Binary_functions.h"
                #undef HANDLE_OPERATION

                #define HANDLE_OPERATION(name, tex_decl, ...)                               \
                case name ## _OPERATION:                                                    \
                    fprintf_s(out_stream, "{");                                             \
                    if (need_left_parenthesize(cur_node->data, cur_node->left->data)) {     \
                        fprintf_s(out_stream, "\\left(");                                   \
                        CHECK_FUNC(tex_write_subtree, out_stream, cur_node->left);          \
                        fprintf_s(out_stream, "\\right)");                                  \
                    }                                                                       \
                    else {                                                                  \
                        CHECK_FUNC(tex_write_subtree, out_stream, cur_node->left);          \
                    }                                                                       \
                    fprintf_s(out_stream, "}");                                             \
                    fprintf_s(out_stream, " " tex_decl " ");                                \
                    fprintf_s(out_stream, "{");                                             \
                    if (need_right_parenthesize(cur_node->data, cur_node->right->data)) {   \
                        fprintf_s(out_stream, "\\left(");                                   \
                        CHECK_FUNC(tex_write_subtree, out_stream, cur_node->right);         \
                        fprintf_s(out_stream, "\\right)");                                  \
                    }                                                                       \
                    else {                                                                  \
                        CHECK_FUNC(tex_write_subtree, out_stream, cur_node->right);         \
                    }                                                                       \
                    fprintf_s(out_stream, "}");                                             \
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

        case EXPRESSION_TREE_ID_TYPE:
            assert(!cur_node->left); assert(!cur_node->right);
            fprintf_s(out_stream, "%s", cur_node->data.val.name);
            break;

        default:
            PRINT_LINE();
            abort();
    }

    return 0;
}

errno_t tex_subtree_dump(FILE *const out_stream, Bin_tree_node const *const node_ptr) {
    assert(out_stream);

    fprintf_s(out_stream, "\\documentclass{article}\n\\begin{document}\n$");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);
    fprintf_s(out_stream, "$\n\\end{document}\n");

    return 0;
}

static errno_t skip_spaces(char const **const cur_pos_ptr) {
    assert(cur_pos_ptr); assert(*cur_pos_ptr);

    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 0, *cur_pos_ptr, " %zn", &extra_len);
    *cur_pos_ptr += extra_len;

    return 0;
}

static bool is_number(char const *const cur_pos) {
    assert(cur_pos);

    char *last_ptr = nullptr;
    strtod(cur_pos, &last_ptr);
    return last_ptr != cur_pos;
}

static errno_t parse_derivative_string(FILE *const out_stream,
                                       Bin_tree_node const *const cur_node, char const *const main_var,
                                       char const **const cur_pos_ptr,
                                       size_t const parent_order, bool const is_right) {
    assert(out_stream); assert(cur_node); assert(main_var); assert(cur_pos_ptr); assert(*cur_pos_ptr);

    CHECK_FUNC(skip_spaces, cur_pos_ptr);
    if (**cur_pos_ptr == '%') {
        *cur_pos_ptr += 1;

        CHECK_FUNC(skip_spaces, cur_pos_ptr);
        switch (**cur_pos_ptr) {
            #define HANDLE_OPERATION(name, tex_decl, left_order, right_order, der_specifier, ...)       \
            case der_specifier:                                                                         \
                *cur_pos_ptr += 1;                                                                      \
                                                                                                        \
                CHECK_FUNC(skip_spaces, cur_pos_ptr);                                                   \
                if (**cur_pos_ptr != '(') { PRINT_LINE(); abort(); }                                    \
                *cur_pos_ptr += 1;                                                                      \
                                                                                                        \
                if ((!is_right and parent_order < left_order) or                                        \
                    (is_right  and parent_order < right_order)) {                                       \
                    fprintf_s(out_stream, "\\left(");                                                   \
                                                                                                        \
                    fprintf_s(out_stream, "{");                                                         \
                    CHECK_FUNC(parse_derivative_string, out_stream, cur_node, main_var, cur_pos_ptr,    \
                                                        left_order, false);                             \
                    fprintf_s(out_stream, "}");                                                         \
                    CHECK_FUNC(skip_spaces, cur_pos_ptr);                                               \
                    if (**cur_pos_ptr != ')') { PRINT_LINE(); abort(); }                                \
                    *cur_pos_ptr += 1;                                                                  \
                    fprintf_s(out_stream, " " tex_decl " ");                                            \
                    CHECK_FUNC(skip_spaces, cur_pos_ptr);                                               \
                    if (**cur_pos_ptr != '(') { PRINT_LINE(); abort(); }                                \
                    *cur_pos_ptr += 1;                                                                  \
                    fprintf_s(out_stream, "{");                                                         \
                    CHECK_FUNC(parse_derivative_string, out_stream, cur_node, main_var, cur_pos_ptr,    \
                                                        right_order, true);                             \
                    fprintf_s(out_stream, "}");                                                         \
                                                                                                        \
                    fprintf_s(out_stream, "\\right)");                                                  \
                }                                                                                       \
                else {                                                                                  \
                    fprintf_s(out_stream, "{");                                                         \
                    CHECK_FUNC(parse_derivative_string, out_stream, cur_node, main_var, cur_pos_ptr,    \
                                                        left_order, false);                             \
                    fprintf_s(out_stream, "}");                                                         \
                    CHECK_FUNC(skip_spaces, cur_pos_ptr);                                               \
                    if (**cur_pos_ptr != ')') { PRINT_LINE(); abort(); }                                \
                    *cur_pos_ptr += 1;                                                                  \
                    fprintf_s(out_stream, " " tex_decl " ");                                            \
                    CHECK_FUNC(skip_spaces, cur_pos_ptr);                                               \
                    if (**cur_pos_ptr != '(') { PRINT_LINE(); abort(); }                                \
                    *cur_pos_ptr += 1;                                                                  \
                    fprintf_s(out_stream, "{");                                                         \
                    CHECK_FUNC(parse_derivative_string, out_stream, cur_node, main_var, cur_pos_ptr,    \
                                                        right_order, true);                             \
                    fprintf_s(out_stream, "}");                                                         \
                }                                                                                       \
                                                                                                        \
                CHECK_FUNC(skip_spaces, cur_pos_ptr);                                                   \
                if (**cur_pos_ptr != ')') { PRINT_LINE(); abort(); }                                    \
                *cur_pos_ptr += 1;                                                                      \
                break;
            //This include generates cases for all
            //binary operator specifiers (except %f) by applying
            //previously declared macros HANDLE_OPERATION to them
            #include "Tex_operations/Binary_operators.h"
            #undef HANDLE_OPERATION

            case 'f':
                *cur_pos_ptr += 1;

                fprintf_s(out_stream, "\\frac");

                CHECK_FUNC(skip_spaces, cur_pos_ptr);
                if (**cur_pos_ptr != '(') { PRINT_LINE(); abort(); }
                *cur_pos_ptr += 1;
                fprintf_s(out_stream, "{");
                CHECK_FUNC(parse_derivative_string, out_stream, cur_node, main_var, cur_pos_ptr, -1, false);
                fprintf_s(out_stream, "}");
                CHECK_FUNC(skip_spaces, cur_pos_ptr);
                if (**cur_pos_ptr != ')') { PRINT_LINE(); abort(); }
                *cur_pos_ptr += 1;
                CHECK_FUNC(skip_spaces, cur_pos_ptr);
                if (**cur_pos_ptr != '(') { PRINT_LINE(); abort(); }
                *cur_pos_ptr += 1;
                fprintf_s(out_stream, "{");
                CHECK_FUNC(parse_derivative_string, out_stream, cur_node, main_var, cur_pos_ptr, -1, true);
                fprintf_s(out_stream, "}");
                CHECK_FUNC(skip_spaces, cur_pos_ptr);
                if (**cur_pos_ptr != ')') { PRINT_LINE(); abort(); }
                *cur_pos_ptr += 1;
                break;

            case '[':
                *cur_pos_ptr += 1;

                CHECK_FUNC(skip_spaces, cur_pos_ptr);
                #define HANDLE_OPERATION(name, tex_decl, text_description, ...)                                 \
                if (!strncmp(*cur_pos_ptr, text_description, strlen(text_description))) {                       \
                    *cur_pos_ptr += strlen(text_description);                                                   \
                    CHECK_FUNC(skip_spaces, cur_pos_ptr);                                                       \
                    if (**cur_pos_ptr != ']') { PRINT_LINE(); abort(); }                                        \
                    *cur_pos_ptr += 1;                                                                          \
                                                                                                                \
                    fprintf_s(out_stream, tex_decl);                                                            \
                    CHECK_FUNC(skip_spaces, cur_pos_ptr);                                                       \
                    if (**cur_pos_ptr != '(') { PRINT_LINE(); abort(); }                                        \
                    *cur_pos_ptr += 1;                                                                          \
                    fprintf_s(out_stream, "\\left(");                                                           \
                    CHECK_FUNC(parse_derivative_string, out_stream, cur_node, main_var, cur_pos_ptr, -1, true); \
                    fprintf_s(out_stream, "\\right)");                                                          \
                    CHECK_FUNC(skip_spaces, cur_pos_ptr);                                                       \
                    if (**cur_pos_ptr != ')') { PRINT_LINE(); abort(); }                                        \
                    *cur_pos_ptr += 1;                                                                          \
                    break;                                                                                      \
                }
                //This include generates branches of
                //detecting and handling all (unary) function specifiers
                //by applying previously declared macros HANDLE_OPERATION
                //to them
                #include "Tex_operations/Unary_functions.h"
                #undef HANDLE_OPERATION

                PRINT_LINE();
                abort();

            case 'd':
                *cur_pos_ptr += 1;

                fprintf_s(out_stream, "\\frac{d}{d%s}", main_var);

                CHECK_FUNC(skip_spaces, cur_pos_ptr);
                if (**cur_pos_ptr != '(') { PRINT_LINE(); abort(); }
                *cur_pos_ptr += 1;
                fprintf_s(out_stream, "\\left(");
                CHECK_FUNC(parse_derivative_string, out_stream, cur_node, main_var, cur_pos_ptr, -1, true);
                fprintf_s(out_stream, "\\right)");
                CHECK_FUNC(skip_spaces, cur_pos_ptr);
                if (**cur_pos_ptr != ')') { PRINT_LINE(); abort(); }
                *cur_pos_ptr += 1;
                break;

            case 'c':
                *cur_pos_ptr += 1;

                if ((!is_right and parent_order < get_left_order(cur_node->data)) or
                    (is_right  and parent_order < get_right_order(cur_node->data))) {
                    fprintf_s(out_stream, "\\left(");
                    CHECK_FUNC(tex_write_subtree, out_stream, cur_node);
                    fprintf_s(out_stream, "\\right)");
                }
                else {
                    CHECK_FUNC(tex_write_subtree, out_stream, cur_node);
                }
                break;

            case 'l':
                *cur_pos_ptr += 1;

                if ((!is_right and parent_order < get_left_order(cur_node->data)) or
                    (is_right  and parent_order < get_right_order(cur_node->data))) {
                    fprintf_s(out_stream, "\\left(");
                    CHECK_FUNC(tex_write_subtree, out_stream, cur_node->left);
                    fprintf_s(out_stream, "\\right)");
                }
                else {
                    CHECK_FUNC(tex_write_subtree, out_stream, cur_node->left);
                }
                break;

            case 'r':
                *cur_pos_ptr += 1;

                if ((!is_right and parent_order < get_left_order(cur_node->data)) or
                    (is_right  and parent_order < get_right_order(cur_node->data))) {
                    fprintf_s(out_stream, "\\left(");
                    CHECK_FUNC(tex_write_subtree, out_stream, cur_node->right);
                    fprintf_s(out_stream, "\\right)");
                }
                else {
                    CHECK_FUNC(tex_write_subtree, out_stream, cur_node->right);
                }
                break;

            default:
                PRINT_LINE();
                abort();
        }

        return 0;
    }

    if (is_number(*cur_pos_ptr)) {
        size_t extra_len = 0;
        double cur_val = 0;
        CHECK_FUNC(My_sscanf_s, 1, *cur_pos_ptr, "%lG%zn", &cur_val, &extra_len);
        *cur_pos_ptr += extra_len;
        fprintf_s(out_stream, "%lG", cur_val);

        return 0;
    }

    PRINT_LINE();
    abort();
}

static errno_t following_tex_step_differentiate_subtree(FILE *const out_stream,
                                                        Bin_tree_node const *const node_ptr,
                                                        char const *const main_var) {
    assert(out_stream); assert(node_ptr); assert(main_var);

    errno_t cur_err = 0;

    fprintf_s(out_stream, "It is obvious that:"); //TODO - modify phrases
    fprintf_s(out_stream, "\n\\\\\n");
    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "\\frac{d}{d%s}", main_var);
    fprintf_s(out_stream, "\\left(");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);
    fprintf_s(out_stream, "\\right)");
    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, " = ");

    char const    *cur_pos   = nullptr;
    Bin_tree_node *dif_node  = nullptr,
                  *simp_node = nullptr;
    switch (node_ptr->data.type) {
        case EXPRESSION_TREE_LITERAL_TYPE:
            assert(!node_ptr->left); assert(!node_ptr->right);

            fprintf_s(out_stream, "$");
            fprintf_s(out_stream, "0");
            fprintf_s(out_stream, "$");
            fprintf_s(out_stream, " - Finally");
            fprintf_s(out_stream, "\n");
            fprintf_s(out_stream, "\\\\\n");
            fprintf_s(out_stream, "\\\\\n");
            break;

        case EXPRESSION_TREE_OPERATION_TYPE:
            switch (node_ptr->data.val.operation) {
                #define HANDLE_OPERATION(name, der_string, ...)                                                 \
                case name ## _OPERATION:                                                                        \
                    fprintf_s(out_stream, "$");                                                                 \
                    cur_pos = der_string;                                                                       \
                    CHECK_FUNC(parse_derivative_string, out_stream, node_ptr, main_var, &cur_pos, -1, true);    \
                    fprintf_s(out_stream, "$");                                                                 \
                    fprintf_s(out_stream, "\n");                                                                \
                    fprintf_s(out_stream, "\\\\\n");                                                            \
                    fprintf_s(out_stream, "\\\\\n");                                                            \
                                                                                                                \
                    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream,                            \
                                                                         node_ptr->right, main_var);            \
                                                                                                                \
                    dif_node = subtree_differentiate(node_ptr, main_var, &cur_err);                             \
                    CHECK_FUNC(simplify_subtree, &simp_node, dif_node);                                         \
                    CHECK_FUNC(subtree_Dtor, dif_node);                                                         \
                    dif_node = nullptr;                                                                         \
                    fprintf_s(out_stream, "So we get that:");                                                   \
                    fprintf_s(out_stream, "\n\\\\\n");                                                          \
                    fprintf_s(out_stream, "$");                                                                 \
                    fprintf_s(out_stream, "\\frac{d}{d%s}", main_var);                                          \
                    fprintf_s(out_stream, "\\left(");                                                           \
                    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);                                        \
                    fprintf_s(out_stream, "\\right)");                                                          \
                    fprintf_s(out_stream, "$");                                                                 \
                    fprintf_s(out_stream, " = ");                                                               \
                    fprintf_s(out_stream, "$");                                                                 \
                    CHECK_FUNC(tex_write_subtree, out_stream, simp_node);                                       \
                    fprintf_s(out_stream, "$");                                                                 \
                    fprintf_s(out_stream, "\n");                                                                \
                    fprintf_s(out_stream, "\\\\\n");                                                            \
                    fprintf_s(out_stream, "\\\\\n");                                                            \
                                                                                                                \
                    CHECK_FUNC(subtree_Dtor, simp_node);                                                        \
                    simp_node = nullptr;                                                                        \
                    break;
                //This include generates cases for all
                //unary operations by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Differentiator_info/Unary_functions.h"
                #undef HANDLE_OPERATION

                #define HANDLE_OPERATION(name, der_string, ...)                                                 \
                case name ## _OPERATION:                                                                        \
                    if (name ## _OPERATION == POW_OPERATION) {                                                  \
                        if (node_ptr->left->data.type == EXPRESSION_TREE_LITERAL_TYPE) {                        \
                            fprintf_s(out_stream, "$");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "{");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "{");                                                         \
                                                                                                                \
                            CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);                                \
                                                                                                                \
                            fprintf_s(out_stream, "}");                                                         \
                            fprintf_s(out_stream, " \\cdot ");                                                  \
                            fprintf_s(out_stream, "{");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "\\ln");                                                      \
                            fprintf_s(out_stream, "\\left(");                                                   \
                            CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->left);                          \
                            fprintf_s(out_stream, "\\right)");                                                  \
                                                                                                                \
                            fprintf_s(out_stream, "}");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "}");                                                         \
                            fprintf_s(out_stream, " \\cdot ");                                                  \
                            fprintf_s(out_stream, "{");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "\\frac{d}{d%s}", main_var);                                  \
                            fprintf_s(out_stream, "\\left(");                                                   \
                            CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);                         \
                            fprintf_s(out_stream, "\\right)");                                                  \
                                                                                                                \
                            fprintf_s(out_stream, "}");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "$");                                                         \
                            fprintf_s(out_stream, "\n");                                                        \
                            fprintf_s(out_stream, "\\\\\n");                                                    \
                            fprintf_s(out_stream, "\\\\\n");                                                    \
                                                                                                                \
                            CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream,                    \
                                                                                 node_ptr->right, main_var);    \
                                                                                                                \
                            dif_node = subtree_differentiate(node_ptr, main_var, &cur_err);                     \
                            CHECK_FUNC(simplify_subtree, &simp_node, dif_node);                                 \
                            CHECK_FUNC(subtree_Dtor, dif_node);                                                 \
                            dif_node = nullptr;                                                                 \
                                                                                                                \
                            fprintf_s(out_stream, "So we get that:");                                           \
                            fprintf_s(out_stream, "\n\\\\\n");                                                  \
                            fprintf_s(out_stream, "$");                                                         \
                            fprintf_s(out_stream, "\\frac{d}{d%s}", main_var);                                  \
                            fprintf_s(out_stream, "\\left(");                                                   \
                            CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);                                \
                            fprintf_s(out_stream, "\\right)");                                                  \
                            fprintf_s(out_stream, "$");                                                         \
                            fprintf_s(out_stream, " = ");                                                       \
                            fprintf_s(out_stream, "$");                                                         \
                            CHECK_FUNC(tex_write_subtree, out_stream, simp_node);                               \
                            fprintf_s(out_stream, "$");                                                         \
                            fprintf_s(out_stream, "\n");                                                        \
                            fprintf_s(out_stream, "\\\\\n");                                                    \
                            fprintf_s(out_stream, "\\\\\n");                                                    \
                                                                                                                \
                            CHECK_FUNC(subtree_Dtor, simp_node);                                                \
                            simp_node = nullptr;                                                                \
                            break;                                                                              \
                        }                                                                                       \
                                                                                                                \
                        if (node_ptr->right->data.type == EXPRESSION_TREE_LITERAL_TYPE) {                       \
                            fprintf_s(out_stream, "$");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "{");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "{");                                                         \
                                                                                                                \
                            CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);                         \
                                                                                                                \
                            fprintf_s(out_stream, "}");                                                         \
                            fprintf_s(out_stream, " \\cdot ");                                                  \
                            fprintf_s(out_stream, "{");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "{");                                                         \
                                                                                                                \
                            if (need_left_parenthesize(Expression_tree_data{                                    \
                                                       EXPRESSION_TREE_OPERATION_TYPE,                          \
                                                       Expression_tree_node_val{.operation = POW_OPERATION}},   \
                                                       node_ptr->left->data)) {                                 \
                                fprintf_s(out_stream, "\\left(");                                               \
                                CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->left);                      \
                                fprintf_s(out_stream, "\\right)");                                              \
                            }                                                                                   \
                            else {                                                                              \
                                CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->left);                      \
                            }                                                                                   \
                                                                                                                \
                            fprintf_s(out_stream, "}");                                                         \
                            fprintf_s(out_stream, " ^ ");                                                       \
                            fprintf_s(out_stream, "{");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "{");                                                         \
                            CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);                         \
                            fprintf_s(out_stream, "}");                                                         \
                            fprintf_s(out_stream, " - ");                                                       \
                            fprintf_s(out_stream, "{");                                                         \
                            fprintf_s(out_stream, "1");                                                         \
                            fprintf_s(out_stream, "}");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "}");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "}");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "}");                                                         \
                            fprintf_s(out_stream, " \\cdot ");                                                  \
                            fprintf_s(out_stream, "{");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "\\frac{d}{d%s}", main_var);                                  \
                            fprintf_s(out_stream, "\\left(");                                                   \
                            CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->left);                          \
                            fprintf_s(out_stream, "\\right)");                                                  \
                                                                                                                \
                            fprintf_s(out_stream, "}");                                                         \
                                                                                                                \
                            fprintf_s(out_stream, "$");                                                         \
                            fprintf_s(out_stream, "\n");                                                        \
                            fprintf_s(out_stream, "\\\\\n");                                                    \
                            fprintf_s(out_stream, "\\\\\n");                                                    \
                                                                                                                \
                            CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream,                    \
                                                                                 node_ptr->left, main_var);     \
                                                                                                                \
                            dif_node = subtree_differentiate(node_ptr, main_var, &cur_err);                     \
                            CHECK_FUNC(simplify_subtree, &simp_node, dif_node);                                 \
                            CHECK_FUNC(subtree_Dtor, dif_node);                                                 \
                            dif_node = nullptr;                                                                 \
                                                                                                                \
                            fprintf_s(out_stream, "So we get that:");                                           \
                            fprintf_s(out_stream, "\n\\\\\n");                                                  \
                            fprintf_s(out_stream, "$");                                                         \
                            fprintf_s(out_stream, "\\frac{d}{d%s}", main_var);                                  \
                            fprintf_s(out_stream, "\\left(");                                                   \
                            CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);                                \
                            fprintf_s(out_stream, "\\right)");                                                  \
                            fprintf_s(out_stream, "$");                                                         \
                            fprintf_s(out_stream, " = ");                                                       \
                            fprintf_s(out_stream, "$");                                                         \
                            CHECK_FUNC(tex_write_subtree, out_stream, simp_node);                               \
                            fprintf_s(out_stream, "$");                                                         \
                            fprintf_s(out_stream, "\n");                                                        \
                            fprintf_s(out_stream, "\\\\\n");                                                    \
                            fprintf_s(out_stream, "\\\\\n");                                                    \
                                                                                                                \
                            CHECK_FUNC(subtree_Dtor, simp_node);                                                \
                            simp_node = nullptr;                                                                \
                            break;                                                                              \
                        }                                                                                       \
                    }                                                                                           \
                                                                                                                \
                    fprintf_s(out_stream, "$");                                                                 \
                    cur_pos = der_string;                                                                       \
                    CHECK_FUNC(parse_derivative_string, out_stream, node_ptr, main_var, &cur_pos, -1, true);    \
                    fprintf_s(out_stream, "$");                                                                 \
                    fprintf_s(out_stream, "\n");                                                                \
                    fprintf_s(out_stream, "\\\\\n");                                                            \
                    fprintf_s(out_stream, "\\\\\n");                                                            \
                                                                                                                \
                    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream,                            \
                                                                         node_ptr->left, main_var);             \
                    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream,                            \
                                                                         node_ptr->right, main_var);            \
                                                                                                                \
                    dif_node = subtree_differentiate(node_ptr, main_var, &cur_err);                             \
                    CHECK_FUNC(simplify_subtree, &simp_node, dif_node);                                         \
                    CHECK_FUNC(subtree_Dtor, dif_node);                                                         \
                    dif_node = nullptr;                                                                         \
                    fprintf_s(out_stream, "So we get that:");                                                   \
                    fprintf_s(out_stream, "\n\\\\\n");                                                          \
                    fprintf_s(out_stream, "$");                                                                 \
                    fprintf_s(out_stream, "\\frac{d}{d%s}", main_var);                                          \
                    fprintf_s(out_stream, "\\left(");                                                           \
                    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);                                        \
                    fprintf_s(out_stream, "\\right)");                                                          \
                    fprintf_s(out_stream, "$");                                                                 \
                    fprintf_s(out_stream, " = ");                                                               \
                    fprintf_s(out_stream, "$");                                                                 \
                    CHECK_FUNC(tex_write_subtree, out_stream, simp_node);                                       \
                    fprintf_s(out_stream, "$");                                                                 \
                    fprintf_s(out_stream, "\n");                                                                \
                    fprintf_s(out_stream, "\\\\\n");                                                            \
                    fprintf_s(out_stream, "\\\\\n");                                                            \
                                                                                                                \
                    CHECK_FUNC(subtree_Dtor, simp_node);                                                        \
                    simp_node = nullptr;                                                                        \
                    break;
                //This include generates cases for all
                //binary operations by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Differentiator_info/Binary_functions.h"
                #undef HANDLE_OPERATION

                default:
                    PRINT_LINE();
                    abort();
            }
            break;

        case EXPRESSION_TREE_ID_TYPE:
            assert(!node_ptr->left); assert(!node_ptr->right);

            if (!strcmp(node_ptr->data.val.name, main_var)) {
                fprintf_s(out_stream, "$");
                fprintf_s(out_stream, "1");
                fprintf_s(out_stream, "$");
                fprintf_s(out_stream, " - Finally");
                fprintf_s(out_stream, "\n");
                fprintf_s(out_stream, "\\\\\n");
                fprintf_s(out_stream, "\\\\\n");
            }
            else {
                fprintf_s(out_stream, "$");
                fprintf_s(out_stream, "0");
                fprintf_s(out_stream, "$");
                fprintf_s(out_stream, " - Finally");
                fprintf_s(out_stream, "\n");
                fprintf_s(out_stream, "\\\\\n");
                fprintf_s(out_stream, "\\\\\n");
            }
            break;

        default:
            PRINT_LINE();
            abort();
    }

    return cur_err;
}

errno_t tex_step_differentiate_subtree(FILE *const out_stream, Bin_tree_node const *const node_ptr,
                                       char const *const main_var) {
    assert(out_stream);

    fprintf_s(out_stream, "\\documentclass{article}\n\\begin{document}\n");

    fprintf_s(out_stream, "Let's calculate");
    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "\\frac{d}{d%s}", main_var);
    fprintf_s(out_stream, "\\left(");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);
    fprintf_s(out_stream, "\\right)");
    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "\\\\\n");
    fprintf_s(out_stream, "\\\\\n");
    fprintf_s(out_stream, "\\\\\n");

    Bin_tree_node *simp_node = nullptr;
    CHECK_FUNC(simplify_subtree, &simp_node, node_ptr);
    fprintf_s(out_stream, "First, let's simplify this. We'll get:");
    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "\\frac{d}{d%s}", main_var);
    fprintf_s(out_stream, "\\left(");
    CHECK_FUNC(tex_write_subtree, out_stream, simp_node);
    fprintf_s(out_stream, "\\right)");
    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "\\\\\n");
    fprintf_s(out_stream, "\\\\\n");

    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, simp_node, main_var);
    CHECK_FUNC(subtree_Dtor, simp_node);
    simp_node = nullptr;

    fprintf_s(out_stream, "\n\\end{document}\n");

    return 0;
}

#undef FINAL_CODE
