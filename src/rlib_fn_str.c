/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rlib_fn_str.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 18:30:25 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 18:31:12 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

void				ft_putstr_fd(char *str, int fd)
{
	write(fd, str, strlen(str));
}

void				ft_pustr(char *str)
{
	ft_putstr_fd(str, 1);
}

void				ft_putchar_fd(char c, int fd)
{
	write(1, &c, fd);
}

void				ft_putendl_fd(char *str, int fd)
{
	ft_putstr_fd(str, fd);
	ft_putchar_fd('\n', fd);
}

void				ft_putendl(char *str)
{
	ft_putendl_fd(str, 1);
}
