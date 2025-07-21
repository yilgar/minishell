/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yilgar <yilgar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/25 17:50:49 by yilgar            #+#    #+#             */
/*   Updated: 2024/10/27 18:15:43 by yilgar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	controller(const char *s, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		while (s[i] == c)
			i++;
		if (s[i] && s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
	}
	return (count);
}

static char	*writer(const char *s, char c)
{
	int		i;
	char	*newstr;

	i = 0;
	while (s[i] && s[i] != c)
		i++;
	newstr = (char *)malloc(sizeof(char) * (i + 1));
	if (!newstr)
		return (NULL);
	i = 0;
	while (s[i] && s[i] != c)
	{
		newstr[i] = s[i];
		i++;
	}
	newstr[i] = '\0';
	return (newstr);
}

static void	free_split(char **split, int j)
{
	while (j >= 0)
	{
		free(split[j]);
		j--;
	}
	free(split);
}

static int	fill_split(char **write, const char *s, char c)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (s[i])
	{
		while (s[i] == c)
			i++;
		if (s[i] && s[i] != c)
		{
			write[j] = writer(&s[i], c);
			if (!write[j])
			{
				free_split(write, j - 1);
				return (0);
			}
			j++;
			while (s[i] && s[i] != c)
				i++;
		}
	}
	write[j] = NULL;
	return (1);
}

char	**ft_split(const char *s, char c)
{
	char	**write;

	if (!s)
		return (NULL);
	write = (char **)malloc(sizeof(char *) * (controller(s, c) + 1));
	if (!write)
		return (NULL);
	if (!fill_split(write, s, c))
		return (NULL);
	return (write);
}
