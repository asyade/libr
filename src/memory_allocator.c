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

void memalloc_dump_area(size_t size, char c, size_t divid)
{
    size_t nbr = size / divid;
    printf("|");
    if (nbr == 0)
        nbr++;
    while (nbr--)
        printf("%c", c);
}

#define NBR_COL (128)
void memalloc_dump(t_memalloc *allocator)
{
    size_t used = 0;
    size_t empty = 0;

    size_t divider = allocator->buffer_size / NBR_COL;
    printf("Size : %lu Free cells : %lu Used cells : %lu\n", allocator->buffer_size, allocator->emptyEntries->size, allocator->usedEntries->size);
    t_memmagic *buff = (t_memmagic *)(allocator + 1);
    while (1)
    {
        t_memmagic *other = (t_memmagic *)(((size_t)buff + buff->size) - sizeof(t_memmagic));
        if (ft_memcmp(other, buff, sizeof(t_memmagic)) != 0)
        {
            printf("invalide right magic, mory arena corupted !\n");
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
    printf("|\n");
}

int entries_cmp(void *aa, void *bb)
{
    t_mementry *a = (t_mementry *)aa;
    t_mementry *b = (t_mementry *)bb;

    if (a->addr == b->addr)
        return (0);
    return (a->addr < b->addr ? -1 : 1);
}

// Check mem magic at offset and asure it's long of size
// if recursive == 1 check also right and left entry if it's in allocator range
int check_mem_magic(t_memalloc *allocator, size_t offset, size_t size, int recursive)
{
    t_memmagic magics[2];
    if (size == 0)
        return (-6);
    //Double check to avoid overflow
    if (size + offset > allocator->buffer_size || offset > MAX_ALLOC_SIZE * 128 || size > MAX_ALLOC_SIZE)
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
            return (-3);
    }
    return (0);
}

int fill_mem_magic(t_memalloc *allocator, size_t offset, size_t size, t_alloc_stat status, int check_recursive)
{
    if (allocator->buffer_size < offset + size)
    {
        printf("Invalide memory arena %lu - %lu for buffer size of %lu\n", offset, offset + size, allocator->buffer_size);
        return (1);
    }

    *(t_memmagic *)(ALLOC_SPTR(allocator) + offset) = (t_memmagic){status, size};
    *(t_memmagic *)((ALLOC_SPTR(allocator) + offset + size) - sizeof(t_memmagic)) = (t_memmagic){status, size};
    return (check_mem_magic(allocator, offset, size, check_recursive));
}

t_memalloc *memalloc_new(size_t buffer_size, size_t emptyHeapSize, size_t usedHeapSize)
{
    t_memalloc *alloc;
    t_memchunk *chunk;

    if ((chunk = mchunk_alloc(buffer_size)) == NULL)
        return (NULL);
    alloc = (t_memalloc *)(chunk + 1);
    alloc->buffer_size = chunk->size - sizeof(t_memalloc);
    if ((chunk = mchunk_alloc(emptyHeapSize)) == NULL)
    {
        mchunk_free((t_memchunk *)(alloc - 1) - 1);
        return (NULL);
    }
    alloc->emptyEntries = bheap_new(chunk + 1, chunk->size, sizeof(t_mementry), entries_cmp);
    if ((chunk = mchunk_alloc(usedHeapSize)) == NULL)
    {
        mchunk_free((t_memchunk *)(alloc - 1) - 1);
        mchunk_free((t_memchunk *)(alloc->emptyEntries - 1) - 1);
        return (NULL);
    }
    alloc->usedEntries = bheap_new(chunk + 1, chunk->size, sizeof(t_mementry), entries_cmp);

    bheap_insert(alloc->emptyEntries, &(t_mementry){alloc->buffer_size, alloc + 1});
    fill_mem_magic(alloc, 0, alloc->buffer_size, FREE, 1);
    return (alloc);
}

size_t find_empty_entry(t_bheap *heap, size_t size)
{
    size_t i;
    size_t choice;
    t_mementry *entries;

    choice = BH_NOTFOUND;
    entries = (t_mementry *)(heap + 1);
    i = 0;
    while (i < heap->size)
    {
        if (entries[i].size >= size * 3)
        {
            choice = i;
            break;
        }
        else if (entries[i].size >= size)
            choice = i;
        i++;
    }
    return (choice);
}

int join_empty_entries(t_memalloc *allocator, size_t final, size_t drained)
{
    t_mementry drained_entry;
    t_mementry *final_entry;

    if (final == BH_NOTFOUND || drained == BH_NOTFOUND)
    {
        printf("Invalide entries for join !\n");
        return (-1);
    }
    drained_entry = *(EMPTY_PTR(allocator) + drained);
    final_entry = (EMPTY_PTR(allocator) + final);
    printf("-- BEFORE DRAIN --\n");
    printf("Drained %lu into %lu\n", drained, final);
    print_heap(allocator->emptyEntries);
    if (bheap_remove(allocator->emptyEntries, drained) != 0)
    {
        printf("Can't remove drained entry !\n");
        return (-2);
    }
    printf("-- AFTER DRAIN --\n");
    print_heap(allocator->emptyEntries);
    final_entry->size += drained_entry.size;
    return (fill_mem_magic(allocator, (size_t)final_entry->addr - ALLOC_SPTR(allocator), final_entry->size, FREE, 1));
}

int try_join_empty_entry_right(t_memalloc *allocator, size_t index)
{
    t_memmagic *magicptr;
    t_mementry *entry;
    t_memmagic right_magics[2];

    entry = EMPTY_PTR(allocator) + index;
    if (entry->addr == ALLOC_VPTR(allocator) ||
        ((size_t)entry->addr - ALLOC_SPTR(allocator)) + sizeof(t_memmagic) + entry->size >= allocator->buffer_size - 1)
        return -1;
    magicptr = (t_memmagic *)((size_t)entry->addr + entry->size);
    right_magics[0] = *magicptr;
    if (right_magics[0].status != USED && right_magics[0].status != FREE)
    {
        printf("Invalide left magic for %p !\n", magicptr);
        return (-2);
    }
    if (right_magics[0].status == USED)
        return (0);
    right_magics[1] = *(t_memmagic *)(((size_t)magicptr + magicptr->size) - sizeof(t_memmagic));
    if (ft_memcmp(right_magics, right_magics + 1, sizeof(t_memmagic)) != 0)
    {
        printf("Corupted data for right magic %p\n", magicptr);
        return (-3);
    }
    return join_empty_entries(allocator, index, bheap_find(allocator->emptyEntries, &(t_mementry){0, magicptr}, 0)) == 0 ? 1 : -4;
}

void try_join_empty_entry_left(t_memalloc *allocator, size_t index)
{
    t_mementry *entry;
    t_memmagic *magicptr;
    t_memmagic left_magic;
    size_t final;

    entry = EMPTY_PTR(allocator) + index;
    if ((size_t)entry->addr == ALLOC_SPTR(allocator))
        return;

    left_magic = *(t_memmagic *)((size_t)entry->addr - sizeof(t_memmagic));
    if (left_magic.status == USED ||
        (left_magic.status != USED && left_magic.status != FREE) ||
        (size_t)entry->addr - left_magic.size >= ALLOC_SPTR(allocator) + allocator->buffer_size ||
        ((size_t)entry->addr - ALLOC_SPTR(allocator)) + entry->size > allocator->buffer_size - 1)
        return;
    magicptr = (t_memmagic *)((size_t)entry->addr - left_magic.size);
    if (ft_memcmp(&left_magic, magicptr, sizeof(t_memmagic)) != 0)
    {
        printf("Corupted left magic for %p\n", entry->addr);
        return;
    }
    if ((final = bheap_find(allocator->emptyEntries, &(t_mementry){0, magicptr}, 0)) == BH_NOTFOUND)
    {
        printf("Can't find presumed entry !\n");
        return;
    }
    join_empty_entries(allocator, final, index);
}

void *fill_entry_begin(t_memalloc *allocator, t_mementry entry, size_t size)
{
    size_t new_index;
    if (entry.size > size && entry.size - size < sizeof(t_memmagic) * 3)
        size = entry.size;
    if (entry.size > size)
    {
        if ((new_index = bheap_insert(allocator->emptyEntries, &(t_mementry){entry.size - size, (void *)((size_t)entry.addr + size)})) == BH_NOTFOUND)
            return (NULL);
        if (fill_mem_magic(allocator, ((size_t)entry.addr - ALLOC_SPTR(allocator)) + size, entry.size - size, FREE, 0) != 0)
            return (NULL);
    }
    if ((bheap_insert(allocator->usedEntries, &(t_mementry){size, entry.addr})) == BH_NOTFOUND)
        return (NULL);
    if (fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), size, USED, 0) != 0)
        return (NULL);
    return (entry.addr);
}

