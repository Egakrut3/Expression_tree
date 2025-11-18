#ifndef BIN_TREE_H
#define BIN_TREE_H

#include "Common.h"
#include "Bin_tree_node.h"

struct Bin_tree {
    ON_DEBUG(Var_info var_info;)
    Bin_tree_node     *root;

    bool              is_valid;
};

#define INITIAL_VAL "Nothing"
errno_t Bin_tree_Ctor(Bin_tree *const tree_ptr
           ON_DEBUG(, Var_info var_info));
#ifdef _DEBUG
#define BIN_TREE_CREATE(handler, name)                                  \
Bin_tree name = {};                                                     \
handler(Bin_tree_Ctor, &name,                                           \
        Var_info{Position_info{__FILE__, __func__, __LINE__}, #name})
#else
#define BIN_TREE_CREATE(handler, name)                                  \
Bin_tree name = {};                                                     \
handler(Bin_tree_Ctor, &name)
#endif

errno_t Bin_tree_Dtor(Bin_tree *tree_ptr);

#define TREE_INVALID   0B10'000'000'000'000
#define TREE_NULL_ROOT 0B100'000'000'000'000
errno_t Bin_tree_verify(Bin_tree const *tree_ptr, errno_t *err_ptr);

errno_t Bin_tree_html_dump(Bin_tree const *tree_ptr, FILE *out_stream, size_t id,
                           Position_info from_where,
                           bool need_extra_info, size_t tab_count);

#define BIN_TREE_HTML_DUMP(name, out_stream, id, handler)                           \
handler(Bin_tree_html_dump, &name, out_stream, id,                                  \
                            Position_info{__FILE__, __func__, __LINE__}, true, 0)

#endif
