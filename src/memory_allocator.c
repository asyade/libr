#include "libr.h"

int memalloc_cmp(void *a, void *b)
{
    if (a == b)
        return (0);
    return (a < b) ? -1 : 1;
}
/*
t_memalloc *memalloc_new(size_t buffer_size)
{
    t_memalloc *retval;
    t_memchunk *chunk;

    if ((chunk = mchunk_alloc(buffer_size)) == NULL)
        return (NULL);
}
*/