void *fill_entry_middel(t_memalloc *allocator, t_mementry entry, size_t size)
{
    size_t offset;
    size_t new_size;
    size_t new_index;

    offset = (entry.size / 2) - (size / 2);
    new_size = entry.size - (entry.size - offset);
    if ((new_index = bheap_insert(allocator->emptyEntries, &(t_mementry){new_size, entry.addr})) == BH_NOTFOUND)
        return (NULL);
    if (fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), new_size, FREE, 0) != 0)
        return (NULL);
    //  try_join_empty_entry_left(allocator, new_index);
    return fill_entry_begin(allocator, (t_mementry){entry.size - new_size, (void *)((size_t)entry.addr + new_size)}, size);
}

void memalloc_panic(const char *message)
{
    printf(message);
}

void *memalloc_alloc(t_memalloc *allocator, size_t size)
{ //Todo place rwlock here for thread safe
    //Important todo asume that the memmagic use space in size
    size_t index;
    t_mementry entry;
    void *ret;

    if ((index = find_empty_entry(allocator->emptyEntries, size)) == BH_NOTFOUND)
        return (NULL);
    entry = ((t_mementry *)((allocator->emptyEntries) + 1))[index];
    bheap_remove(allocator->emptyEntries, index);
    if (entry.size >= size * 3)
        ret = (fill_entry_middel(allocator, entry, size));
    else
        ret = (fill_entry_begin(allocator, entry, size));
    if (ret == NULL)
        memalloc_panic("@@@@@ ----- MEMORY ARENA CORUPTED !!!\n");
    return (ret);
}

