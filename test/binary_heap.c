#include "libr.h"
#include <stdlib.h>
int cmpf(void *pa, void *pb)
{
    int diff;

    diff = *(int *)pa - *(int *)pb;
    if (diff == 0)
        return (0);
    else
        return (diff) > 0 ? 1 : -1;
}

int check_heap_at(t_bheap *heap, size_t index, size_t new_index)
{
    if (new_index >= heap->size || index >= heap->size)
        return (1);
    if (cmpf(bh_index(heap, index), bh_index(heap, new_index)) > 0)
        return (0);
    return (check_heap(heap, new_index));
}
int check_heap(t_bheap *heap, int index)
{
    if (!check_heap_at(heap, index, BH_LEFT(index)))
        return (0);
    if (!check_heap_at(heap, index, BH_RIGHT(index)))
        return (0);
    return (1);
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
            printf("%d ", ptr[z++ + i]);
        }
        i += x;
        x *= 2;
        printf("\n");
    }
    printf("\n");
}

#define COUNT 1024
int main(int ac, char **av)
{
    srand((ac > 1) ? atoi(av[1]) : 42);
    //t_bheap *heap = bheap_new(sizeof(int), COUNT, cmpf);
    int vals[COUNT];
    for (int i = 0; i < COUNT; i++)
    {
        vals[i] = i;
    }
    //for (int i = 0; i < COUNT; i++)
    //{
    //    int npos = rand() % COUNT;
    //    int tmp = vals[npos];
    //    vals[npos] = vals[i];
    //    vals[i] = tmp;
    //}
    ft_shift_array((unsigned char *)vals, COUNT, sizeof(*vals), rand);
    for (int i = 0; i < COUNT; i++)
    {
        printf("%d, ", vals[i]);
        //bheap_insert(heap, &vals[i]);
        // if (!check_heap(heap, 0))
        // {
        // printf("Error\n");
        // return (1);
        // }
    }
}
