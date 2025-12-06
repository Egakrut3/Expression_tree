#include "Bin_tree_node.h"
#include <string.h>
#include <math.h>
//This file contains functions
//that does not satisfy common code-style,
//since they returns error-code by pointer-parametre,
//not by returning value. This is done for
//the purposes of DSL

#define MAIN_VARIABLE "x" //TODO -

#define COPY(src) copy_subtree(src, err_ptr)
#define DIFF(src) differentiate_subtree(src, err_ptr)

#define LITER_(lit_val)                                                                             \
DSL_new_Bin_tree_node(nullptr, nullptr,                                                             \
                      Expression_token{EXPRESSION_LITERAL_TYPE, Expression_val{.val = lit_val}},    \
                      err_ptr)

#define ADD_(left, right)                                                           \
DSL_new_Bin_tree_node(left, right,                                                  \
                      Expression_token{EXPRESSION_OPERATION_TYPE,                   \
                                       Expression_val{.operation = ADD_OPERATION}}, \
                      err_ptr)
#define SUB_(left, right)                                                           \
DSL_new_Bin_tree_node(left, right,                                                  \
                      Expression_token{EXPRESSION_OPERATION_TYPE,                   \
                                       Expression_val{.operation = SUB_OPERATION}}, \
                      err_ptr)

#define MLT_(left, right)                                                           \
DSL_new_Bin_tree_node(left, right,                                                  \
                      Expression_token{EXPRESSION_OPERATION_TYPE,                   \
                                       Expression_val{.operation = MLT_OPERATION}}, \
                      err_ptr)
#define DIV_(left, right)                                                           \
DSL_new_Bin_tree_node(left, right,                                                  \
                      Expression_token{EXPRESSION_OPERATION_TYPE,                   \
                                       Expression_val{.operation = DIV_OPERATION}}, \
                      err_ptr)

#define LN_(src)                                                                    \
DSL_new_Bin_tree_node(nullptr, src,                                                 \
                      Expression_token{EXPRESSION_OPERATION_TYPE,                   \
                                       Expression_val{.operation = LN_OPERATION}},  \
                      err_ptr)

#define POW_(left, right)                                                           \
DSL_new_Bin_tree_node(left, right,                                                  \
                      Expression_token{EXPRESSION_OPERATION_TYPE,                   \
                                       Expression_val{.operation = POW_OPERATION}}, \
                      err_ptr)

#define SIN_(src)                                                                   \
DSL_new_Bin_tree_node(nullptr, src,                                                 \
                      Expression_token{EXPRESSION_OPERATION_TYPE,                   \
                                       Expression_val{.operation = SIN_OPERATION}}, \
                      err_ptr)
#define COS_(src)                                                                   \
DSL_new_Bin_tree_node(nullptr, src,                                                 \
                      Expression_token{EXPRESSION_OPERATION_TYPE,                   \
                                       Expression_val{.operation = COS_OPERATION}}, \
                      err_ptr)

#define FINAL_CODE

Bin_tree_node *copy_subtree(Bin_tree_node const *const src, errno_t *const err_ptr) {
    if (!src) { return nullptr; }

    if (src->data.type == EXPRESSION_NAME_TYPE) {
        return DSL_new_Bin_tree_node(COPY(src->left), COPY(src->right),
                                     Expression_token{EXPRESSION_NAME_TYPE,
                                                      Expression_val{.name = strdup(src->data.val.name)}},
                                     err_ptr);
    }
    else { return DSL_new_Bin_tree_node(COPY(src->left), COPY(src->right), src->data, err_ptr); }
}

static Bin_tree_node *ADD_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->data.type == EXPRESSION_OPERATION_TYPE); assert(src->data.val.operation == ADD_OPERATION);

    return ADD_(DIFF(src->left), DIFF(src->right));
}

static Bin_tree_node *SUB_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->data.type == EXPRESSION_OPERATION_TYPE); assert(src->data.val.operation == SUB_OPERATION);

    return SUB_(DIFF(src->left), DIFF(src->right));
}

