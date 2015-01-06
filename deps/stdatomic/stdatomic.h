#if defined(__has_include_next) && __has_include_next(<stdatomic.h>)
#include_next <stdatomic.h>
#elif __clang__
#include "stdatomic_clang.h"
#else
#include "stdatomic_portable.h"
#endif
