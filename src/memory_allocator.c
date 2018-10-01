#include "libr.h"

int fill_mem_magic(t_memalloc *allocator, size_t offset, size_t size, t_alloc_stat status, int check_recursive)
{
    if (allocator->buffer_size < offset + size)
    {
        ft_putfmt("Invalide memory arena %u - %u for buffer size of %u\n", offset, offset + size, allocator->buffer_size);
        return (1);
    }
    if (offset > (size_t)allocator)
        return (E_OVERFLOW);

    *(t_memmagic *)(ALLOC_SPTR(allocator) + offset) = (t_memmagic){status, size};
    *(t_memmagic *)((ALLOC_SPTR(allocator) + offset + size) - sizeof(t_memmagic)) = (t_memmagic){status, size};
    return (check_mem_magic(allocator, offset, size, check_recursive));
}

int join_empty_entries(t_memalloc *allocator, size_t final, size_t drained)
{
    t_mementry drained_entry;
    t_mementry *final_entry;

    if (final == BH_NOTFOUND || drained == BH_NOTFOUND)
        return (E_FIND_HEAP);
    drained_entry = *(EMPTY_PTR(allocator) + drained);
    final_entry = (EMPTY_PTR(allocator) + final);
    final_entry->size += drained_entry.size;
    if (fill_mem_magic(allocator, (size_t)final_entry->addr - ALLOC_SPTR(allocator), final_entry->size, FREE, 1) != 0)
        return (E_MAGIC);
    if (bheap_remove(allocator->emptyEntries, drained) != 0)
        return (E_DEL_HEAP);
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
    if (check_mem_magic(allocator, ((size_t)entry->addr + entry->size) - ALLOC_SPTR(allocator), magicptr->size, 1) != 0)
        return (E_MAGIC);
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
        ((size_t)entry->addr - ALLOC_SPTR(allocator)) + entry->size > allocator->buffer_size)
        return (E_OVERFLOW);
    if (left_magic.status == USED || (size_t)entry->addr - left_magic.size == ALLOC_SPTR(allocator) + allocator->buffer_size)
        return (0);
    magicptr = (t_memmagic *)((size_t)entry->addr - left_magic.size);
    if (check_mem_magic(allocator, (size_t)magicptr - ALLOC_SPTR(allocator), left_magic.size, 1) != 0)
        return (E_MAGIC);
    if ((final = bheap_find(allocator->emptyEntries, &(t_mementry){0, magicptr}, 0)) == BH_NOTFOUND)
    {
        ft_putfmt("Cant find empty elem\n");
        memalloc_dump(allocator);
        return (E_FIND_HEAP);
    }
    return (join_empty_entries(allocator, final, index) == 0 ? 1 : -4);
}

void *fill_entry_begin(t_memalloc *allocator, t_mementry entry, size_t size)
{
    size_t new_index;
    if (entry.size > size && entry.size - size < sizeof(t_memmagic) * 3)
        size = entry.size;
    if (entry.size > size)
    {
        if ((new_index = bheap_insert(allocator->emptyEntries, &(t_mementry){entry.size - size, (void *)((size_t)entry.addr + size)})) == BH_NOTFOUND)
        {
            memalloc_seterr(E_INS_EMPTY);
            return (NULL);
        }
        if (fill_mem_magic(allocator, ((size_t)entry.addr - ALLOC_SPTR(allocator)) + size, entry.size - size, FREE, 0) != 0)
        {
            memalloc_seterr(E_MAGIC);
            return (NULL);
        }
    }
    if ((bheap_insert(allocator->usedEntries, &(t_mementry){size, entry.addr})) == BH_NOTFOUND)
    {
        memalloc_seterr(E_INS_USED);
        return (NULL);
    }
    if (fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), size, USED, 0) != 0)
    {
        memalloc_seterr(E_MAGIC);
        return (NULL);
    }
    return (entry.addr);
}

void *fill_entry_middel(t_memalloc *allocator, t_mementry entry, size_t size)
{
    size_t offset;
    size_t new_size;
    size_t new_index;

    offset = (entry.size / 2) - (size / 2);
    new_size = entry.size - (entry.size - offset);
    if (offset % 2)
    {
        offset++;
        new_size = entry.size - (entry.size - offset);
    }
    if ((new_index = bheap_insert(allocator->emptyEntries, &(t_mementry){new_size, entry.addr})) == BH_NOTFOUND)
    {
        memalloc_seterr(E_INS_EMPTY);
        return (NULL);
    }
    if (fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), new_size, FREE, 0) != 0)
    {
        memalloc_seterr(E_MAGIC);
        return (NULL);
    }
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
        memalloc_seterr(E_DEL_HEAP);
        return (NULL);
    }
    if (entry.size >= size * 3)
        ret = (fill_entry_middel(allocator, entry, size));
    else
        ret = (fill_entry_begin(allocator, entry, size));
    return (ret ? (t_memmagic *)ret + 1 : NULL);
}

