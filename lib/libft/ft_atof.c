/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atod.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/24 15:55:26 by vgroux            #+#    #+#             */
/*   Updated: 2022/12/24 15:40:17 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

double	ft_atof(const char *str)
{
	int		i;
	double	j;
	double	sum;
	double	temp;

	i = 0;
	j = 1;
	temp = 0.1;
	if (str[i] == '-' && str[i++] == '-')
		j = -1;
	else if (str[i] == '+')
		i++;
	sum = 0.0;
	while (str[i] > 47 && str[i] < 58 && str[i] != '.')
		sum = (sum * 10) + (str[i++] - 48);
	if (str[i] == '.')
		i++;
	while (str[i] > 47 && str[i] < 58)
	{
		sum += ((str[i] - 48) * temp);
		temp /= 10;
		i++;
	}
	return (sum * j);
}
