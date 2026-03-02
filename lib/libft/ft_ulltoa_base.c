/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ulltoa.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 13:52:03 by vgroux            #+#    #+#             */
/*   Updated: 2022/10/24 18:07:00 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_ulllen(unsigned long long n, char *base)
{
	int	len;

	len = 0;
	if (n == 0)
		len++;
	while (n > 0)
	{
		n /= ft_strlen(base);
		len++;
	}
	return (len);
}

char	*ft_ulltoa_base(unsigned long long n, char *base)
{
	int		i;
	char	*str;

	i = ft_ulllen(n, base);
	str = (char *)malloc(sizeof(char) * (i + 1));
	if (!str)
		return (NULL);
	str[i--] = '\0';
	if (n <= 0)
		str[i] = 0;
	while (i >= 0)
	{
		str[i--] = base[n % ft_strlen(base)];
		n /= ft_strlen(base);
	}
	return (str);
}
