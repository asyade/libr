#include "libr.h"

void ft_shift_array(unsigned char *array, size_t size, size_t elem_size, int (*rnd)());
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
void *ft_memcpy(void *dest, const void *src, size_t n)
{
    return memcpy(dest, src, n);
}