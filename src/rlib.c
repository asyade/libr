#include "libr.h"

void ft_shift_array(void *src, size_t size, size_t elem_size, long int (*rnd)())
{
    size_t i;
    unsigned char *array;
    size_t x;

    array = (unsigned char *)src;
    i = 0;
    if (size <= 1 || elem_size <= 1)
        return;
    while (i < size)
    {
        x = (rnd)();
        x = x % size;
        ft_memswap(array + (i * elem_size), array + (x * elem_size), elem_size);
        i++;
    }
}

void ft_memswap(unsigned char *src, unsigned char *dest, size_t n)
{
    unsigned char c;

    if (src == dest)
        return;
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

void *ft_memmove(void *dest, const void *src, size_t n)
{
    unsigned char *dptr;
    unsigned char *sptr;

    dptr = (unsigned char *)dest;
    sptr = (unsigned char *)src;
    if (dptr == sptr)
        return (dptr);
    if (dptr > sptr)
    {
        while (n--)
        {
            dptr[n] = sptr[n];
        }
    }
    else
        ft_memcpy(dest, src, n);
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

size_t ft_strlen(char *str)
{
    size_t len;

    len = 0;
    while (*str++ != '\0')
    {
        len++;
    }
    return (len);
}

void ft_putstr_fd(char *str, int fd)
{
    write(fd, str, strlen(str));
}

void ft_pustr(char *str)
{
    ft_putstr_fd(str, 1);
}

void ft_putchar_fd(char c, int fd)
{
    write(1, &c, fd);
}

void ft_putendl_fd(char *str, int fd)
{
    ft_putstr_fd(str, fd);
    ft_putchar_fd('\n', fd);
}

void ft_putendl(char *str)
{
    ft_putendl_fd(str, 1);
}

static void ft_putnbrbase_fd_inner(uintmax_t nbr, char *base, size_t baselen,
                                   int fd)
{
    if (nbr >= baselen)
    {
        ft_putnbrbase_fd_inner(nbr / baselen, base, baselen, fd);
        ft_putnbrbase_fd_inner(nbr % baselen, base, baselen, fd);
    }
    else
        ft_putchar_buff_fd(base[nbr], fd);
}

void ft_putnbrbase_fd(uintmax_t nbr, char *base, int fd)
{
    ft_putnbrbase_fd_inner(nbr, base, ft_strlen(base), fd);
}

int ft_ulllen(uintmax_t nbr)
{
    int len;

    len = 1;
    while ((nbr /= 10) > 0)
        len++;
    return (len);
}

void ft_putull_fd(uintmax_t nbr, int fd)
{
    if (nbr > 9)
        ft_putull_fd(nbr / 10, fd);
    ft_putchar_buff_fd((nbr % 10) + '0', fd);
}

void ft_putnbr_fd(int n, int fd)
{
    unsigned int nb;

    if (n < 0)
        ft_putchar_buff_fd('-', fd);
    nb = ((n > 0) ? n : -n);
    if (nb >= 10)
        ft_putnbr_fd(nb / 10, fd);
    ft_putchar_buff_fd((nb % 10) + '0', fd);
}

int ft_strncmp(const char *a, const char *b, size_t n)
{
    char *ptra;
    char *ptrb;

    ptra = (char *)a;
    ptrb = (char *)b;
    if (!ptra && !ptrb)
        return (0);
    while (*ptra && *ptrb && *ptra == *ptrb && n--)
    {
        ptra++;
        ptrb++;
    }
    return (n == 0) ? 0 : ((unsigned char)*ptra - (unsigned char)*ptrb);
}

#define PUTFMT_BUFFER_SIZE 1024 * 16
void ft_putchar_buff_fd(char c, int fd)
{
    static size_t index = 0;
    static char buffer[PUTFMT_BUFFER_SIZE] = {0};

    buffer[index++] = c;
    if (c == '\n' || index == PUTFMT_BUFFER_SIZE - 1)
    {
        write(fd, buffer, index);
        index = 0;
    }
}

#define FMT_BUFFER_SIZE 4096
void ft_putfmt(char *fmt, ...)
{
    va_list args;
    size_t i = 0;
    size_t len = ft_strlen(fmt);

    va_start(args, fmt);
    while (i < len)
    {
        if (fmt[i] == '%')
        {
            if (ft_strncmp(fmt + i + 1, "u", 1) == 0)
            {
                ft_putull_fd(va_arg(args, uintmax_t), 1);
                i += 2;
            }
            else if (fmt[i + 1] == 'x' || fmt[i + 1] == 'p')
            {
                ft_putnbrbase_fd(va_arg(args, uintmax_t), "0123456789ABCDEF", 1);
                i += 2;
            }
            else if (fmt[i + 1] == 'd')
            {
                ft_putnbr_fd(va_arg(args, int), 1);
                i += 2;
            }
            else
                ft_putchar_fd(fmt[i], 1);
        }
        ft_putchar_buff_fd(fmt[i], 1);
        i++;
    }
}