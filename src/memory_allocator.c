#include "libr.h"

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

int join_empty_entries(t_memalloc *allocator, size_t final, size_t drained)
{
    t_mementry drained_entry;
    t_mementry *final_entry;

    if (final == BH_NOTFOUND || drained == BH_NOTFOUND)
        return (-1);
    drained_entry = *(EMPTY_PTR(allocator) + drained);
    final_entry = (EMPTY_PTR(allocator) + final);
    final_entry->size += drained_entry.size;
    if (fill_mem_magic(allocator, ENT_OFF(allocator, (size_t)final_entry->addr), final_entry->size, FREE, 1) != 0)
        return (-2);
    if (bheap_remove(allocator->emptyEntries, drained) != 0)
        return (-3);
    return (0);
}

int try_join_empty_entry_right(t_memalloc *allocator, size_t index)
{
    t_memmagic *magicptr;
    t_mementry *entry;

    entry = EMPTY_PTR(allocator) + index;
    //TODO check overflow for addr
    magicptr = (t_memmagic *)((size_t)entry->addr + entry->size);
    if ((size_t)magicptr + sizeof(t_memmagic) > (size_t)allocator + sizeof(t_memalloc) + allocator->buffer_size)
        return (0);
    if (check_mem_magic(allocator, ENT_OFF(allocator, ((size_t)entry->addr + entry->size)), magicptr->size, 1) != 0)
        return (-2);
    if (magicptr->status == USED)
        return (0);
    return join_empty_entries(allocator, index, bheap_find(allocator->emptyEntries, &(t_mementry){0, magicptr}, 0)) == 0 ? 1 : -4;
}

int try_join_empty_entry_left(t_memalloc *allocator, size_t index)
{
    t_mementry *entry;
    t_memmagic *magicptr;
    t_memmagic left_magic;
    size_t final;

    entry = EMPTY_PTR(allocator) + index;
    if ((size_t)entry->addr == ALLOC_SPTR(allocator))
        return 0;
    left_magic = *(t_memmagic *)((size_t)entry->addr - sizeof(t_memmagic));
    if ((left_magic.status != USED && left_magic.status != FREE) ||
        (size_t)entry->addr - left_magic.size > ALLOC_SPTR(allocator) + allocator->buffer_size ||
        ENT_OFF(allocator, entry->addr) + entry->size > allocator->buffer_size)
        return (-1);
    if (left_magic.status == USED || (size_t)entry->addr - left_magic.size == ALLOC_SPTR(allocator) + allocator->buffer_size)
        return (0);
    magicptr = (t_memmagic *)((size_t)entry->addr - left_magic.size);
    if (check_mem_magic(allocator, (size_t)magicptr - ALLOC_SPTR(allocator), left_magic.size, 1) != 0)
        return (-3);
    if ((final = bheap_find(allocator->emptyEntries, &(t_mementry){0, magicptr}, 0)) == BH_NOTFOUND)
        return (-4);
    return (join_empty_entries(allocator, final, index) == 0 ? 1 : -4);
}

void *fill_entry_begin(t_memalloc *allocator, t_mementry entry, size_t size)
{
    size_t new_index;
    if (entry.size > size && entry.size - size < sizeof(t_memmagic) * 3)
        size = entry.size;
    if (entry.size > size)
    {
        if ((new_index = bheap_insert(allocator->emptyEntries, &(t_mementry){entry.size - size, (void *)((size_t)entry.addr + size)})) == BH_NOTFOUND ||
            fill_mem_magic(allocator, (ENT_OFF(allocator, entry.addr)) + size, entry.size - size, FREE, 0) != 0)
            return (NULL);
    }
    if ((bheap_insert(allocator->usedEntries, &(t_mementry){size, entry.addr})) == BH_NOTFOUND ||
        fill_mem_magic(allocator, ENT_OFF(allocator, entry.addr), size, USED, 0) != 0)
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
    if ((new_index = bheap_insert(allocator->emptyEntries, &(t_mementry){new_size, entry.addr})) == BH_NOTFOUND ||
        fill_mem_magic(allocator, ENT_OFF(allocator, entry.addr), new_size, FREE, 0) != 0)
        return (NULL);
    return fill_entry_begin(allocator, (t_mementry){entry.size - new_size, (void *)((size_t)entry.addr + new_size)}, size);
}

