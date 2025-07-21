/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yilgar <yilgar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 03:17:01 by yilgar            #+#    #+#             */
/*   Updated: 2024/10/19 21:11:14 by yilgar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(const char *s1, unsigned int start, size_t len)
{
	char	*s;
	size_t	i;

	if (!s1)
		return (NULL);
	if (start > ft_strlen(s1))
		return (ft_strdup(""));
	if (len > ft_strlen(s1) - start)
		len = ft_strlen(s1) - start;
	s = malloc(sizeof(char) * (len + 1));
	if (!s)
		return (NULL);
	i = 0;
	while (i < len)
	{
		s[i] = s1[start + i];
		i++;
	}
	s[i] = '\0';
	return (s);
}
