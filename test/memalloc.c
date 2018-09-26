#include "libr.h"

int main()
{
    t_memalloc allocator;

    printf("New allocator return : %d\n", memalloc_new(&allocator, 4096, 16));
}