static Bin_tree_node *MLT_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->data.type == EXPRESSION_OPERATION_TYPE); assert(src->data.val.operation == MLT_OPERATION);

    return ADD_(MLT_(DIFF(src->left), COPY(src->right)),
                MLT_(COPY(src->left), DIFF(src->right)));
}

static Bin_tree_node *DIV_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->data.type == EXPRESSION_OPERATION_TYPE); assert(src->data.val.operation == DIV_OPERATION);

    return DIV_(SUB_(MLT_(DIFF(src->left), COPY(src->right)),
                     MLT_(COPY(src->left), DIFF(src->right))),
                POW_(COPY(src->right), LITER_(2)));
}

static Bin_tree_node *LN_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->data.type == EXPRESSION_OPERATION_TYPE); assert(src->data.val.operation == LN_OPERATION);
    assert(!src->left);

    return DIV_(DIFF(src->right), COPY(src->right));
}

static Bin_tree_node *POW_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->data.type == EXPRESSION_OPERATION_TYPE); assert(src->data.val.operation == POW_OPERATION);

    if (src->left->data.type == EXPRESSION_LITERAL_TYPE) {
        assert(!src->left->left); assert(!src->left->right);

        return MLT_(COPY(src),
                    MLT_(DIFF(src->right), LN_(src->left)));
    }

    if (src->right->data.type == EXPRESSION_LITERAL_TYPE) {
        assert(!src->right->left); assert(!src->right->right);

        return MLT_(MLT_(COPY(src->right), POW_(COPY(src->left), SUB_(COPY(src->right), LITER_(1)))),
                         DIFF(src->left));
    }

    return MLT_(COPY(src), ADD_(MLT_(DIFF(src->right), LN_(src->left)),
                                MLT_(COPY(src->right), DIV_(DIFF(src->left), COPY(src->left)))));
}

static Bin_tree_node *SIN_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->data.type == EXPRESSION_OPERATION_TYPE); assert(src->data.val.operation == SIN_OPERATION);
    assert(!src->left);

    return MLT_(COS_(COPY(src->right)), DIFF(src->right));
}

static Bin_tree_node *COS_differentiate(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);
    assert(src->data.type == EXPRESSION_OPERATION_TYPE); assert(src->data.val.operation == COS_OPERATION);
    assert(!src->left);

    return MLT_(MLT_(LITER_(-1), SIN_(COPY(src->right))), DIFF(src->right));
}

