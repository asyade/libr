/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memalloc_errno.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/01 16:44:54 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/01 17:41:15 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"
#include <signal.h>

int				errcode(int val, int set)
{
	static int	err = 0;

	if (set)
		err = val;
	return (err);
}

int				memalloc_geterr(void)
{
	return (errcode(0, 0));
}

int				memalloc_seterr(int code)
{
	return (errcode(code, 1));
}

void			memalloc_panic(int code)
{
	ft_putfmt("Memory arena corupted exit, (error: %d)", code);
	raise(5);
}
