HANDLE_OPERATION(LN,  "%f(%d(%r))(%r)")

HANDLE_OPERATION(SIN, "%m(       %[cos](%r)) (%d(%r))")
HANDLE_OPERATION(COS, "%m(%m(-1)(%[sin](%r)))(%d(%r))")
