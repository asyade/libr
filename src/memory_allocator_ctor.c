#include "libr.h"

int entries_cmp(void *aa, void *bb)
{
    t_mementry *a = (t_mementry *)aa;
    t_mementry *b = (t_mementry *)bb;

    if (a->addr == b->addr)
        return (0);
    return (a->addr < b->addr ? -1 : 1);
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

void memalloc_destroy(t_memalloc *allocator)
{
    if (!allocator)
        return;
    if (mchunk_free((t_memchunk *)((size_t)allocator->emptyEntries - sizeof(t_memchunk))) != 0 ||
        mchunk_free((t_memchunk *)((size_t)allocator->usedEntries - sizeof(t_memchunk))) != 0 ||
        mchunk_free((t_memchunk *)((size_t)allocator - sizeof(t_memchunk))) != 0)
    {
        printf("Can't unmap allocator, memory criticaly corupted have to exit");
    }
}