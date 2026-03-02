/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 10:37:41 by vgroux            #+#    #+#             */
/*   Updated: 2022/10/11 19:18:00 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	int		i;
	char	*ptr_temp;

	i = 0;
	ptr_temp = (char *)s;
	while (ptr_temp[i] != (char)c)
	{
		if (ptr_temp[i] == '\0')
			return (NULL);
		i++;
	}
	return (&ptr_temp[i]);
}
