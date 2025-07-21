/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yilgar <yilgar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 12:39:37 by yilgar            #+#    #+#             */
/*   Updated: 2024/10/18 19:20:02 by yilgar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	char	*sc;
	char	*sd;

	if (dst == src && !len)
		return (NULL);
	sc = (char *)src;
	sd = (char *)dst;
	if (sc < sd)
	{
		while (len--)
			sd[len] = sc[len];
	}
	else
		ft_memcpy(sd, sc, len);
	return (dst);
}
