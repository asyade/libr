#include "libr.h"
#include <stdlib.h>

#define COUNT 100

int cmpf(void *pa, void *pb)
{

    int a = *(int *)pa;
    int b = *(int *)pb;
    if (a == b)
        return (0);
    else
        return (a > b) ? 1 : -1;
}

void print_heap(t_bheap *heap)
{
    int *ptr = (int *)(heap + 1);
    size_t i = 0;
    size_t x = 1;
    while (i < heap->size)
    {
        size_t z = 0;
        while (z < x && z + i < heap->size)
        {
            ft_putfmt("%d ", ptr[z++ + i]);
        }
        i += x;
        x *= 2;
        ft_putfmt("\n");
    }
    ft_putfmt("\n");
}

int main(int ac, char **av)
{
    size_t buffer_size = (COUNT * sizeof(int) + sizeof(t_bheap));
    srand((ac > 1) ? atoi(av[1]) : 42);
    t_bheap *heap = bheap_new(mchunk_alloc(buffer_size) + 1, buffer_size, sizeof(int), cmpf);

    int vals[COUNT];
    for (int i = 0; i < COUNT; i++)
    {
        vals[i] = i;
    }
    for (int i = 0; i < COUNT; i++)
    {
        int npos = rand() % COUNT;
        int tmp = vals[npos];
        vals[npos] = vals[i];
        vals[i] = tmp;
    }
    for (int i = 0; i < COUNT; i++)
    {
        bheap_insert(heap, &vals[i]);
    }
    for (int i = 0; i < COUNT; i++)
    {
        size_t val = bheap_find(heap, (void *)&i, 0);
        if (val == BH_NOTFOUND)
        {
            ft_putfmt("Error in find, value %d not found", i);
            return (1);
        }
    }
    for (int i = COUNT; i >= 0; i--)
    {
        bheap_remove(heap, i);
    }

    for (int i = 0; i < COUNT; i++)
    {
        bheap_insert(heap, &vals[i]);
    }
    for (int i = 0; i < COUNT; i++)
    {
        bheap_remove(heap, rand() % heap->size);
    }
    if (heap->size != 0)
    {
        ft_putfmt("Error heap is not empty !");
        return (1);
    }
    memalloc_destroy(allocator);
    return (0);
}
