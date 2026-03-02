/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 15:48:16 by vgroux            #+#    #+#             */
/*   Updated: 2022/10/25 15:56:55 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_printf_char(char c)
{
	return (write(1, &c, 1));
}

int	ft_printf_str(char *str)
{
	int	i;
	int	len;

	i = 0;
	len = 0;
	if (str != NULL)
	{
		while (str[i] != '\0')
		{
			len += ft_printf_char(str[i]);
			i++;
		}
	}
	else
		len = ft_printf_str("(null)");
	return (len);
}

int	ft_printf_n_base(long long num, char *base)
{
	int		baselen;
	int		len;

	baselen = ft_strlen(base);
	len = 0;
	if (num < 0)
	{
		len += ft_printf_char('-');
		len += ft_printf_n_base(num * -1, base);
	}
	else if (num >= baselen)
	{
		len += ft_printf_n_base(num / baselen, base);
		len += ft_printf_n_base(num % baselen, base);
	}
	else
		len += ft_printf_char(base[num]);
	return (len);
}

int	ft_printf_ui(unsigned int n)
{
	char	*str;
	int		len;

	str = ft_ulltoa_base(n, "0123456789");
	if (!str)
		return (0);
	len = ft_printf_str(str);
	free(str);
	return (len);
}
