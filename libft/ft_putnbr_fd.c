/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yilgar <yilgar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 20:20:11 by yilgar            #+#    #+#             */
/*   Updated: 2024/10/24 17:27:02 by yilgar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	unsigned int	nmbr;

	if (n == -2147483648)
	{
		ft_putchar_fd('-', fd);
		ft_putstr_fd("2147483648", fd);
		return ;
	}
	if (n < 0)
	{
		ft_putchar_fd('-', fd);
		nmbr = n * (-1);
	}
	else
		nmbr = n;
	if (nmbr >= 10)
	{
		ft_putnbr_fd(nmbr / 10, fd);
	}
	ft_putchar_fd((nmbr % 10) + '0', fd);
}
