/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rlib.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:29:21 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 18:33:56 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

static void			ft_putnbrbase_fd_inner(uintmax_t nbr, char *base,
											size_t baselen, int fd)
{
	if (nbr >= baselen)
	{
		ft_putnbrbase_fd_inner(nbr / baselen, base, baselen, fd);
		ft_putnbrbase_fd_inner(nbr % baselen, base, baselen, fd);
	}
	else
		ft_putchar_buff_fd(base[nbr], fd);
}

void				ft_putnbrbase_fd(uintmax_t nbr, char *base, int fd)
{
	ft_putnbrbase_fd_inner(nbr, base, ft_strlen(base), fd);
}

int					ft_ulllen(uintmax_t nbr)
{
	int len;

	len = 1;
	while ((nbr /= 10) > 0)
		len++;
	return (len);
}

void				ft_putull_fd(uintmax_t nbr, int fd)
{
	if (nbr > 9)
		ft_putull_fd(nbr / 10, fd);
	ft_putchar_buff_fd((nbr % 10) + '0', fd);
}
