/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rlib_fn_str3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:33:46 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 18:33:52 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

void				ft_shift_array(void *s, size_t size, size_t es, t_rnd rnd)
{
	size_t			i;
	unsigned char	*array;
	size_t			x;

	array = (unsigned char *)s;
	i = 0;
	if (size <= 1 || es <= 1)
		return ;
	while (i < size)
	{
		x = (rnd)();
		x = x % size;
		ft_memswap(array + (i * es), array + (x * es), es);
		i++;
	}
}

size_t				ft_strlen(char *str)
{
	size_t			len;

	len = 0;
	while (*str++ != '\0')
	{
		len++;
	}
	return (len);
}
