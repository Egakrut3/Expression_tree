#include "Bin_tree_node.h"

#define FINAL_CODE

errno_t Bin_tree_node_Ctor(Bin_tree_node *const node_ptr,
                           Bin_tree_node *const left, Bin_tree_node *const right,
                           tree_elem_t const val, bool const need_copy) {
    assert(node_ptr); assert(!node_ptr->is_valid);

    node_ptr->left       = left;
    node_ptr->right      = right;
    node_ptr->need_copy  = need_copy;
    if (node_ptr->need_copy) { TREE_ELEM_COPY(node_ptr->val, val); }
    else                     { node_ptr->val = val; }
    // in order to reuse memory of buffer we've read from file, I must distinguish node, that I must clear and not

    node_ptr->is_valid    = true;
    node_ptr->verify_used = false;
    return 0;
}

errno_t get_new_Bin_tree_node(Bin_tree_node **const dest,
                              Bin_tree_node *const left, Bin_tree_node *const right,
                              tree_elem_t const val, bool const need_copy) {
    CHECK_FUNC(My_calloc, (void **)dest, 1, sizeof(Bin_tree_node));
    CHECK_FUNC(Bin_tree_node_Ctor, *dest, left, right, val, need_copy);

    return 0;
}

errno_t Bin_tree_node_Dtor(Bin_tree_node *const node_ptr) {
    assert(node_ptr); assert(node_ptr->is_valid);

    if (node_ptr->need_copy) { TREE_ELEM_DTOR(node_ptr->val); }

    node_ptr->is_valid = false;
    return 0;
}

errno_t Bin_tree_node_verify(Bin_tree_node const *const node_ptr, errno_t *const err_ptr) {
    assert(node_ptr); assert(err_ptr);

    if (!node_ptr->is_valid)   { *err_ptr |= TREE_NODE_INVALID; }
    if (node_ptr->verify_used) { *err_ptr |= TREE_NODE_VERIFY_USED; }

    return 0;
}

errno_t Bin_subtree_verify(Bin_tree_node *const node_ptr, errno_t *const err_ptr) {
    assert(err_ptr);

    if (!node_ptr) { return 0; }

    if (node_ptr->verify_used) { *err_ptr |= TREE_INVALID_STRUCTURE; return 0; }

    CHECK_FUNC(Bin_tree_node_verify, node_ptr, err_ptr);
    node_ptr->verify_used = true;
    CHECK_FUNC(Bin_subtree_verify, node_ptr->left,  err_ptr);
    CHECK_FUNC(Bin_subtree_verify, node_ptr->right, err_ptr);
    node_ptr->verify_used = false;

    return 0;
}

static errno_t Bin_subtree_Dtor_uncheked(Bin_tree_node *const node_ptr) {
    if (!node_ptr) {
        return 0;
    }

    CHECK_FUNC(Bin_subtree_Dtor_uncheked, node_ptr->left);
    CHECK_FUNC(Bin_subtree_Dtor_uncheked, node_ptr->right);
    CHECK_FUNC(Bin_tree_node_Dtor, node_ptr);

    return 0;
}

errno_t Bin_subtree_Dtor(Bin_tree_node *const node_ptr) {
    errno_t verify_err = 0;
    CHECK_FUNC(Bin_tree_node_verify, node_ptr, &verify_err);
    if (verify_err) { return verify_err; }

    CHECK_FUNC(Bin_subtree_Dtor_uncheked, node_ptr);

    return 0;
}

static uint32_t ptr_hash(void const *const ptr) {
    uint32_t const mlt  = 0X01'00'01'93;
    uint32_t       hash = 0X81'1C'9D'C5;

    for (size_t i = 0; i < sizeof(ptr); ++i) {
        hash = (hash ^ (uint32_t)((size_t)ptr >> i * CHAR_BIT & 0XFF)) * mlt;
    }

    return hash;
}

static uint32_t ptr_color(void const *const ptr) {
    uint32_t hash = ptr_hash(ptr);

    if ((hash       & 0XFF) +
        (hash >> 8  & 0XFF) +
        (hash >> 16 & 0XFF) < 0X80 * 3) {
        hash = ~hash;
    }

    return hash & 0XFF'FF'FF;
}

