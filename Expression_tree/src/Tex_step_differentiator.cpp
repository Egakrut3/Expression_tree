#include "Bin_tree_node.h"
#include <string.h>

#define FINAL_CODE

static errno_t tex_write_subtree_derivative(FILE *const out_stream, Bin_tree_node const *const node_ptr,
                                            char const *const main_var) {
    assert(out_stream);

    fprintf_s(out_stream, "\\frac{d}{d%s}\\left(", main_var);

    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);

    fprintf_s(out_stream, "\\right)");

    return 0;
}

static errno_t following_tex_step_differentiate_subtree(FILE *out_stream,
                                                        Bin_tree_node const *node_ptr,
                                                        char const *main_var);

static errno_t ADD_differentiate(FILE *const out_stream, Bin_tree_node const *const node_ptr,
                                 char const *const main_var) {
    assert(out_stream); assert(node_ptr);
    assert(node_ptr->data.type == EXPRESSION_TREE_OPERATION_TYPE);
    assert(node_ptr->data.val.operation == ADD_OPERATION);

    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->left, main_var);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " + ");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->right, main_var);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "$");

    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->left, main_var);
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->right, main_var);

    return 0;
}

static errno_t SUB_differentiate(FILE *const out_stream, Bin_tree_node const *const node_ptr,
                                 char const *const main_var) {
    assert(out_stream); assert(node_ptr);
    assert(node_ptr->data.type == EXPRESSION_TREE_OPERATION_TYPE);
    assert(node_ptr->data.val.operation == SUB_OPERATION);

    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->left, main_var);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " - ");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->right, main_var);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "$");

    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->left, main_var);
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->right, main_var);

    return 0;
}

static errno_t MLT_differentiate(FILE *const out_stream, Bin_tree_node const *const node_ptr,
                                 char const *const main_var) {
    assert(out_stream); assert(node_ptr);
    assert(node_ptr->data.type == EXPRESSION_TREE_OPERATION_TYPE);
    assert(node_ptr->data.val.operation == MLT_OPERATION);

    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->left, main_var);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " \\cdot ");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);
    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " + ");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->left);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " \\cdot ");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->right, main_var);
    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "$");

    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->left, main_var);
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->right, main_var);

    return 0;
}

static errno_t DIV_differentiate(FILE *const out_stream, Bin_tree_node const *const node_ptr,
                                 char const *const main_var) {
    assert(out_stream); assert(node_ptr);
    assert(node_ptr->data.type == EXPRESSION_TREE_OPERATION_TYPE);
    assert(node_ptr->data.val.operation == DIV_OPERATION);

    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "\\frac");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->left, main_var);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " \\cdot ");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);
    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " - ");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->left);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " \\cdot ");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->right, main_var);
    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " ^ ");
    fprintf_s(out_stream, "{");
    fprintf_s(out_stream, "2");
    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "$");

    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->left, main_var);
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->right, main_var);

    return 0;
}

static errno_t LN_differentiate(FILE *const out_stream, Bin_tree_node const *const node_ptr,
                                char const *const main_var) {
    assert(out_stream); assert(node_ptr);
    assert(node_ptr->data.type == EXPRESSION_TREE_OPERATION_TYPE);
    assert(node_ptr->data.val.operation == LN_OPERATION);
    assert(!node_ptr->left);

    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "\\frac");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->right, main_var);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "$");

    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->right, main_var);

    return 0;
}

static errno_t POW_differentiate(FILE *const out_stream, Bin_tree_node const *const node_ptr,
                                 char const *const main_var) {
    assert(out_stream); assert(node_ptr);
    assert(node_ptr->data.type == EXPRESSION_TREE_OPERATION_TYPE);
    assert(node_ptr->data.val.operation == POW_OPERATION);

    if (node_ptr->left->data.type == EXPRESSION_TREE_LITERAL_TYPE) {
        assert(!node_ptr->left->left); assert(!node_ptr->left->right);

        fprintf_s(out_stream, "$");
        fprintf_s(out_stream, "{");

        fprintf_s(out_stream, "{");

        CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);

        fprintf_s(out_stream, "}");
        fprintf_s(out_stream, " \\cdot ");
        fprintf_s(out_stream, "{");

        fprintf_s(out_stream, "\\ln");
        fprintf_s(out_stream, "(");
        CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->left);
        fprintf_s(out_stream, ")");

        fprintf_s(out_stream, "}");

        fprintf_s(out_stream, "}");
        fprintf_s(out_stream, " \\cdot ");
        fprintf_s(out_stream, "{");

        CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->right, main_var);

        fprintf_s(out_stream, "}");
        fprintf_s(out_stream, "$");

        fprintf_s(out_stream, "\n");
        fprintf_s(out_stream, "\\\\\n");
        CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->right, main_var);

        return 0;
    }

    if (node_ptr->right->data.type == EXPRESSION_TREE_LITERAL_TYPE) {
        assert(!node_ptr->right->left); assert(!node_ptr->right->right);

        fprintf_s(out_stream, "$");
        fprintf_s(out_stream, "{");

        fprintf_s(out_stream, "{");

        CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);

        fprintf_s(out_stream, "}");
        fprintf_s(out_stream, " \\cdot ");
        fprintf_s(out_stream, "{");

        fprintf_s(out_stream, "{");

        CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->left);

        fprintf_s(out_stream, "}");
        fprintf_s(out_stream, " ^ ");
        fprintf_s(out_stream, "{");

        fprintf_s(out_stream, "{");
        CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);
        fprintf_s(out_stream, "}");
        fprintf_s(out_stream, " - ");
        fprintf_s(out_stream, "{");
        fprintf_s(out_stream, "1");
        fprintf_s(out_stream, "}");

        fprintf_s(out_stream, "}");

        fprintf_s(out_stream, "}");

        fprintf_s(out_stream, "}");
        fprintf_s(out_stream, " \\cdot ");
        fprintf_s(out_stream, "{");

        CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->left, main_var);

        fprintf_s(out_stream, "}");
        fprintf_s(out_stream, "$");

        fprintf_s(out_stream, "\n");
        fprintf_s(out_stream, "\\\\\n");
        CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->left, main_var);

        return 0;
    }

    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "{");

    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr);

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " \\cdot ");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "{");

    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->right, main_var);

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " \\cdot ");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "\\ln");
    fprintf_s(out_stream, "(");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->left);
    fprintf_s(out_stream, ")");

    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " + ");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "{");

    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " \\cdot ");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "\\frac");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->left, main_var);
    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "{");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->left);
    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "$");

    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->left, main_var);
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->right, main_var);

    return 0;
}