void *memalloc_alloc(t_memalloc *allocator, size_t size)
{ //Todo place rwlock here for thread safe
    //Important todo asume that the memmagic use space in size
    size_t index;
    t_mementry entry;
    void *ret;

    size = SIZE_ALIGN((size + (2 * sizeof(t_memmagic))));
    if ((index = find_empty_entry(allocator->emptyEntries, size)) == BH_NOTFOUND)
        return (NULL);
    entry = ((t_mementry *)((allocator->emptyEntries) + 1))[index];
    if (bheap_remove(allocator->emptyEntries, index) != 0)
    {
        memalloc_panic("\nCan't remove element from emptyHeap\n");
        return (NULL);
    }
    if (entry.size >= size * 3)
        ret = (fill_entry_middel(allocator, entry, size));
    else
        ret = (fill_entry_begin(allocator, entry, size));
    if (ret == NULL)
    {
        memalloc_panic("\nCan't allocate specified zone\n");
    }
    return ((t_memmagic *)ret + 1);
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
    return (try_join_empty_entry_left(allocator, index) >= 0 ? 0 : -3);
}

int memalloc_free(t_memalloc *allocator, void *addr)
{
    size_t index;
    t_mementry entry;

    if (addr == NULL)
        return (0);
    addr = (t_memmagic *)addr - 1;
    if ((index = bheap_find(allocator->usedEntries, &(t_mementry){0, addr}, 0)) == BH_NOTFOUND)
        return (1);
    entry = *((t_mementry *)(allocator->usedEntries + 1) + index);
    if (bheap_remove(allocator->usedEntries, index) != 0)
        memalloc_panic("\nCant find entry to delete\n");
    if ((index = bheap_insert(allocator->emptyEntries, &entry)) == BH_NOTFOUND)
        memalloc_panic("\nCan't allocate new entry\n");
    if (fill_mem_magic(allocator, ENT_OFF(allocator, entry.addr), entry.size, FREE, 1) != 0)
        memalloc_panic("\nCan't fill magic\n");
    if (try_join_empty_entries(allocator, index, addr) != 0)
        memalloc_panic("\nCan't join empty entries\n");
    return (0);
}

int memalloc_expande(t_memalloc *allocator, size_t index, size_t new_size, t_memmagic right_magic)
{
    t_mementry *entry;

    entry = USED_PTR(allocator) + index;
    if (right_magic.size < (new_size - (entry->size + (sizeof(t_memmagic) * 4) + MIN_ALLOC_SIZE)))
    {
        entry->size = entry->size + right_magic.size;
        return (fill_mem_magic(allocator, ENT_OFF(allocator, entry->addr), entry->size, USED, 1));
    }
    entry->size = new_size;
    return (0);
}
//Return 0 on success, 1 if no space is avaiable and negative number on critical error
int memalloc_try_expande(t_memalloc *allocator, void *addr, size_t new_size)
{
    size_t index;
    size_t right_index;
    t_memmagic *magic;
    t_memmagic *right_magic;

    new_size = SIZE_ALIGN(new_size);
    if (addr == NULL || (size_t)addr + sizeof(t_memmagic) > ALLOC_SPTR(allocator) + allocator->buffer_size)
        return (-1);
    magic = (t_memmagic *)addr - 1;
    if ((index = bheap_find(allocator->usedEntries, &(t_mementry){0, magic}, 0)) == BH_NOTFOUND)
        return (-2);
    if (check_mem_magic(allocator, (size_t)magic - ALLOC_SPTR(allocator), magic->size, 1) != 0)
        return (-3);
    if (magic->size - (sizeof(t_memmagic) * 2) >= new_size)
        return (0);
    right_magic = (t_memmagic *)((size_t)magic + magic->size);
    if ((size_t)right_magic >= ALLOC_SPTR(allocator) + allocator->buffer_size ||
        right_magic->status == USED ||
        (magic->size + right_magic->size) - sizeof(t_memmagic) * 2 < new_size)
        return (1);
    if ((right_index = bheap_find(allocator->emptyEntries, &(t_mementry){0, right_magic}, 0)) == BH_NOTFOUND)
        return (-4);
    if (check_mem_magic(allocator, (size_t)right_magic - ALLOC_SPTR(allocator), (EMPTY_PTR(allocator) + right_index)->size, 1) != 0)
        return (-3);
    if (bheap_remove(allocator->emptyEntries, right_index) != 0)
        return (-5);
    //Now we have the entry we want expande and the magic of the entry we gonna drain for expand we also know that the new entry will be bigger or equals than new_size
    return (memalloc_expande(allocator, index, new_size, *right_magic));
}