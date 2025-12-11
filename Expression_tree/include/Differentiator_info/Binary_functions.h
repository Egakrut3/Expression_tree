HANDLE_OPERATION(ADD, "%a(%d(%l))(%d(%r))")
HANDLE_OPERATION(SUB, "%s(%d(%l))(%d(%r))")

HANDLE_OPERATION(MLT, "%a(%m(%d(%l))(%r))"
                        "(%m(%l)(%d(%r)))")
HANDLE_OPERATION(DIV, "%f(%s(%m(%d(%l))(%r))"
                           "(%m(%l)(%d(%r))))"
                        "(%p(%r)(2))")

HANDLE_OPERATION(POW, "%m(%c)(%a(%m(%d(%r))(%[ln](%l)))"
                               "(%m(%r)(%f(%d(%l))(%l))))")