static errno_t SIN_differentiate(FILE *const out_stream, Bin_tree_node const *const node_ptr,
                                 char const *const main_var) {
    assert(out_stream); assert(node_ptr);
    assert(node_ptr->data.type == EXPRESSION_TREE_OPERATION_TYPE);
    assert(node_ptr->data.val.operation == SIN_OPERATION);
    assert(!node_ptr->left);

    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "\\cos");
    fprintf_s(out_stream, "(");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);
    fprintf_s(out_stream, ")");

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " \\cdot ");
    fprintf_s(out_stream, "{");

    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->right, main_var);

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "$");

    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->right, main_var);

    return 0;
}

static errno_t COS_differentiate(FILE *const out_stream, Bin_tree_node const *const node_ptr,
                                 char const *const main_var) {
    assert(out_stream); assert(node_ptr);
    assert(node_ptr->data.type == EXPRESSION_TREE_OPERATION_TYPE);
    assert(node_ptr->data.val.operation == COS_OPERATION);
    assert(!node_ptr->left);

    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "-1");

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " \\cdot ");
    fprintf_s(out_stream, "{");

    fprintf_s(out_stream, "\\sin");
    fprintf_s(out_stream, "(");
    CHECK_FUNC(tex_write_subtree, out_stream, node_ptr->right);
    fprintf_s(out_stream, ")");

    fprintf_s(out_stream, "}");

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, " \\cdot ");
    fprintf_s(out_stream, "{");

    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr->right, main_var);

    fprintf_s(out_stream, "}");
    fprintf_s(out_stream, "$");

    fprintf_s(out_stream, "\n");
    fprintf_s(out_stream, "\\\\\n");
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr->right, main_var);

    return 0;
}

static errno_t following_tex_step_differentiate_subtree(FILE *const out_stream,
                                                        Bin_tree_node const *const node_ptr,
                                                        char const *const main_var) {
    assert(out_stream); assert(node_ptr);

    errno_t cur_err = 0;

    fprintf_s(out_stream, "It is obvious that:\n\\\\\n"); //TODO - modify phrases
    fprintf_s(out_stream, "$");
    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr, main_var);
    fprintf_s(out_stream, "$");
    fprintf_s(out_stream, " = ");

    Bin_tree_node *dif_node  = nullptr,
                  *simp_node = nullptr;
    switch (node_ptr->data.type) {
        case EXPRESSION_TREE_LITERAL_TYPE:
            assert(!node_ptr->left); assert(!node_ptr->right);

            fprintf_s(out_stream, "$");
            fprintf_s(out_stream, "0");
            fprintf_s(out_stream, "$");
            fprintf_s(out_stream, " - Finally\n\\\\\n");
            break;

        case EXPRESSION_TREE_OPERATION_TYPE:
            switch (node_ptr->data.val.operation) {
                #define HANDLE_OPERATION(name, ...)                                             \
                case name ## _OPERATION:                                                        \
                    CHECK_FUNC(name ## _differentiate, out_stream, node_ptr, main_var);         \
                    fprintf_s(out_stream, "\\\\\n");                                            \
                                                                                                \
                    dif_node = subtree_differentiate(node_ptr, main_var, &cur_err);             \
                    MAIN_CHECK_FUNC(simplify_subtree, &simp_node, dif_node);                    \
                    CHECK_FUNC(subtree_Dtor, dif_node);                                         \
                    dif_node = nullptr;                                                         \
                                                                                                \
                    fprintf_s(out_stream, "So we get that:\n\\\\\n");                           \
                    fprintf_s(out_stream, "$");                                                 \
                    CHECK_FUNC(tex_write_subtree_derivative, out_stream, node_ptr, main_var);   \
                    fprintf_s(out_stream, "$");                                                 \
                    fprintf_s(out_stream, " = ");                                               \
                    fprintf_s(out_stream, "$");                                                 \
                    CHECK_FUNC(tex_write_subtree, out_stream, simp_node);                       \
                    fprintf_s(out_stream, "$");                                                 \
                    fprintf_s(out_stream, "\n\\\\\n");                                          \
                                                                                                \
                    CHECK_FUNC(subtree_Dtor, simp_node);                                        \
                    simp_node = nullptr;                                                        \
                    break;
                //This include generates cases for all
                //operations by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Text_operations/Unary_functions.h"
                #include "Text_operations/Binary_functions.h"
                #include "Text_operations/Binary_operators.h"
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
                fprintf_s(out_stream, " - Finally\n\\\\\n");
                break;
            }
            fprintf_s(out_stream, "$");
            fprintf_s(out_stream, "0");
            fprintf_s(out_stream, "$");
            fprintf_s(out_stream, " - Finally\n\\\\\n");
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
    CHECK_FUNC(following_tex_step_differentiate_subtree, out_stream, node_ptr, main_var);
    fprintf_s(out_stream, "\n\\end{document}\n");

    return 0;
}

#undef FINAL_CODE
