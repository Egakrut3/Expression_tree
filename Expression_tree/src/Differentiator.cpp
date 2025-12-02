#include "Bin_tree_node.h"
#include <string.h>
//This file contains functions
//that does not satisfy common code-style,
//since they returns error-code by pointer-parametre,
//not by returning value. This is done for
//the purposes of DSL

#define FINAL_CODE

#define MAIN_VARIABLE "x"
#define DIFF(src) differentiate_subtree(src, err_ptr)
#define COPY(src) copy_subtree(src, err_ptr)

#define LITER_(lit_val)                                                                                 \
get_new_Bin_tree_node(nullptr, nullptr,                                                                 \
                      EXPRESSION_LITERAL_TYPE, expression_val{.val = lit_val}, err_ptr)

#define ADD_(left, right)                                                                               \
get_new_Bin_tree_node(left, right,                                                                      \
                      EXPRESSION_OPERATION_TYPE, expression_val{.operation = ADD_OPERATION}, err_ptr)
#define SUB_(left, right)                                                                               \
get_new_Bin_tree_node(left, right,                                                                      \
                      EXPRESSION_OPERATION_TYPE, expression_val{.operation = SUB_OPERATION}, err_ptr)

#define MLT_(left, right)                                                                               \
get_new_Bin_tree_node(left, right,                                                                      \
                      EXPRESSION_OPERATION_TYPE, expression_val{.operation = MLT_OPERATION}, err_ptr)
#define DIV_(left, right)                                                                               \
get_new_Bin_tree_node(left, right,                                                                      \
                      EXPRESSION_OPERATION_TYPE, expression_val{.operation = DIV_OPERATION}, err_ptr)

#define SIN_(src)                                                                                       \
get_new_Bin_tree_node(nullptr, src,                                                                     \
                      EXPRESSION_OPERATION_TYPE, expression_val{.operation = SIN_OPERATION}, err_ptr)
#define COS_(src)                                                                                       \
get_new_Bin_tree_node(nullptr, src,                                                                     \
                      EXPRESSION_OPERATION_TYPE, expression_val{.operation = COS_OPERATION}, err_ptr)

Bin_tree_node *copy_subtree(Bin_tree_node const *const src, errno_t *const err_ptr) {
    if (!src) { return nullptr; }

    if (src->type == EXPRESSION_VARIABLE_TYPE) {
        return get_new_Bin_tree_node(copy_subtree(src->left, err_ptr), copy_subtree(src->right, err_ptr),
                                     src->type, expression_val{.name = strdup(src->val.name)}, err_ptr);
    }
    else {
        return get_new_Bin_tree_node(copy_subtree(src->left, err_ptr), copy_subtree(src->right, err_ptr),
                                     src->type, src->val, err_ptr);
    }
}

static Bin_tree_node *ADD_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->type == EXPRESSION_OPERATION_TYPE); assert(src->val.operation == ADD_OPERATION);

    return ADD_(DIFF(src->left), DIFF(src->right));
}

static Bin_tree_node *SUB_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->type == EXPRESSION_OPERATION_TYPE); assert(src->val.operation == SUB_OPERATION);

    return SUB_(DIFF(src->left), DIFF(src->right));
}

static Bin_tree_node *MLT_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->type == EXPRESSION_OPERATION_TYPE); assert(src->val.operation == MLT_OPERATION);

    return ADD_(MLT_(DIFF(src->left), COPY(src->right)),
                MLT_(COPY(src->left), DIFF(src->right)));
}

static Bin_tree_node *DIV_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->type == EXPRESSION_OPERATION_TYPE); assert(src->val.operation == DIV_OPERATION);

    return DIV_(SUB_(MLT_(DIFF(src->left), COPY(src->right)),
                     MLT_(COPY(src->left), DIFF(src->right))),
                MLT_(COPY(src->right), COPY(src->right))); //TODO - possible ^2
}

static Bin_tree_node *SIN_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->type == EXPRESSION_OPERATION_TYPE); assert(src->val.operation == SIN_OPERATION);
    assert(!src->left);

    return MLT_(COS_(COPY(src->right)), DIFF(src->right));
}

static Bin_tree_node *COS_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->type == EXPRESSION_OPERATION_TYPE); assert(src->val.operation == COS_OPERATION);
    assert(!src->left);

    return MLT_(MLT_(LITER_(-1), SIN_(COPY(src->right))), DIFF(src->right));
}

