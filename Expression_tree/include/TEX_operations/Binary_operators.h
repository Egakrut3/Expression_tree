/*
HANDLE_OPERATION(name, tex_decl, left_order, right_order, der_specifier)
*/

HANDLE_OPERATION(ADD, "+",      3, 4, 'a')
HANDLE_OPERATION(SUB, "-",      3, 3, 's')

HANDLE_OPERATION(MLT, "\\cdot", 2, 2, 'm')

HANDLE_OPERATION(POW, "^",      1, 1, 'p')
