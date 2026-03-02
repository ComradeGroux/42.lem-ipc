/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 11:57:15 by vgroux            #+#    #+#             */
/*   Updated: 2022/10/18 13:10:04 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	char	*temp;
	size_t	i;

	temp = (char *)src;
	i = 0;
	if ((char *)dst == temp)
		return (dst);
	else if ((char *)dst > temp)
		while (len-- > 0)
			((char *)dst)[len] = temp[len];
	else
	{
		while (i < len)
		{
			((char *)dst)[i] = temp[i];
			i++;
		}
	}
	return (dst);
}