Bin_tree_node *differentiate_subtree(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);

    switch (src->type) {
        case EXPRESSION_LITERAL_TYPE:
            assert(!src->left); assert(!src->right);

            return LITER_(0);

        case EXPRESSION_OPERATION_TYPE:
            #define HANDLE_OPERATION(name, ...)                 \
            case name ## _OPERATION:                            \
                return name ## _differentiate(src, err_ptr);
            switch (src->val.operation) {
                //This include generates cases for all
                //operations by applying previously declared
                //macros HANDLE_OPERATION to them
                #include "Operation_list.h"

                default:
                    PRINT_LINE();
                    abort();
            }
            #undef HANDLE_OPERATION

            break;

        case EXPRESSION_VARIABLE_TYPE:
            assert(!src->left); assert(!src->right);

            if (!strcmp(src->val.name, MAIN_VARIABLE)) {
                return LITER_(1);
            }

            return LITER_(0);

        default:
            PRINT_LINE();
            abort();
    }

    return 0;
}

errno_t simplify_subtree(Bin_tree_node **const dest, Bin_tree_node const *const src) {
    assert(src);

    errno_t cur_err = 0;
    errno_t *const err_ptr = &cur_err;

    if (src->type == EXPRESSION_OPERATION_TYPE) {
        Bin_tree_node *left_res  = nullptr,
                      *right_res = nullptr;

        switch (src->val.operation) {
        #define HANDLE_OPERATION(name, decl, ...)                                                           \
        case name ## _OPERATION:                                                                            \
            CHECK_FUNC(simplify_subtree, &right_res, src->right);                                           \
            if (right_res->type == EXPRESSION_LITERAL_TYPE) {                                               \
                *dest = LITER_(decl(right_res->val.val));                                                   \
                CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                                  \
                right_res = nullptr;                                                                        \
            }                                                                                               \
            else {                                                                                          \
                *dest = get_new_Bin_tree_node(nullptr, right_res,                                           \
                                              EXPRESSION_OPERATION_TYPE,                                    \
                                              expression_val{.operation = name ## _OPERATION},              \
                                              err_ptr);                                                     \
            }                                                                                               \
            break;

        //This include generates cases for
        //simplifying all existing unary functions
        //by applying previously declared macros
        //HANDLE_OPERATION to them
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        #include "Differentiator_unary_functions.h"
        #pragma GCC diagnostic pop
        #undef HANDLE_OPERATION





        #define HANDLE_OPERATION(name, decl, left_neutral, right_neutral,                                   \
                                 left_const_crit, left_const_res, right_const_crit, right_const_res, ...)   \
        case name ## _OPERATION:                                                                            \
            CHECK_FUNC(simplify_subtree, &left_res,  src->left);                                            \
            CHECK_FUNC(simplify_subtree, &right_res, src->right);                                           \
            if (left_res->type == EXPRESSION_LITERAL_TYPE) {                                                \
                if (left_res->val.val == left_const_crit) {                                                 \
                    *dest = LITER_(left_const_res);                                                         \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                              \
                    left_res  = nullptr;                                                                    \
                    right_res = nullptr;                                                                    \
                }                                                                                           \
                else if (left_res->val.val == left_neutral) {                                               \
                    *dest = right_res;                                                                      \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                               \
                    left_res = nullptr;                                                                     \
                }                                                                                           \
                else if (right_res->type == EXPRESSION_LITERAL_TYPE) {                                      \
                    *dest = LITER_(decl(left_res->val.val, right_res->val.val));                            \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                              \
                    left_res  = nullptr;                                                                    \
                    right_res = nullptr;                                                                    \
                }                                                                                           \
                else {                                                                                      \
                    *dest = get_new_Bin_tree_node(left_res, right_res,                                      \
                                                  EXPRESSION_OPERATION_TYPE,                                \
                                                  expression_val{.operation = name ## _OPERATION},          \
                                                  err_ptr);                                                 \
                }                                                                                           \
            }                                                                                               \
            else if (right_res->type == EXPRESSION_LITERAL_TYPE) {                                          \
                if (right_res->val.val == right_const_crit) {                                               \
                    *dest = LITER_(right_const_res);                                                        \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                              \
                    left_res  = nullptr;                                                                    \
                    right_res = nullptr;                                                                    \
                }                                                                                           \
                else if (right_res->val.val == right_neutral) {                                             \
                    *dest = left_res;                                                                       \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                              \
                    right_res = nullptr;                                                                    \
                }                                                                                           \
                else {                                                                                      \
                    *dest = get_new_Bin_tree_node(left_res, right_res,                                      \
                                                  EXPRESSION_OPERATION_TYPE,                                \
                                                  expression_val{.operation = name ## _OPERATION},          \
                                                  err_ptr);                                                 \
                }                                                                                           \
            }                                                                                               \
            else {                                                                                          \
                *dest = get_new_Bin_tree_node(left_res, right_res,                                          \
                                              EXPRESSION_OPERATION_TYPE,                                    \
                                              expression_val{.operation = name ## _OPERATION},              \
                                              err_ptr);                                                     \
            }                                                                                               \
            break;

        //This include generates cases for
        //simplifying all existing binary functions
        //by applying previously declared macros
        //HANDLE_OPERATION to them
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        #include "Differentiator_binary_functions.h"
        #pragma GCC diagnostic pop
        #undef HANDLE_OPERATION





        #define HANDLE_OPERATION(name, decl, left_neutral, right_neutral,                                   \
                                 left_const_crit, left_const_res, right_const_crit, right_const_res, ...)   \
        case name ## _OPERATION:                                                                            \
            CHECK_FUNC(simplify_subtree, &left_res,  src->left);                                            \
            CHECK_FUNC(simplify_subtree, &right_res, src->right);                                           \
            if (left_res->type == EXPRESSION_LITERAL_TYPE) {                                                \
                if (left_res->val.val == left_const_crit) {                                                 \
                    *dest = LITER_(left_const_res);                                                         \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                              \
                    left_res  = nullptr;                                                                    \
                    right_res = nullptr;                                                                    \
                }                                                                                           \
                else if (left_res->val.val == left_neutral) {                                               \
                    *dest = right_res;                                                                      \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                               \
                    left_res = nullptr;                                                                     \
                }                                                                                           \
                else if (right_res->type == EXPRESSION_LITERAL_TYPE) {                                      \
                    *dest = LITER_(left_res->val.val decl right_res->val.val);                              \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                              \
                    left_res  = nullptr;                                                                    \
                    right_res = nullptr;                                                                    \
                }                                                                                           \
                else {                                                                                      \
                    *dest = get_new_Bin_tree_node(left_res, right_res,                                      \
                                                  EXPRESSION_OPERATION_TYPE,                                \
                                                  expression_val{.operation = name ## _OPERATION},          \
                                                  err_ptr);                                                 \
                }                                                                                           \
            }                                                                                               \
            else if (right_res->type == EXPRESSION_LITERAL_TYPE) {                                          \
                if (right_res->val.val == right_const_crit) {                                               \
                    *dest = LITER_(right_const_res);                                                        \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                              \
                    left_res  = nullptr;                                                                    \
                    right_res = nullptr;                                                                    \
                }                                                                                           \
                else if (right_res->val.val == right_neutral) {                                             \
                    *dest = left_res;                                                                       \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                                              \
                    right_res = nullptr;                                                                    \
                }                                                                                           \
                else {                                                                                      \
                    *dest = get_new_Bin_tree_node(left_res, right_res,                                      \
                                                  EXPRESSION_OPERATION_TYPE,                                \
                                                  expression_val{.operation = name ## _OPERATION},          \
                                                  err_ptr);                                                 \
                }                                                                                           \
            }                                                                                               \
            else {                                                                                          \
                *dest = get_new_Bin_tree_node(left_res, right_res,                                          \
                                              EXPRESSION_OPERATION_TYPE,                                    \
                                              expression_val{.operation = name ## _OPERATION},              \
                                              err_ptr);                                                     \
            }                                                                                               \
            break;

        //This include generates cases for
        //simplifying all existing binary operators
        //by applying previously declared macros
        //HANDLE_OPERATION to them
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        #include "Differentiator_binary_operators.h"
        #pragma GCC diagnostic pop
        #undef HANDLE_OPERATION

            default:
                PRINT_LINE();
                abort();
        }
        #undef HANDLE_OPERATION

        return 0;
    }

    assert(!src->left); assert(!src->right);

    *dest = COPY(src);

    return 0;
}

#undef FINAL_CODE
