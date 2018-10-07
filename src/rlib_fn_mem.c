/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rlib_fn_mem.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:29:53 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 18:30:04 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

void				ft_memswap(uint8_t *src, uint8_t *dest, size_t n)
{
	unsigned char	c;

	if (src == dest)
		return ;
	while (n--)
	{
		c = src[n];
		src[n] = dest[n];
		dest[n] = c;
	}
}

void				ft_memset(unsigned char *dest, unsigned char c, size_t n)
{
	while (n--)
	{
		dest[n] = c;
	}
}

void				*ft_memcpy(void *dest, const void *src, size_t n)
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

void				*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char	*dptr;
	unsigned char	*sptr;

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

int					ft_memcmp(void *a, void *b, size_t n)
{
	unsigned char	*pa;
	unsigned char	*pb;

	pa = (unsigned char *)a;
	pb = (unsigned char *)b;
	while (n--)
	{
		if (pa[n] != pb[n])
			return (1);
	}
	return (0);
}
