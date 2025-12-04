#include "Bin_tree_node.h"
#include <string.h>

#define FINAL_CODE

static errno_t str_prefix_read_subtree_from_pos(Bin_tree_node **const dest,
                                                char const **const cur_pos) {
    assert(dest); assert(cur_pos); assert(*cur_pos);

    errno_t cur_err = 0;
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 0, *cur_pos, " %zn", &extra_len);
    *cur_pos += extra_len;

    if (**cur_pos == '(') {
        CHECK_FUNC(My_sscanf_s, 0, *cur_pos, "( %zn", &extra_len);
        *cur_pos += extra_len;

        //CHECK_FUNC(new_Bin_tree_node, dest, nullptr, nullptr, EXPRESSION_LITERAL_TYPE, expression_val{});
        *dest = new_Bin_tree_node(nullptr, nullptr, EXPRESSION_LITERAL_TYPE, expression_val{}, &cur_err); //TODO - I can't set default value to (*dest)->type

        CHECK_FUNC(My_sscanf_s, 0, *cur_pos, "%*s%zn", &extra_len);

        #define HANDLE_OPERATION(name, text_description, ...)                       \
        else if (!strncmp(*cur_pos, text_description, strlen(text_description))) {  \
            (*dest)->type          = EXPRESSION_OPERATION_TYPE;                     \
            (*dest)->val.operation = name ## _OPERATION;                            \
        }
        char *last_ptr = nullptr;
        strtod(*cur_pos, &last_ptr);
        if (last_ptr == *cur_pos + extra_len) {
            (*dest)->type = EXPRESSION_LITERAL_TYPE;
            CHECK_FUNC(My_sscanf_s, 1, *cur_pos, "%lG", &(*dest)->val.val);
        }
        //This include generates branches of
        //detecting and handling text description
        //of all existing operations by applying
        //previously declared macros HANDLE_OPERATION
        //to them
        #include "Text_operations/Unary_functions.h"
        #include "Text_operations/Binary_functions.h"
        #include "Text_operations/Binary_operators.h"
        else {
            (*dest)->type = EXPRESSION_VARIABLE_TYPE;
            CHECK_FUNC(My_calloc, (void **)&(*dest)->val.name, extra_len + 1, sizeof(*(*dest)->val.name));
            CHECK_FUNC(strncpy_s, (*dest)->val.name, extra_len + 1, *cur_pos, extra_len);
        }
        *cur_pos += extra_len;
        #undef HANDLE_OPERATION

        CHECK_FUNC(str_prefix_read_subtree_from_pos, &(*dest)->left,  cur_pos);
        CHECK_FUNC(str_prefix_read_subtree_from_pos, &(*dest)->right, cur_pos);

        CHECK_FUNC(My_sscanf_s, 0, *cur_pos, " %zn", &extra_len);
        *cur_pos += extra_len;

        if (**cur_pos != ')') { return INCORRECT_TREE_INPUT; }

        *cur_pos += 1;
        return 0;
    }

    if (!strncmp(*cur_pos, "nil", strlen("nil"))) {
        *cur_pos += strlen("nil");
        *dest = nullptr;

        return 0;
    }

    return INCORRECT_TREE_INPUT;
}

errno_t str_prefix_read_subtree(Bin_tree_node **const dest, char const *const buffer) {
    char const *cur_pos = buffer;
    CHECK_FUNC(str_prefix_read_subtree_from_pos, dest, &cur_pos);

    return 0;
}