static errno_t Bin_subtree_following_dot_dump(FILE *const out_stream, Bin_tree_node const *const cur_node) {
    #define BORDER_COLOR      "black"
    #define EMPTY_COLOR       "lightgreen"
    #define LEFT_ARROW_COLOR  "red"
    #define RIGHT_ARROW_COLOR "blue"

    assert(out_stream);

    if (!cur_node) {
        return 0;
    }

    fprintf_s(out_stream, "\tnode%p [shape = plaintext color = " BORDER_COLOR " style = \"\" "
                                     "label = <<TABLE BORDER=\"0\" CELLBORDER=\"1\" "
                                                     "BGCOLOR=\"#%06X\">"
                                     "<TR><TD COLSPAN=\"2\" PORT=\"top\">[%p]</TD></TR>"
                                     "<TR><TD>need_copy = %d</TD><TD>is_valid = %d</TD></TR>"
                                     "<TR><TD COLSPAN=\"2\">verify_used = %d</TD></TR>"
                                     "<TR><TD COLSPAN=\"2\">"
                                         "VAL = " TREE_ELEM_OUT_FRM "</TD></TR>",
                          cur_node,
                          ptr_color(cur_node),
                          cur_node,
                          cur_node->need_copy, cur_node->is_valid,
                          cur_node->verify_used,
                          cur_node->val);

    if (cur_node->left) {
        fprintf_s(out_stream, "<TR><TD PORT=\"left\" BGCOLOR=\"#%06X\">left = %p</TD>",
                              ptr_color(cur_node->left), cur_node->left);
    }
    else {
        fprintf_s(out_stream, "<TR><TD PORT=\"left\" BGCOLOR=\"" EMPTY_COLOR "\">left = nothing</TD>");
    }
    if (cur_node->right) {
        fprintf_s(out_stream, "<TD PORT=\"right\" BGCOLOR=\"#%06X\">right = %p</TD></TR>",
                              ptr_color(cur_node->right), cur_node->right);
    }
    else {
        fprintf_s(out_stream, "<TD PORT=\"right\" BGCOLOR=\"" EMPTY_COLOR "\">right = nothing</TD></TR>");
    }

    fprintf_s(out_stream, "</TABLE>>]\n",
                          cur_node->val);

    if (cur_node->left) {
        fprintf_s(out_stream, "\tnode%p:left -> node%p:top"
                              "[color = " LEFT_ARROW_COLOR "]\n",
                              cur_node, cur_node->left);
        CHECK_FUNC(Bin_subtree_following_dot_dump, out_stream, cur_node->left);
    }

    if(cur_node->right) {
        fprintf_s(out_stream, "\tnode%p:right -> node%p:top"
                              "[color = " RIGHT_ARROW_COLOR "]\n",
                              cur_node, cur_node->right);
        CHECK_FUNC(Bin_subtree_following_dot_dump, out_stream, cur_node->right);
    }

    return 0;

    #undef BORDER_COLOR
    #undef EMPTY_COLOR
    #undef LEFT_ARROW_COLOR
    #undef RIGHT_ARROW_COLOR
}

errno_t Bin_subtree_dot_dump(FILE *const out_stream, Bin_tree_node const *const cur_node) {
    #define BACKGROUND_COLOR  "white"

    assert(out_stream);

    fprintf_s(out_stream, "digraph {\n");
    fprintf_s(out_stream, "\tnode [shape = octagon style = filled fillcolor = red "
                                  "height = 1.0 width = 1.5]\n");
    fprintf_s(out_stream, "\tedge [color = red penwidth = 3.0]\n");
    fprintf_s(out_stream, "\tbgcolor = \"" BACKGROUND_COLOR "\"\n");
    fprintf_s(out_stream, "\n");

    CHECK_FUNC(Bin_subtree_following_dot_dump, out_stream, cur_node);

    fprintf_s(out_stream, "}");

    return 0;

    #undef BACKGROUND_COLOR
}

#undef FINAL_CODE
