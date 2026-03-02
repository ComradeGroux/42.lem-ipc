/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_ptr.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 16:29:25 by vgroux            #+#    #+#             */
/*   Updated: 2022/10/24 18:06:44 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_printf_ptr(unsigned long ptr)
{
	int		len;
	char	*str;

	len = ft_printf_str("0x");
	if (ptr == 0)
		len += ft_printf_char('0');
	else
	{
		str = ft_ulltoa_base(ptr, "0123456789abcdef");
		if (!str)
			return (0);
		len += ft_printf_str(str);
		free(str);
	}
	return (len);
}