errno_t prefix_write_subtree(Bin_tree_node *const src, FILE *const out_stream) {
    assert(out_stream);

    if (!src) { fprintf_s(out_stream, "nil "); return 0; }

    fprintf_s(out_stream, "(");

    switch (src->type) {
        case EXPRESSION_LITERAL_TYPE:
            fprintf_s(out_stream, "%lG ", src->val.val);
            break;

        case EXPRESSION_OPERATION_TYPE:
            switch (src->val.operation) {
                #define HANDLE_OPERATION(name, text_description, ...)   \
                case name ## _OPERATION:                                \
                    fprintf_s(out_stream, text_description " ");        \
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

        case EXPRESSION_VARIABLE_TYPE:
            fprintf_s(out_stream, "%s ", src->val.name);
            break;

        default:
            PRINT_LINE();
            abort();
    }

    CHECK_FUNC(prefix_write_subtree, src->left,  out_stream);
    CHECK_FUNC(prefix_write_subtree, src->right, out_stream);
    fprintf_s(out_stream, ")");

    return 0;
}

static errno_t skip_spaces(char const **const cur_pos) {
    assert(cur_pos); assert(*cur_pos);

    size_t extra_len = 0;

    CHECK_FUNC(My_sscanf_s, 0, *cur_pos, " %zn", &extra_len);
    *cur_pos += extra_len;

    return 0;
}

static errno_t read_N(Bin_tree_node **const dest, char const **const cur_pos) {
    assert(dest); assert(cur_pos); assert(*cur_pos);

    errno_t cur_err = 0;
    size_t extra_len = 0;

    *dest = new_Bin_tree_node(nullptr, nullptr,
                              EXPRESSION_LITERAL_TYPE,
                              expression_val{}, &cur_err);

    double cur_val = 0;
    CHECK_FUNC(skip_spaces, cur_pos);
    fprintf_s(stderr, "N\n%s\n", *cur_pos);
    CHECK_FUNC(My_sscanf_s, 1, *cur_pos, "%lG%zn", &cur_val, &extra_len);
    (*dest)->val.val = cur_val;
    *cur_pos += extra_len;

    return cur_err;
}

static errno_t read_ID(Bin_tree_node **const dest, char const **cur_pos) {
    assert(dest); assert(cur_pos); assert(*cur_pos);

    errno_t cur_err = 0;
    size_t extra_len = 0;

    *dest = new_Bin_tree_node(nullptr, nullptr,
                              EXPRESSION_VARIABLE_TYPE,
                              expression_val{}, &cur_err);

    CHECK_FUNC(skip_spaces, cur_pos);
    fprintf_s(stderr, "ID\n%s\n", *cur_pos);
    CHECK_FUNC(My_sscanf_s, 1, *cur_pos, "%*[^+-*/,)$]%zn", &extra_len);
    CHECK_FUNC(My_calloc, (void **)&(*dest)->val.name, extra_len + 1, sizeof(*(*dest)->val.name));
    CHECK_FUNC(strncpy_s, (*dest)->val.name, extra_len + 1, *cur_pos, extra_len);
    *cur_pos += extra_len;

    return cur_err;
}

static errno_t read_E(Bin_tree_node **dest, char const **cur_pos);

static errno_t read_P(Bin_tree_node **const dest, char const **const cur_pos) {
    assert(dest); assert(cur_pos); assert(*cur_pos);

    errno_t cur_err = 0;
    size_t extra_len = 0;
    CHECK_FUNC(skip_spaces, cur_pos);
    fprintf_s(stderr, "P\n%s\n", *cur_pos);

    if (**cur_pos == '(') {
        *cur_pos += 1;

        CHECK_FUNC(read_E, dest, cur_pos);

        CHECK_FUNC(skip_spaces, cur_pos);
        if (**cur_pos != ')') { return INCORRECT_TREE_INPUT; }
        *cur_pos += 1;

        return cur_err;
    }

    #define HANDLE_OPERATION(name, text_description, ...)                           \
    if (!strncmp(*cur_pos, text_description, strlen(text_description))) {           \
        *dest = new_Bin_tree_node(nullptr, nullptr,                                 \
                                  EXPRESSION_OPERATION_TYPE,                        \
                                  expression_val{.operation = name ## _OPERATION},  \
                                  &cur_err);                                        \
                                                                                    \
        *cur_pos += strlen(text_description);                                       \
                                                                                    \
        CHECK_FUNC(skip_spaces, cur_pos);                                           \
        if (**cur_pos != '(') { return INCORRECT_TREE_INPUT; }                      \
        *cur_pos += 1;                                                              \
                                                                                    \
        CHECK_FUNC(read_E, &(*dest)->right, cur_pos);                               \
                                                                                    \
        CHECK_FUNC(skip_spaces, cur_pos);                                           \
        if (**cur_pos != ')') { return INCORRECT_TREE_INPUT; }                      \
        *cur_pos += 1;                                                              \
                                                                                    \
        return cur_err;                                                             \
    }
    //This include generates branches of
    //detecting and handling text description
    //of unary functions by applying
    //previously declared macros HANDLE_OPERATION
    //to them
    #include "Text_operations/Unary_functions.h"
    #undef HANDLE_OPERATION

    #define HANDLE_OPERATION(name, text_description, ...)                           \
    if (!strncmp(*cur_pos, text_description, strlen(text_description))) {           \
        *dest = new_Bin_tree_node(nullptr, nullptr,                                 \
                                  EXPRESSION_OPERATION_TYPE,                        \
                                  expression_val{.operation = name ## _OPERATION},  \
                                  &cur_err);                                        \
                                                                                    \
        *cur_pos += strlen(text_description);                                       \
                                                                                    \
        CHECK_FUNC(skip_spaces, cur_pos);                                           \
        if (**cur_pos != '(') { return INCORRECT_TREE_INPUT; }                      \
        *cur_pos += 1;                                                              \
                                                                                    \
        CHECK_FUNC(read_E, &(*dest)->left,  cur_pos);                               \
                                                                                    \
        CHECK_FUNC(skip_spaces, cur_pos);                                           \
        if (**cur_pos != ',') { return INCORRECT_TREE_INPUT; }                      \
        *cur_pos += 1;                                                              \
                                                                                    \
        CHECK_FUNC(read_E, &(*dest)->right, cur_pos);                               \
                                                                                    \
        CHECK_FUNC(skip_spaces, cur_pos);                                           \
        if (**cur_pos != ')') { return INCORRECT_TREE_INPUT; }                      \
        *cur_pos += 1;                                                              \
                                                                                    \
        return cur_err;                                                             \
    }
    //This include generates branches of
    //detecting and handling text description
    //of binary functions by applying
    //previously declared macros HANDLE_OPERATION
    //to them
    #include "Text_operations/Binary_functions.h"
    #undef HANDLE_OPERATION

    CHECK_FUNC(skip_spaces, cur_pos);
    CHECK_FUNC(My_sscanf_s, 1, *cur_pos, "%*[^+-*/,)$]%zn", &extra_len);

    char *last_ptr = nullptr;
    strtod(*cur_pos, &last_ptr);
    if (last_ptr == *cur_pos + extra_len) { CHECK_FUNC(read_N, dest, cur_pos); return cur_err; }

    CHECK_FUNC(read_ID, dest, cur_pos); //TODO - possible if with INCORRECT_TREE_INPUT

    return cur_err;
}

static errno_t read_T(Bin_tree_node **const dest, char const **const cur_pos) {
    assert(cur_pos); assert(*cur_pos);

    fprintf_s(stderr, "T\n%s\n", *cur_pos);

    errno_t cur_err = 0;

    CHECK_FUNC(read_P, dest, cur_pos);

    while (true) { //TODO - possible code-generation
        CHECK_FUNC(skip_spaces, cur_pos);
        if (**cur_pos == '*') {
            *dest = new_Bin_tree_node(*dest, nullptr,
                                      EXPRESSION_OPERATION_TYPE,
                                      expression_val{.operation = MLT_OPERATION}, &cur_err);
            *cur_pos += 1;

            CHECK_FUNC(read_P, &(*dest)->right, cur_pos);
        }
        else if (**cur_pos == '/') {
            *dest = new_Bin_tree_node(*dest, nullptr,
                                      EXPRESSION_OPERATION_TYPE,
                                      expression_val{.operation = DIV_OPERATION}, &cur_err);
            *cur_pos += 1;

            CHECK_FUNC(read_P, &(*dest)->right, cur_pos);
        }
        else { return cur_err; }
    }

    PRINT_LINE();
    abort();
}

static errno_t read_E(Bin_tree_node **const dest, char const **const cur_pos) {
    assert(cur_pos); assert(*cur_pos);

    fprintf_s(stderr, "E\n%s\n", *cur_pos);

    errno_t cur_err = 0;

    CHECK_FUNC(read_T, dest, cur_pos);

    while (true) { //TODO - possible code-generation
        CHECK_FUNC(skip_spaces, cur_pos);
        if (**cur_pos == '+') {
            *dest = new_Bin_tree_node(*dest, nullptr,
                                      EXPRESSION_OPERATION_TYPE,
                                      expression_val{.operation = ADD_OPERATION}, &cur_err);

            *cur_pos += 1;

            CHECK_FUNC(read_T, &(*dest)->right, cur_pos);
        }
        else if (**cur_pos == '-') {
            *dest = new_Bin_tree_node(*dest, nullptr,
                                      EXPRESSION_OPERATION_TYPE,
                                      expression_val{.operation = SUB_OPERATION}, &cur_err);

            *cur_pos += 1;

            CHECK_FUNC(read_T, &(*dest)->right, cur_pos);
        }
        else { return cur_err; }
    }

    PRINT_LINE();
    abort();
}

static errno_t read_G(Bin_tree_node **const dest, char const **const cur_pos) {
    assert(cur_pos); assert(*cur_pos);

    fprintf_s(stderr, "G\n%s\n", *cur_pos);

    CHECK_FUNC(read_E, dest, cur_pos);

    CHECK_FUNC(skip_spaces, cur_pos);
    if (**cur_pos != '$') { return INCORRECT_TREE_INPUT; }
    *cur_pos += 1;

    return 0;
}

errno_t str_infix_read_subtree(Bin_tree_node **const dest, char const *const buffer) {
    char const *cur_pos = buffer;
    CHECK_FUNC(read_G, dest, &cur_pos);

    return 0;
}

#undef FINAL_CODE