int try_join_empty_entries(t_memalloc *allocator, size_t index, void *addr)
{
    int join_result;

    // Try to join with right cell, if it's success index of curent entry may change
    join_result = try_join_empty_entry_right(allocator, index);
    // Find new index of the entry into the heap if is joined to the right cell
    if (join_result > 0 && (index = bheap_find(allocator->emptyEntries, &(t_mementry){0, addr}, 0)) == BH_NOTFOUND)
        return (-2);
    else if (join_result < 0)
        return (-1);
    try_join_empty_entry_left(allocator, index);
    return (0);
}

int memalloc_free(t_memalloc *allocator, void *addr)
{
    if (allocator->emptyEntries->size == 7 && allocator->usedEntries->size == 6)
    {
        printf("Here shit appen\n");
    }

    printf("%p freed\n", addr);
    size_t index;
    t_mementry entry;

    if ((index = bheap_find(allocator->usedEntries, &(t_mementry){0, addr}, 0)) == BH_NOTFOUND)
        return (-1);
    entry = *((t_mementry *)(allocator->usedEntries + 1) + index);
    if (bheap_remove(allocator->usedEntries, index) != 0)
    {
        memalloc_panic("@@@@@ ----- MEMORY ARENA CORUPTED !!!\nCant find entry to delete\n");
        return (-2);
    }
    if ((index = bheap_insert(allocator->emptyEntries, &entry)) == BH_NOTFOUND)
    {
        memalloc_panic("@@@@@ ----- MEMORY ARENA CORUPTED !!!\nCan't allocate new entry\n");
        return (-3);
    }
    if (fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), entry.size, FREE, 1) != 0)
    {
        memalloc_panic("@@@@@ ----- MEMORY ARENA CORUPTED !!!\nCan't fill magic\n");

        return (-4);
    }
    try_join_empty_entries(allocator, index, addr);
    return (0);
}
