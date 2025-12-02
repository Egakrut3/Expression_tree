#include "Expression_tree.h"

#define FINAL_CODE

errno_t Expression_tree_Ctor(Expression_tree *const tree_ptr
                  ON_DEBUG(, Var_info const var_info)) {
    assert(tree_ptr); assert(!tree_ptr->is_valid);
    ON_DEBUG(
    assert(var_info.position.file_name); assert(var_info.position.function_name);
    assert(var_info.name);
    )

    ON_DEBUG(tree_ptr->var_info = var_info;)

    CHECK_FUNC(get_new_Bin_tree_node, &tree_ptr->root, nullptr, nullptr, INITIAL_VAL, false);

    tree_ptr->is_valid = true;
    return 0;
}

errno_t Bin_tree_Dtor(Bin_tree *const tree_ptr) {
    assert(tree_ptr); assert(tree_ptr->is_valid);

    CHECK_FUNC(Bin_subtree_Dtor, tree_ptr->root);

    tree_ptr->is_valid = false;
    return 0;
}

errno_t Bin_tree_verify(Bin_tree const *const tree_ptr, errno_t *const err_ptr) {
    assert(tree_ptr); assert(err_ptr);

    if (!tree_ptr->is_valid) { *err_ptr |= TREE_INVALID; }
    if (!tree_ptr->root)     { *err_ptr |= TREE_NULL_ROOT; }

    CHECK_FUNC(Bin_subtree_verify, tree_ptr->root, err_ptr);

    return 0;
}

#undef FINAL_CODE

errno_t Bin_tree_html_dump(Bin_tree const *const tree_ptr, FILE *const out_stream, size_t const id,
                           Position_info const from_where,
                           bool const need_extra_info, size_t const tab_count) {
    assert(tree_ptr); assert(out_stream);

    #define FINAL_CODE

    char *tab_str = nullptr;
    CHECK_FUNC(My_calloc, (void **)&tab_str, tab_count + 1, sizeof(*tab_str));
    for (size_t i = 0; i < tab_count; ++i) {
        tab_str[i] = '\t';
    }

    #undef FINAL_CODE
    #define FINAL_CODE  \
        free(tab_str);

    errno_t verify_err = 0;
    CHECK_FUNC(Bin_tree_verify, tree_ptr, &verify_err);

    fprintf_s(out_stream, "%s<pre>\n", tab_str);

    if (need_extra_info) {
        ON_DEBUG(
        assert(tree_ptr->var_info.position.file_name); assert(tree_ptr->var_info.position.function_name);
        assert(tree_ptr->var_info.name);
        )
        assert(from_where.file_name); assert(from_where.function_name);

        fprintf_s(out_stream, "%s<font size=\"7\">\n\tDump №%zu\n%s</font>\n\n", tab_str, id, tab_str);

        fprintf_s(out_stream, "%scalled at file %s, line %zu in \"%s\" function for reasons of:\n",
                              tab_str,
                              from_where.file_name, from_where.line, from_where.function_name);

        if (!verify_err) {
            fprintf_s(out_stream, "%s\tNo error\n", tab_str);
        }

        if (verify_err & TREE_NODE_INVALID) {
            fprintf_s(out_stream, "%s\tOne of tree's nodes is invalid\n", tab_str);
        }

        if (verify_err & TREE_NODE_VERIFY_USED) {
            fprintf_s(out_stream, "%s\tOne of tree's nodes has \"verify_used\" field set to true\n",
                                  tab_str);
        }

        if (verify_err & TREE_INVALID_STRUCTURE) {
            fprintf_s(out_stream, "%s\tTree has invalid structure\n", tab_str);
        }

        if (verify_err & TREE_INVALID) {
            fprintf_s(out_stream, "%s\tTree is invalid\n", tab_str);
        }

        if (verify_err & TREE_NULL_ROOT) {
            fprintf_s(out_stream, "%s\tTree has null root\n", tab_str);
        }

        fprintf_s(out_stream, "\n%sBin_tree<" TREE_ELEM_STR ">[%p]"
                     ON_DEBUG(" \"%s\" declared in file %s, line %zu in \"%s\" function")
                              " {\n", tab_str,
                              tree_ptr
                   ON_DEBUG(, tree_ptr->var_info.name,
                              tree_ptr->var_info.position.file_name, tree_ptr->var_info.position.line,
                              tree_ptr->var_info.position.function_name));
    }

    if (verify_err & TREE_INVALID_STRUCTURE) {
        fprintf_s(out_stream, "%s\tInvalid tree structure\n", tab_str); //TODO - probably bad practice
    }
    else {
        #define DOT_FILE_PATH "./Visual_html/DOT_file.txt"

        FILE *dot_stream = nullptr;
        CHECK_FUNC(fopen_s, &dot_stream, DOT_FILE_PATH, "w");

        #undef FINAL_CODE
        #define FINAL_CODE          \
            free(tab_str);          \
            fclose(dot_stream);     \
            dot_stream = nullptr;

        CHECK_FUNC(Bin_subtree_dot_dump, dot_stream, tree_ptr->root);
        fclose(dot_stream);

        #undef FINAL_CODE
        #define FINAL_CODE  \
            free(tab_str);

        size_t const SYS_STR_MAX_SIZE = 100;
        char sys_str[SYS_STR_MAX_SIZE] = {};
        sprintf_s(sys_str, SYS_STR_MAX_SIZE, "dot -Tsvg " DOT_FILE_PATH
                                             " > ./Visual_html/DOT_output%zu.svg", id);
        CHECK_FUNC(system, sys_str);
        fprintf_s(out_stream, "%s\t<img src=\"DOT_output%zu.svg\" width=1000/>\n", tab_str, id);

        #undef DOT_FILE_PATH
    }

    fprintf_s(out_stream, "%s\tis_valid = %d\n", tab_str, tree_ptr->is_valid);

    fprintf_s(out_stream, "%s}\n", tab_str);

    fprintf_s(out_stream, "%s</pre>\n", tab_str);

    return 0;

    #undef FINAL_CODE
}
