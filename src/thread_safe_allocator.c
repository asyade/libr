#include "libr.h"

void lock_panic()
{
    printf("Can't take lock, exit !");
    exit(1);
}

static int lock(int op)
{
    static pthread_mutex_t mutex;
    static int mutex_initialized = 0;
    static int mutex_locked = 0;

    if (mutex_initialized == 0)
    {
        if (pthread_mutex_init(&mutex, NULL) != 0)
            lock_panic();
        mutex_initialized = 1;
    }
    if (op == LOCK_GET)
    {
        if (pthread_mutex_lock(&mutex) != 0)
            lock_panic();
        mutex_locked = LOCK_USED;
        return (LOCK_USED);
    }
    if (op == LOCK_LIBERATE)
    {
        if (pthread_mutex_unlock(&mutex) != 0)
            lock_panic();
        mutex_locked = LOCK_USABLE;
    }
    return (mutex_locked);
}

int arrange_allocator_heap(t_memalloc *allocator, size_t buffer_size)
{
    size_t med = buffer_size / 2;
    t_bheap *new;

    new = (t_bheap *)((size_t)allocator->emptyEntries + allocator->emptyEntries->buffer_size + sizeof(t_bheap));
    ft_memmove(new, allocator->usedEntries, allocator->usedEntries->buffer_size);
    allocator->emptyEntries->buffer_size = med - sizeof(t_bheap);
    allocator->usedEntries->buffer_size = med - sizeof(t_bheap);
    allocator->emptyEntries->capacity = allocator->emptyEntries->buffer_size / sizeof(t_mementry);
    allocator->usedEntries->capacity = allocator->usedEntries->buffer_size / sizeof(t_mementry);
    return (0);
}

int try_expande_allocator_heap(t_memalloc *allocator)
{
    t_memchunk *new;
    t_bheap *empty;
    t_bheap *used;

    size_t replace = allocator->emptyEntries->buffer_size + allocator->usedEntries->buffer_size + (2 * sizeof(t_bheap));

    replace *= 2;
    printf("%lu\n", replace);
    if ((new = mchunk_alloc(replace)) == NULL)
        return (1);
    replace = new->size;
    empty = (t_bheap *)(new + 1);
    used = (t_bheap *)((size_t)empty + (replace / 2));
    ft_memcpy(empty, allocator->emptyEntries, allocator->emptyEntries->buffer_size + sizeof(t_bheap));
    ft_memcpy(used, allocator->usedEntries, allocator->usedEntries->buffer_size + sizeof(t_bheap));
    mchunk_free((t_memchunk *)allocator->emptyEntries - 1);
    used->buffer_size = (replace / 2) - sizeof(t_bheap);
    empty->buffer_size = (replace / 2) - sizeof(t_bheap);
    used->capacity = used->buffer_size / sizeof(t_mementry);
    empty->capacity = empty->buffer_size / sizeof(t_mementry);
    allocator->emptyEntries = empty;
    allocator->usedEntries = used;
    return (0);
}

void *safe_memalloc_alloc(t_memalloc *allocator, size_t size, int retry)
{
    void *ptr;
    int err;

    lock(LOCK_GET);
    ptr = NULL;
    if (allocator->emptyEntries->capacity < allocator->emptyEntries->size + 3 ||
        allocator->usedEntries->capacity < allocator->usedEntries->size + 2 ||
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
    lock(LOCK_LIBERATE);
    return (ptr);
}