Bin_tree_node *differentiate_subtree(Bin_tree_node const *const src, errno_t *const err_ptr) {
    assert(src);

    switch (src->data.type) {
        case EXPRESSION_LITERAL_TYPE:
            assert(!src->left); assert(!src->right);

            return LITER_(0);

        case EXPRESSION_OPERATION_TYPE:
            switch (src->data.val.operation) {
                #define HANDLE_OPERATION(name, ...)                 \
                case name ## _OPERATION:                            \
                    return name ## _differentiate(src, err_ptr);
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

        case EXPRESSION_NAME_TYPE:
            assert(!src->left); assert(!src->right);

            if (!strcmp(src->data.val.name, MAIN_VARIABLE)) {
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

    if (src->data.type != EXPRESSION_OPERATION_TYPE) {
        assert(!src->left); assert(!src->right);

        *dest = COPY(src);

        return 0;
    }

    Bin_tree_node *left_res  = nullptr,
                  *right_res = nullptr;
    switch (src->data.val.operation) {
        #define HANDLE_OPERATION(name, decl, ...)                   \
        case name ## _OPERATION:                                    \
            CHECK_FUNC(simplify_subtree, &right_res, src->right);   \
            if (right_res->data.type == EXPRESSION_LITERAL_TYPE) {  \
                *dest = LITER_(decl(right_res->data.val.val));      \
                CHECK_FUNC(Bin_tree_node_Dtor, right_res);          \
                right_res = nullptr;                                \
            }                                                       \
            else { *dest = name ## _(right_res); }                  \
            break;
        //This include generates cases for
        //simplifying all existing unary functions
        //by applying previously declared macros
        //HANDLE_OPERATION to them
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        #include "Simplifier_info/Unary_functions.h"
        #pragma GCC diagnostic pop
        #undef HANDLE_OPERATION

        #define HANDLE_OPERATION(name, decl, left_neutral, right_neutral,                   \
                                 left_const_crit,  left_const_res,                          \
                                 right_const_crit, right_const_res, ...)                    \
        case name ## _OPERATION:                                                            \
            CHECK_FUNC(simplify_subtree, &left_res,  src->left);                            \
            CHECK_FUNC(simplify_subtree, &right_res, src->right);                           \
            if (left_res->data.type == EXPRESSION_LITERAL_TYPE) {                           \
                if (left_res->data.val.val == left_const_crit) {                            \
                    *dest = LITER_(left_const_res);                                         \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else if (left_res->data.val.val == left_neutral) {                          \
                    *dest = right_res;                                                      \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    left_res = nullptr;                                                     \
                }                                                                           \
                else if (right_res->data.type == EXPRESSION_LITERAL_TYPE) {                 \
                    *dest = LITER_(decl(left_res->data.val.val, right_res->data.val.val));  \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else { *dest = name ## _(left_res, right_res); }                            \
            }                                                                               \
            else if (right_res->data.type == EXPRESSION_LITERAL_TYPE) {                     \
                if (right_res->data.val.val == right_const_crit) {                          \
                    *dest = LITER_(right_const_res);                                        \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else if (right_res->data.val.val == right_neutral) {                        \
                    *dest = left_res;                                                       \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else { *dest = name ## _(left_res, right_res); }                            \
            }                                                                               \
            else { *dest = name ## _(left_res, right_res); }                                \
            break;
        //This include generates cases for
        //simplifying all existing binary functions
        //by applying previously declared macros
        //HANDLE_OPERATION to them
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        #include "Simplifier_info/Binary_functions.h"
        #pragma GCC diagnostic pop
        #undef HANDLE_OPERATION


        #define HANDLE_OPERATION(name, decl, left_neutral, right_neutral,                   \
                                 left_const_crit,  left_const_res,                          \
                                 right_const_crit, right_const_res, ...)                    \
        case name ## _OPERATION:                                                            \
            CHECK_FUNC(simplify_subtree, &left_res,  src->left);                            \
            CHECK_FUNC(simplify_subtree, &right_res, src->right);                           \
            if (left_res->data.type == EXPRESSION_LITERAL_TYPE) {                           \
                if (left_res->data.val.val == left_const_crit) {                            \
                    *dest = LITER_(left_const_res);                                         \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else if (left_res->data.val.val == left_neutral) {                          \
                    *dest = right_res;                                                      \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    left_res = nullptr;                                                     \
                }                                                                           \
                else if (right_res->data.type == EXPRESSION_LITERAL_TYPE) {                 \
                    *dest = LITER_(left_res->data.val.val decl right_res->data.val.val);    \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else { *dest = name ## _(left_res, right_res); }                            \
            }                                                                               \
            else if (right_res->data.type == EXPRESSION_LITERAL_TYPE) {                     \
                if (right_res->data.val.val == right_const_crit) {                          \
                    *dest = LITER_(right_const_res);                                        \
                    CHECK_FUNC(Bin_tree_node_Dtor, left_res);                               \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    left_res  = nullptr;                                                    \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else if (right_res->data.val.val == right_neutral) {                        \
                    *dest = left_res;                                                       \
                    CHECK_FUNC(Bin_tree_node_Dtor, right_res);                              \
                    right_res = nullptr;                                                    \
                }                                                                           \
                else { *dest = name ## _(left_res, right_res); }                            \
            }                                                                               \
            else { *dest = name ## _(left_res, right_res); }                                \
            break;
        //This include generates cases for
        //simplifying all existing binary operators
        //by applying previously declared macros
        //HANDLE_OPERATION to them
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        #include "Simplifier_info/Binary_operators.h"
        #pragma GCC diagnostic pop
        #undef HANDLE_OPERATION

        default:
            PRINT_LINE();
            abort();
    }

    return 0;
}

#undef FINAL_CODE
