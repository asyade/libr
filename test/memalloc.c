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

void random_test()
{
    t_memalloc *allocator = memalloc_new(1024 * 1024, 1024 * 1024, 1024 * 1024);

    size_t max_ptr = 10000;
    size_t nbr_ops = 1024 * 1024;

    t_sized_ptr *ptrs = malloc(max_ptr * sizeof(t_sized_ptr));

    size_t nbr_ptr = 0;
    for (size_t i = 0; i < nbr_ops; i++)
    {
        switch (random_range(0, 9))
        {
        // Free
        case 2:
            if (nbr_ptr > 0)
            {
                if (random() % 2 && random() % 2)
                {
                    printf(CL_GREEN "SHIFTING PTRS\n" CL_RESET);
                    ft_shift_array(ptrs, nbr_ptr, sizeof(t_sized_ptr), random);
                }
                nbr_ptr--;
                int freeres;
                if ((freeres = memalloc_free(allocator, ptrs[nbr_ptr].ptr)) != 0)
                {
                    printf(CL_CYAN "EMPTY ENTRIES\n" CL_RESET);
                    print_heap(allocator->emptyEntries);
                    printf(CL_RED "USED ENTRIES\n" CL_RESET);
                    print_heap(allocator->usedEntries);
                    printf("Invalide FREED %4.4lx! %d\n", ((size_t)ptrs[nbr_ptr].ptr - sizeof(t_memmagic)), freeres);
                    return;
                }

                printf(CL_RED "FREE(%p)\n" CL_RESET, ptrs[nbr_ptr].ptr);
            }
            break;
        case 5:
        {
            void *addr = ptrs[nbr_ptr - 1].ptr + random_range(1, 128);
            if (memalloc_free(allocator, addr) == 0)
            {
                for (size_t i = 0; i < nbr_ptr; i++)
                {
                    if (ptrs[i].ptr == addr)
                    {
                        printf(CL_YELLOW "FALSE FREE SUCCESS !\n" CL_RESET);
                        ptrs[i] = ptrs[--nbr_ptr];
                        break;
                    }
                }
                if (i == nbr_ptr)
                {
                    printf("Can't find false free success addr in addr database, strange !\n");
                    return;
                }
            }
            printf(CL_MAGENTA "FALSE FREE\n" CL_RESET);
        }
        break;
        case 4:
        {
            size_t to = random_range(2, 20);
            if (random() % 2 && random() % 2)
            {
                printf(CL_GREEN "SHIFTING PTRS\n" CL_RESET);
                ft_shift_array(ptrs, nbr_ptr, sizeof(t_sized_ptr), random);
                break;
            }
            for (size_t i = 0; i < to && nbr_ptr > 0; i++)
            {
                nbr_ptr--;
                if (memalloc_free(allocator, ptrs[nbr_ptr].ptr) != 0)
                    printf(CL_RED "Invalide freed range %p\n" CL_RESET, ptrs[nbr_ptr].ptr);
                printf(CL_YELLOW "FREE_RANGE(%p)\n" CL_RESET, ptrs[nbr_ptr].ptr);
            }
        }
        break;
        ///Realloc
        case 6:
        case 7:
        case 8:
        case 9:
            if (nbr_ptr <= 0)
                break;
            int fail_reason;
            // if (random() % 2 && random() % 2)
            // {
            // printf(CL_GREEN "SHIFTING PTRS\n" CL_RESET);
            // ft_shift_array(ptrs, nbr_ptr, sizeof(t_sized_ptr), random);
            // }
            size_t new_size = ptrs[0].size * random_range(0, 4);
            size_t old_size = ptrs[0].size;
            if ((fail_reason = memalloc_try_expande(allocator, ptrs[0].ptr, new_size)) == 0)
            {
                ptrs[0].size = new_size;
                size_t real_size = ((t_memmagic *)ptrs[0].ptr - 1)->size - (sizeof(t_memmagic) * 2);
                ft_memset(ptrs[0].ptr, 43, ptrs[0].size);
                printf(CL_CYAN "EXAPND(%p, %lu-%lu, real %lu)" CL_GREEN " SUCCESS\n" CL_RESET, ptrs[0].ptr, old_size, ptrs[0].size, real_size);
            }
            else if (fail_reason == 1)
                printf(CL_CYAN "EXAPND(%p, %lu-%lu)" CL_YELLOW " NO SPACE LEFT\n" CL_RESET, ptrs[0].ptr, old_size, new_size);
            else
            {
                printf(CL_CYAN "EXAPND(%p, %lu-%lu)" CL_RED " ERROR %d\n" CL_RESET, ptrs[0].ptr, old_size, new_size, fail_reason);

                return;
            }
            break;
        //Alloc
        case 1:
            if (nbr_ptr >= max_ptr)
                break;
            ptrs[nbr_ptr].size = random_range(128, 1024 * 512);
            ptrs[nbr_ptr].ptr = memalloc_alloc(allocator, ptrs[nbr_ptr].size);
            if (!ptrs[nbr_ptr].ptr)
                break;
            ft_memset(ptrs[nbr_ptr].ptr, 42, ((t_memmagic *)ptrs[nbr_ptr].ptr - 1)->size - sizeof(t_memmagic) * 2);

            printf(CL_RED "ALLOC_MED(%lu, real %lu)=%p\n" CL_RESET, ptrs[nbr_ptr].size, ((t_memmagic *)ptrs[nbr_ptr].ptr - 1)->size, ptrs[nbr_ptr].ptr);
            nbr_ptr++;
            break;
        case 3:
        {
            size_t to = random_range(4, 20);
            size_t sz = (random_range(1, 4) + 1) * 16;
            for (size_t i = 0; i < to && nbr_ptr < max_ptr; i++)
            {
                ptrs[nbr_ptr].size = sz;
                ptrs[nbr_ptr].ptr = memalloc_alloc(allocator, ptrs[nbr_ptr].size);
                if (!ptrs[nbr_ptr].ptr)
                    continue;
                ft_memset(ptrs[nbr_ptr].ptr, 42, ((t_memmagic *)ptrs[nbr_ptr].ptr - 1)->size - sizeof(t_memmagic) * 2);

                printf(CL_GREEN "ALLOC_RANG(%lu, real=%lu)=%p\n" CL_RESET, ptrs[nbr_ptr].size, ((t_memmagic *)ptrs[nbr_ptr].ptr - 1)->size, ptrs[nbr_ptr].ptr);
                nbr_ptr++;
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
                if (!ptrs[nbr_ptr].ptr)
                    continue;
                ft_memset(ptrs[nbr_ptr].ptr, 42, ((t_memmagic *)ptrs[nbr_ptr].ptr - 1)->size - sizeof(t_memmagic) * 2);
                printf(CL_BLUE "ALLOC_RND_RANG(%lu, real=%lu)=%p\n" CL_RESET, ptrs[nbr_ptr].size, ((t_memmagic *)ptrs[nbr_ptr].ptr - 1)->size, ptrs[nbr_ptr].ptr);
                nbr_ptr++;
            }
        }
        }
    }
    printf("# DONE #\n");
}

int main(int ac, char **av)
{
    srand(ac > 1 ? atoi(av[1]) : 1);
    random_test();
}