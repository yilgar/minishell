/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yilgar <yilgar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 12:49:59 by yilgar            #+#    #+#             */
/*   Updated: 2024/10/24 17:26:02 by yilgar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	itoalen(int n)
{
	int	len;

	len = 1;
	if (n < 0)
		len++;
	while (n / 10 != 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	int		i;
	char	*s;
	long	nb;

	nb = n;
	i = itoalen(n);
	s = (char *)malloc(sizeof(char) * (i + 1));
	if (!s)
		return (NULL);
	s[i] = '\0';
	if (nb < 0)
	{
		s[0] = '-';
		nb = -nb;
	}
	while (i-- > 0 && nb > 0)
	{
		s[i] = 48 + (nb % 10);
		nb /= 10;
	}
	if (n == 0)
		s[0] = '0';
	return (s);
}
