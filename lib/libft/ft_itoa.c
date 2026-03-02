/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/14 19:53:44 by vgroux            #+#    #+#             */
/*   Updated: 2022/10/14 21:23:26 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_intlen(long n)
{
	int	len;

	len = 0;
	if (n < 0)
	{
		n *= -1;
		len++;
	}
	else if (n == 0)
		len++;
	while (n > 0)
	{
		n /= 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	char	*str;
	long	num;
	size_t	i;

	num = n;
	i = ft_intlen(num);
	str = (char *)malloc((i + 1) * sizeof(char));
	if (!str)
		return (NULL);
	if (num < 0)
	{
		num *= -1;
		str[0] = '-';
	}
	str[i--] = '\0';
	if (num == 0)
		str[i] = '0';
	while (num > 0)
	{
		str[i--] = (num % 10) + '0';
		num /= 10;
	}
	return (str);
}
