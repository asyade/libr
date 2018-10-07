/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_chunk.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acorbeau <acorbeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/07 14:49:24 by acorbeau          #+#    #+#             */
/*   Updated: 2018/10/07 14:51:32 by acorbeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libr.h"

t_memchunk		*mchunk_alloc(size_t size)
{
	int			page_size;
	size_t		new_size;
	t_memchunk	*chunk;

	page_size = getpagesize();
	size += sizeof(t_memchunk);
	new_size = ((size / page_size) + size % page_size) * page_size;
	chunk = mmap(NULL, new_size, MMAP_PROT, MMAP_FLAGS, 0, 0);
	if (chunk == MMAP_NULL)
		return (NULL);
	chunk->size = new_size - sizeof(t_memchunk);
	chunk->magic = MEMCHUNK_MAGIC;
	ft_memset((unsigned char *)(chunk + 1), 0, chunk->size);
	return (chunk);
}

t_memchunk		*mchunk_realloc(t_memchunk *chunk, size_t new_size)
{
	t_memchunk	*new_chunk;

	if (chunk->magic != MEMCHUNK_MAGIC)
		return (NULL);
	if (new_size < chunk->size)
		return (chunk);
	new_chunk = mchunk_alloc(new_size);
	ft_memcpy(new_chunk + 1, chunk + 1, chunk->size);
	mchunk_free(chunk);
	return (new_chunk);
}

int				mchunk_free(t_memchunk *chunk)
{
	if (chunk->magic != MEMCHUNK_MAGIC)
		return (1);
	return (munmap(chunk, chunk->size + sizeof(t_memchunk)));
}
