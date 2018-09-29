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

int try_expande_allocator_heap(t_memalloc *allocator, int emptyHeap)
{
    allocator++;
    if (emptyHeap)
        printf("Expanding empty heap\n");
    else
        printf("Exapnding used heap\n");
    return (-1);
}

void *safe_memalloc_alloc(t_memalloc *allocator, size_t size, int retry)
{
    void *ptr;
    int err;

    lock(LOCK_GET);
    if ((ptr = memalloc_alloc(allocator, size)) == NULL)
    {
        err = memalloc_geterr();
        if (err == E_INS_EMPTY || err == E_INS_USED)
        {
            if (!retry)
                return (NULL);
            if (try_expande_allocator_heap(allocator, err == E_INS_EMPTY) >= 0)
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