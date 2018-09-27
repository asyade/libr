#include "libr.h"
#define N 31

typedef struct s_sized_ptr
{
    void *ptr;
    size_t size;
} t_sized_ptr;

int random_range(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}

t_sized_ptr *allocate_random_sizes(t_memalloc *allocator, size_t count, size_t min, size_t max)
{
    t_sized_ptr *retval = malloc(sizeof(t_sized_ptr) * count);
    while (count--)
    {
        retval[count].size = (size_t)random_range((int)min, (int)max);
        retval[count].ptr = memalloc_alloc(allocator, retval[count].size);
        if (retval[count].ptr)
            ft_memset(retval[count].ptr, 42, retval[count].size);
    }
    return (retval);
}

int main(int ac, char **av)
{
    srand(ac > 1 ? atoi(av[1]) : 1);
    t_memalloc *allocator = memalloc_new(1024 * 1024, 1024 * 1024, 1024 * 1024);

    memalloc_dump(allocator);
    memalloc_dump(allocator);

    size_t max_ptr = 1024 * 128;
    size_t nbr_ops = 1024 * 1024;

    t_sized_ptr *ptrs = malloc(max_ptr * sizeof(t_sized_ptr));

    size_t nbr_ptr = 0;
    for (size_t i = 0; i < nbr_ops; i++)
    {
        switch (random_range(0, 4))
        {
        case 1:
            if (nbr_ptr >= max_ptr)
                break;
            ptrs[nbr_ptr].size = random_range(128, 1024);
            ptrs[nbr_ptr].ptr = memalloc_alloc(allocator, ptrs[nbr_ptr].size);
            if (ptrs[nbr_ptr].ptr)
                ft_memset(ptrs[nbr_ptr].ptr, 42, ptrs[nbr_ptr].size);

            printf(CL_RED "ALLOC_MED(%lu)=%p\n" CL_RESET, ptrs[nbr_ptr].size, ptrs[nbr_ptr].ptr);
            nbr_ptr++;
            break;
        case 2:
            if (nbr_ptr > 0)
            {
                ft_shift_array(ptrs, nbr_ptr, sizeof(t_sized_ptr), random);
                nbr_ptr--;
                if (memalloc_free(allocator, ptrs[nbr_ptr].ptr) != 0)
                {
                    printf(CL_CYAN "EMPTY ENTRIES\n" CL_RESET);
                    print_heap(allocator->emptyEntries);
                    printf(CL_RED "USED ENTRIES\n" CL_RESET);
                    print_heap(allocator->usedEntries);
                    for (size_t i = 0; i <= nbr_ptr; i++)
                    {
                        printf("ptr %p of size%lu\n", ptrs[i].ptr, ptrs[i].size);
                    }
                    printf("Invalide FREED %4.4lx!\n", ((size_t)ptrs[nbr_ptr].ptr - sizeof(t_memmagic)));
                    exit(1);
                }
                printf(CL_RED "FREE(%p)\n" CL_RESET, ptrs[nbr_ptr].ptr);
            }
            break;
        case 3:
        {
            size_t to = random_range(4, 20);
            size_t sz = (random_range(1, 4) + 1) * 16;
            for (size_t i = 0; i < to && nbr_ptr < max_ptr; i++)
            {
                ptrs[nbr_ptr].size = sz;
                ptrs[nbr_ptr].ptr = memalloc_alloc(allocator, ptrs[nbr_ptr].size);
                if (ptrs[nbr_ptr].ptr)
                    ft_memset(ptrs[nbr_ptr].ptr, 42, ptrs[nbr_ptr].size);

                printf(CL_RED "ALLOC_RANG(%lu)=%p\n" CL_RESET, ptrs[nbr_ptr].size, ptrs[nbr_ptr].ptr);
                nbr_ptr++;
            }
        }
        break;

        case 4:
        {
            size_t to = random_range(2, 20);
            ft_shift_array(ptrs, nbr_ptr, sizeof(t_sized_ptr), random);
            for (size_t i = 0; i < to && nbr_ptr > 0; i++)
            {
                nbr_ptr--;

                if (memalloc_free(allocator, ptrs[nbr_ptr].ptr) != 0)
                {
                    printf(CL_RED "Invalide freed range %p\n" CL_RESET, ptrs[nbr_ptr].ptr);
                    for (size_t i = 0; i <= nbr_ptr; i++)
                    {
                        printf("ptr %p of size%lu\n", ptrs[i].ptr, ptrs[i].size);
                    }

                    return (1);
                }
                printf(CL_RED "FREE_RANGE(%p)\n" CL_RESET, ptrs[nbr_ptr].ptr);
            }
        }
        break;
        default:
        {
            size_t to = random_range(4, 20);
            for (size_t i = 0; i < to && nbr_ptr < max_ptr; i++)
            {
                ptrs[nbr_ptr].size = random_range(12, 128);
                ptrs[nbr_ptr].ptr = memalloc_alloc(allocator, ptrs[nbr_ptr].size);
                if (ptrs[nbr_ptr].ptr)
                    ft_memset(ptrs[nbr_ptr].ptr, 42, ptrs[nbr_ptr].size);
                printf(CL_RED "ALLOC_RND_RANG(%lu)=%p\n" CL_RESET, ptrs[nbr_ptr].size, ptrs[nbr_ptr].ptr);
                nbr_ptr++;
            }
        }
        }
    }
}