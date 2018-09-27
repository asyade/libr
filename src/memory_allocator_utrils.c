#include "libr.h"
void print_heap(t_bheap *heap)
{
    t_mementry *ptr = (t_mementry *)(heap + 1);
    size_t i = 0;
    size_t x = 1;
    while (i < heap->size)
    {
        size_t z = 0;
        while (z < x && z + i < heap->size)
        {
            printf("%4.4lx ", (size_t)ptr[z++ + i].addr);
        }
        i += x;
        x *= 2;
        printf("\n");
    }
    printf("\n");
}

size_t get_entries_size_sum(t_mementry *entries, size_t size)
{
    size_t ret;

    ret = 0;
    while (size--)
    {
        ret += entries[size].size;
    }
    return (ret);
}

void memalloc_dump_area(size_t size, char c, size_t divid)
{
    size_t nbr = size / divid;
    if (c == '#' && nbr > 0)
        nbr -= 1;
    printf("|");
    if (nbr == 0)
        nbr++;
    while (nbr--)
        printf("%c", c);
}

#define NBR_COL (148)
void memalloc_dump(t_memalloc *allocator)
{
    size_t used = 0;
    size_t empty = 0;

    size_t divider = allocator->buffer_size / NBR_COL;
    printf(CL_GREEN "Size : %lu Free cells : %lu Used cells : %lu\n" CL_RESET, allocator->buffer_size, allocator->emptyEntries->size, allocator->usedEntries->size);
    printf("Allocated size %lu space renaming %lu\n", get_entries_size_sum(USED_PTR(allocator), allocator->usedEntries->size), get_entries_size_sum(EMPTY_PTR(allocator), allocator->emptyEntries->size));
    t_memmagic *buff = (t_memmagic *)(allocator + 1);
    while (1)
    {
        t_memmagic *other = (t_memmagic *)(((size_t)buff + buff->size) - sizeof(t_memmagic));
        if (ft_memcmp(other, buff, sizeof(t_memmagic)) != 0)
        {
            printf("invalide right magic, memory arena corupted ! index %.4lx of %.4lx\n", (size_t)buff + buff->size, ALLOC_SPTR(allocator) + allocator->buffer_size);
            return;
        }
        if (buff->status == USED)
        {
            size_t index = bheap_find(allocator->usedEntries, &(t_mementry){0, buff}, 0);
            if (index == BH_NOTFOUND)
            {
                printf("Can't find memory entry, memory arena corupted !\n");
                return;
            }
            memalloc_dump_area(buff->size, '#', divider);
            used++;
        }
        else
        {
            size_t index = bheap_find(allocator->emptyEntries, &(t_mementry){0, buff}, 0);
            if (index == BH_NOTFOUND)
            {
                printf("OL#JRLK#J$L$_)#($)_#($_)#($_)#($_#)($\n");
            }
            memalloc_dump_area(buff->size, '-', divider);
            empty++;
        }
        //printf(" %p ", buff);
        if (((size_t)buff - ALLOC_SPTR(allocator)) + (sizeof(t_memmagic) * 3) + buff->size < allocator->buffer_size && buff->size > sizeof(t_memmagic) * 3)
            buff = (t_memmagic *)((size_t)buff + buff->size);
        else
            break;
        //if (allocator->emptyEntries->size != empty)
        //    printf("Empty allocations database corupted %ld vs $ld\n", allocator->emptyEntries->size, empty);
        //if (allocator->usedEntries->size != used)
        //    printf("Empty allocations database corupted %ld vs $ld\n", allocator->usedEntries->size, used);
    }
    printf("|\n\n");
}

// Check mem magic at offset and asure it's long of size
// if recursive == 1 check also right and left entry if it's in allocator range
int check_mem_magic(t_memalloc *allocator, size_t offset, size_t size, int recursive)
{
    t_memmagic magics[2];
    if (size == 0 || size + offset > allocator->buffer_size || offset > MAX_ALLOC_SIZE * 128 || size > MAX_ALLOC_SIZE)
        return (-1);
    magics[0] = *(t_memmagic *)(ALLOC_SPTR(allocator) + offset);
    magics[1] = *(t_memmagic *)(ALLOC_SPTR(allocator) + offset + (size - sizeof(t_memmagic)));
    if (ft_memcmp(magics, magics + 1, sizeof(t_memmagic)) != 0 || magics[0].size != size || !(magics[0].status & (USED | FREE)))
    {
        printf("Magics do not match at %lu %lu (recursive : %s)\n", offset, size, recursive ? "true" : "false");
        return (-2);
    }
    if (!recursive)
        return (0);
    if (offset > 0)
    {
        magics[0] = *(t_memmagic *)(ALLOC_SPTR(allocator) + (offset - sizeof(t_memmagic)));
        if (check_mem_magic(allocator, offset - magics[0].size, magics[0].size, 0) != 0)
            return (-3);
    }
    if (offset + size < allocator->buffer_size)
    {
        magics[0] = *(t_memmagic *)(ALLOC_SPTR(allocator) + offset + size);
        if (check_mem_magic(allocator, offset + size, magics[0].size, 0) != 0)
            return (-4);
    }
    return (0);
}