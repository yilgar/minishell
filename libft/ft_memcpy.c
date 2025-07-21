/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yilgar <yilgar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 12:32:56 by yilgar            #+#    #+#             */
/*   Updated: 2024/10/28 14:07:04 by yilgar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void	*dst, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*sd;
	unsigned char	*sc;

	i = 0;
	if (dst == src)
		return (dst);
	sd = (unsigned char *) dst;
	sc = (unsigned char *) src;
	while (i < n)
	{
		sd[i] = sc[i];
		i++;
	}
	return (dst);
}
