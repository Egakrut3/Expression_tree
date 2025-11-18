#include "Bin_tree_node.h"

#define FINAL_CODE

errno_t split_node(Bin_tree_node *const node_ptr,
                   tree_elem_t const left_val, tree_elem_t const right_val,
                   bool const left_need_copy, bool const right_need_copy) {
    assert(node_ptr);
    ON_DEBUG(
    errno_t verify_err = 0;
    CHECK_FUNC(Bin_tree_node_verify, node_ptr, &verify_err);
    if (verify_err) { PRINT_LINE(); return verify_err; }
    )

    CHECK_FUNC(get_new_Bin_tree_node, &node_ptr->left,  nullptr, nullptr, left_val,  left_need_copy);
    CHECK_FUNC(get_new_Bin_tree_node, &node_ptr->right, nullptr, nullptr, right_val, right_need_copy);

    return 0;
}

static errno_t str_read_subtree_from_pos(Bin_tree_node **const dest, //TODO - make dump not in console
                                         char **const cur_pos) {     //TODO - works only for strings
    assert(dest); assert(cur_pos); assert(*cur_pos);

    fprintf_s(stderr, "Start:\n%s\n", *cur_pos);

    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 0, *cur_pos, " %zn", &extra_len);
    *cur_pos += extra_len;
    fprintf_s(stderr, "%s\n", *cur_pos);

    if (**cur_pos == '(') {
        CHECK_FUNC(My_sscanf_s, 0, *cur_pos, "( \"%zn", &extra_len);
        *cur_pos += extra_len;
        fprintf_s(stderr, "%s\n", *cur_pos);

        CHECK_FUNC(get_new_Bin_tree_node, dest, nullptr, nullptr, *cur_pos, false);

        CHECK_FUNC(My_sscanf_s, 0, *cur_pos, "%*[^\"]\"%zn", &extra_len);
        *cur_pos += extra_len;
        fprintf_s(stderr, "%s\n", *cur_pos);
        *(*cur_pos - 1) = '\0';

        CHECK_FUNC(str_read_subtree_from_pos, &(*dest)->left,  cur_pos);
        fprintf_s(stderr, "After left:\n%s\n", *cur_pos);
        CHECK_FUNC(str_read_subtree_from_pos, &(*dest)->right, cur_pos);
        fprintf_s(stderr, "After right:\n%s\n", *cur_pos);

        CHECK_FUNC(My_sscanf_s, 0, *cur_pos, " )%zn", &extra_len);
        *cur_pos += extra_len;
        fprintf_s(stderr, "Finish:\n%s\n", *cur_pos);

        return 0;
    }

    if (*(*cur_pos)++ == 'n' and
        *(*cur_pos)++ == 'i' and
        *(*cur_pos)++ == 'l') {
        *dest = nullptr;

        return 0;
    }

    return INCORRECT_TREE_INPUT;
}

errno_t str_read_subtree(Bin_tree_node **const dest, char *const buffer) {
    assert(dest); assert(buffer);

    char *cur_pos = buffer;
    CHECK_FUNC(str_read_subtree_from_pos, dest, &cur_pos);

    return 0;
}

errno_t str_copy_read_subtree(Bin_tree_node **const dest, char *const buffer) { //TODO -
    return 0;
}

errno_t read_subtree(Bin_tree_node **const dest, FILE *const in_stream) { //TODO -
    return 0;
}

errno_t write_subtree(Bin_tree_node *const src, FILE *const out_stream) {
    assert(out_stream);

    if (!src) {
        fprintf_s(out_stream, "nil ");

        return 0;
    }

    fprintf_s(out_stream, "(\"" TREE_ELEM_OUT_FRM "\" ", src->val);
    CHECK_FUNC(write_subtree, src->left,  out_stream);
    CHECK_FUNC(write_subtree, src->right, out_stream);
    fprintf_s(out_stream, ")");

    return 0;
}
