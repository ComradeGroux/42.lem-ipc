/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 14:17:34 by vgroux            #+#    #+#             */
/*   Updated: 2022/10/11 19:21:36 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	unsigned int		i;
	char				*ptr_temp;

	i = ft_strlen(s);
	ptr_temp = (char *)s;
	while (ptr_temp[i] != (char)c && i != 0)
		i--;
	if (ptr_temp[i] != (char)c)
		return (0);
	else
		return (&ptr_temp[i]);
}
