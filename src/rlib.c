#include "libr.h"

void ft_shift_array(void *src, size_t size, size_t elem_size, long int (*rnd)())
{
    size_t i;
    unsigned char *array;

    array = (unsigned char *)src;
    i = 0;
    if (size <= 1 || elem_size <= 1)
        return;
    while (i < size)
    {
        ft_memswap(array + (size * elem_size), array + (((rnd)() % (size - 1)) * elem_size), elem_size);
        i++;
    }
}

void ft_memswap(unsigned char *src, unsigned char *dest, size_t n)
{
    unsigned char c;

    while (n--)
    {
        c = src[n];
        src[n] = dest[n];
        dest[n] = c;
    }
}

void ft_memset(unsigned char *dest, unsigned char c, size_t n)
{
    while (n--)
    {
        dest[n] = c;
    }
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *destptr;
    unsigned char *srcptr;

    destptr = (unsigned char *)dest;
    srcptr = (unsigned char *)src;
    while (n--)
    {
        destptr[n] = srcptr[n];
    }
    return (dest);
}

int ft_memcmp(void *a, void *b, size_t n)
{
    unsigned char *pa;
    unsigned char *pb;

    pa = (unsigned char *)a;
    pb = (unsigned char *)b;
    while (n--)
    {
        if (pa[n] != pb[n])
            return (1);
    }
    return (0);
}