int try_join_empty_entries(t_memalloc *allocator, size_t index, void *addr)
{
    int join_result;

    // Try to join with right cell, if it's success index of curent entry may change
    join_result = try_join_empty_entry_right(allocator, index);
    // Find new index of the entry into the heap if is joined to the right cell
    if (join_result > 0 && (index = bheap_find(allocator->emptyEntries, &(t_mementry){0, addr}, 0)) == BH_NOTFOUND)
        return (E_FIND_HEAP);
    else if (join_result < 0)
        return (join_result);
    join_result = try_join_empty_entry_left(allocator, index);
    return (join_result >= 0 ? 0 : join_result);
}

int memalloc_free(t_memalloc *allocator, void *addr)
{
    size_t index;
    t_mementry entry;
    int val;

    if (addr == NULL)
        return (0);
    addr = (t_memmagic *)addr - 1;
    if ((index = bheap_find(allocator->usedEntries, &(t_mementry){0, addr}, 0)) == BH_NOTFOUND)
        return (bheap_find(allocator->emptyEntries, &(t_mementry){0, addr}, 0) == BH_NOTFOUND ? 1 : E_FIND_HEAP);
    entry = *((t_mementry *)(allocator->usedEntries + 1) + index);
    if (bheap_remove(allocator->usedEntries, index) != 0)
        return (E_DEL_HEAP);
    if ((index = bheap_insert(allocator->emptyEntries, &entry)) == BH_NOTFOUND)
        return (E_INS_EMPTY);
    if (check_mem_magic(allocator, (size_t)addr - ALLOC_SPTR(allocator), entry.size, 1) != 0)
        return (E_MAGIC);
    if (fill_mem_magic(allocator, (size_t)entry.addr - ALLOC_SPTR(allocator), entry.size, FREE, 1) != 0)
        return (E_MAGIC);
    if ((val = try_join_empty_entries(allocator, index, addr)) != 0)
        return (val);
    return (0);
}

int memalloc_expande_full(t_memalloc *allocator, t_memmagic *self, size_t new_size, size_t indexs[2])
{
    size_t joined_size;

    joined_size = new_size;
    if (fill_mem_magic(allocator, (size_t)self - ALLOC_SPTR(allocator), joined_size, USED, 1) != 0)
        return (E_MAGIC);
    (USED_PTR(allocator) + indexs[0])->size = joined_size;
    if (bheap_remove(allocator->emptyEntries, indexs[1]) != 0)
        return (E_DEL_HEAP);
    return (0);
}

int memalloc_expande_insert(t_memalloc *allocator, t_memmagic *self, t_memmagic *other, size_t indexs[2], size_t size)
{
    /*   size_t new_size;

    new_size = other->size - (size - self->size);
    if (fill_mem_magic(allocator, (size_t)self + size, new_size, FREE, 0) != 0)
        return (-1);
    if (bheap_insert(allocator->emptyEntries, &(t_mementry){new_size, (void *)((size_t)self + new_size)}) == BH_NOTFOUND)
        return (-2);

    return (memalloc_expande_full(allocator, self, other, indexs));
*/
    size++;
    return (memalloc_expande_full(allocator, self, ((size_t)other + other->size) - (size_t)self, indexs));
}

//Return 0 on success, 1 if no space is avaiable and negative number on critical error
int memalloc_try_expande(t_memalloc *allocator, void *addr, size_t new_size)
{
    t_memmagic *self;
    t_memmagic *other;
    size_t self_index;
    size_t other_index;

    if (addr == NULL || new_size == 0)
        return (new_size == 0 ? 0 : 1);
    new_size += 2 * sizeof(t_memmagic);
    self = (t_memmagic *)addr - 1;
    if (check_mem_magic_overflow(allocator, self) != 0)
        return (E_OVERFLOW);
    other = (t_memmagic *)((size_t)self + self->size);
    if (check_mem_magic_overflow(allocator, other) != 0)
        return (1);
    if (check_mem_magic(allocator, (size_t)self - ALLOC_SPTR(allocator), self->size, 1) != 0 ||
        check_mem_magic(allocator, (size_t)other - ALLOC_SPTR(allocator), other->size, 1) != 0)
        return (E_MAGIC);
    if ((self_index = bheap_find(allocator->usedEntries, &PTR_AS_ENTRY(allocator, self), 0)) == BH_NOTFOUND ||
        (other_index = bheap_find(allocator->emptyEntries, &PTR_AS_ENTRY(allocator, other), 0)) == BH_NOTFOUND)
        return (1); //Extra security can be disabel for performance
    if (self->size >= new_size)
        return (0);
    if (self->size + other->size < new_size)
        return (1);
    if (self->size + other->size > new_size * 2)
        return (memalloc_expande_insert(allocator, self, other, (size_t[]){self_index, other_index}, new_size));
    else
        return (memalloc_expande_full(allocator, self, ((size_t)other + other->size) - (size_t)self, (size_t[]){self_index, other_index}));
}