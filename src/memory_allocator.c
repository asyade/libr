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
            printf("invalide right magic !\n");
        }
        if (buff->status == USED)
        {
            size_t index = bheap_find(allocator->usedEntries, &(t_mementry){0, buff}, 0);
            if (index == BH_NOTFOUND)
            {
                printf("OL#JRLK#J$L$_)#($)_#($_)#($_)#($_#)($\n");
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
        printf(" %p ", buff);
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

int fill_mem_magic(t_memalloc *allocator, size_t offset, size_t size, t_alloc_stat status)
{
    if (allocator->buffer_size < offset + size)
    {
        printf("Invalide memory arena %lu - %lu for buffer size of %lu\n", offset, offset + size, allocator->buffer_size);
        return (1);
    }
    *(t_memmagic *)(ALLOC_SPTR(allocator) + offset) = (t_memmagic){status, size};
    *(t_memmagic *)((ALLOC_SPTR(allocator) + offset + size) - sizeof(t_memmagic)) = (t_memmagic){status, size};
    return (0);
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
    fill_mem_magic(alloc, 0, alloc->buffer_size, FREE);
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

void join_empty_entries(t_memalloc *allocator, size_t final, size_t drained)
{
    t_mementry drained_entry;
    t_mementry *final_entry;

    if (final == BH_NOTFOUND || drained == BH_NOTFOUND)
    {
        printf("Invalide entries for join !\n");
        return;
    }
    drained_entry = *(EMPTY_PTR(allocator) + drained);
    final_entry = (EMPTY_PTR(allocator) + final);
    print_heap(allocator->emptyEntries);
    printf("de\n");
    if (bheap_remove(allocator->emptyEntries, drained) != 0)
        printf("Can't remove drained entry !\n");

    final_entry->size += drained_entry.size;
    fill_mem_magic(allocator, (size_t)final_entry->addr - ALLOC_SPTR(allocator), final_entry->size, FREE);
}

void try_join_empty_entry_right(t_memalloc *allocator, size_t index)
{
    t_memmagic *magicptr;
    t_mementry *entry;
    t_memmagic right_magics[2];

    entry = EMPTY_PTR(allocator) + index;
    if (entry->addr == ALLOC_VPTR(allocator) ||
        ((size_t)entry->addr - ALLOC_SPTR(allocator)) + sizeof(t_memmagic) + entry->size >= allocator->buffer_size - 1)
        return;
    magicptr = (t_memmagic *)((size_t)entry->addr + entry->size);
    right_magics[0] = *magicptr;
    if (right_magics[0].status != USED && right_magics[0].status != FREE)
    {
        printf("Invalide left magic for %p !\n", magicptr);
        return;
    }
    if (right_magics[0].status == USED)
        return;
    right_magics[1] = *(t_memmagic *)(((size_t)magicptr + magicptr->size) - sizeof(t_memmagic));
    if (ft_memcmp(right_magics, right_magics + 1, sizeof(t_memmagic)) != 0)
    {
        printf("Corupted data for right magic %p\n", magicptr);
    }

    join_empty_entries(allocator, index, bheap_find(allocator->emptyEntries, &(t_mementry){0, magicptr}, 0));
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
        fill_mem_magic(allocator, ((size_t)entry.addr - ALLOC_SPTR(allocator)) + size, entry.size - size, FREE);
        //   try_join_empty_entry_right(allocator, new_index);
    }
    if ((bheap_insert(allocator->usedEntries, &(t_mementry){size, entry.addr})) == BH_NOTFOUND)
        return (NULL);
    fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), size, USED);
    return (entry.addr);
}

void *fill_entry_middel(t_memalloc *allocator, t_mementry entry, size_t size)
{
    size_t offset;
    size_t new_index;

    offset = (entry.size / 2) - (size / 2);
    if ((new_index = bheap_insert(allocator->emptyEntries, &(t_mementry){offset, entry.addr})) == BH_NOTFOUND)
        return (NULL);
    if (fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), offset, FREE) != 0)
    {
        printf("Can't place magic this is not suposed to be possible");
        return (NULL);
    }
    //  try_join_empty_entry_left(allocator, new_index);
    return fill_entry_begin(allocator, (t_mementry){entry.size - offset, (void *)((size_t)entry.addr + offset)}, size);
}

void *memalloc_alloc(t_memalloc *allocator, size_t size)
{ //Todo place rwlock here for thread safe
    //Important todo asume that the memmagic use space in size
    size_t index;
    t_mementry entry;

    if ((index = find_empty_entry(allocator->emptyEntries, size)) == BH_NOTFOUND)
        return (NULL);
    entry = ((t_mementry *)((allocator->emptyEntries) + 1))[index];
    bheap_remove(allocator->emptyEntries, index);
    if (entry.size >= size * 3)
        return (fill_entry_middel(allocator, entry, size));
    else
        return (fill_entry_begin(allocator, entry, size));
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
        return (1);
    entry = *((t_mementry *)(allocator->usedEntries + 1) + index);
    if (bheap_remove(allocator->usedEntries, index) != 0)
    {
        printf("Cant find entry to delete !\n");
        return (2);
    }
    if ((index = bheap_insert(allocator->emptyEntries, &entry)) == BH_NOTFOUND)
    {
        printf("Can't allocate new entry !\n");
        return (3);
    }
    fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), entry.size, FREE);
    try_join_empty_entry_right(allocator, index);
    try_join_empty_entry_left(allocator, index);
    return (0);
}
