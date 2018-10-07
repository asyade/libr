#include "libr.h"

int arrange_allocator_heap(t_memalloc *allocator, size_t buffer_size)
{
    size_t med = buffer_size / 2;
    t_bheap *new;

    new = (t_bheap *)((size_t)allocator->empty_entries + allocator->empty_entries->buffer_size + sizeof(t_bheap));
    ft_memmove(new, allocator->used_entries, allocator->used_entries->buffer_size);
    allocator->empty_entries->buffer_size = med - sizeof(t_bheap);
    allocator->used_entries->buffer_size = med - sizeof(t_bheap);
    allocator->empty_entries->capacity = allocator->empty_entries->buffer_size / sizeof(t_mementry);
    allocator->used_entries->capacity = allocator->used_entries->buffer_size / sizeof(t_mementry);
    return (0);
}

int try_expande_allocator_heap(t_memalloc *allocator)
{
    t_memchunk *new;
    t_bheap *empty;
    t_bheap *used;

    size_t replace = allocator->empty_entries->buffer_size + allocator->used_entries->buffer_size + (2 * sizeof(t_bheap));

    replace *= 2;
    if ((new = mchunk_alloc(replace)) == NULL)
        return (1);
    replace = new->size;
    empty = (t_bheap *)(new + 1);
    used = (t_bheap *)((size_t)empty + (replace / 2));
    ft_memcpy(empty, allocator->empty_entries, allocator->empty_entries->buffer_size + sizeof(t_bheap));
    ft_memcpy(used, allocator->used_entries, allocator->used_entries->buffer_size + sizeof(t_bheap));
    mchunk_free((t_memchunk *)allocator->empty_entries - 1);
    used->buffer_size = (replace / 2) - sizeof(t_bheap);
    empty->buffer_size = (replace / 2) - sizeof(t_bheap);
    used->capacity = used->buffer_size / sizeof(t_mementry);
    empty->capacity = empty->buffer_size / sizeof(t_mementry);
    allocator->empty_entries = empty;
    allocator->used_entries = used;
    return (0);
}

void *safe_memalloc_alloc(t_memalloc *allocator, size_t size, int retry)
{
    void *ptr;
    int err;

    ptr = NULL;
    if (allocator->empty_entries->capacity < allocator->empty_entries->size + 3 ||
        allocator->used_entries->capacity < allocator->used_entries->size + 2 ||
        (ptr = memalloc_alloc(allocator, size)) == NULL)
    {
        err = memalloc_geterr();
        if (err == E_INS_EMPTY || err == E_INS_USED)
        {
            memalloc_seterr(0);
            if (!retry)
                return (NULL);
            if (try_expande_allocator_heap(allocator) >= 0)
                return (safe_memalloc_alloc(allocator, size, 0));
            else
                memalloc_panic(E_EXPAND_HEAP);
        }
        else if (err != 0)
            memalloc_panic(err);
    }
    return (ptr);
}

int safe_memalloc_free(t_memalloc *allocator, void *ptr)
{
    int result;
    if ((result = memalloc_free(allocator, ptr)) < 0)
    {
        if (result == E_FIND_HEAP)
            return (1); //For safety we can throw a panic instead of returning error
        memalloc_panic(result);
    }
    return (result);
}

int safe_memalloc_expande(t_memalloc *allocator, void *ptr, size_t new_size)
{
    int result;

    if ((result = memalloc_try_expande(allocator, ptr, new_size)) < 0)
        memalloc_panic(result);
    return (result);
}