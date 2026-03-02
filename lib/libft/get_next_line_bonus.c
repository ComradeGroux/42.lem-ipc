/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 17:50:59 by vgroux            #+#    #+#             */
/*   Updated: 2022/10/31 18:42:01 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*get_next_line(int fd)
{
	static char	*buffer[4096];
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	buffer[fd] = ft_gnl_read(fd, buffer[fd]);
	if (!buffer[fd])
		return (NULL);
	line = ft_gnl_get_line(buffer[fd]);
	buffer[fd] = ft_gnl_remove_old_line(buffer[fd]);
	return (line);
}

char	*ft_gnl_read(int fd, char *res)
{
	ssize_t	bytes_readed;
	char	*buffer;

	if (!res)
		res = (char *)ft_calloc(1, sizeof(char));
	buffer = (char *)ft_calloc(BUFFER_SIZE + 1, sizeof(char));
	if (!res || !buffer)
		return (NULL);
	bytes_readed = 1;
	while (bytes_readed > 0)
	{
		bytes_readed = read(fd, buffer, BUFFER_SIZE);
		if (bytes_readed == -1)
		{
			free(buffer);
			free(res);
			return (NULL);
		}
		buffer[bytes_readed] = 0;
		res = ft_gnl_strjoin(res, buffer);
		if (ft_gnl_strchr(buffer) != -1)
			break ;
	}
	free(buffer);
	return (res);
}

char	*ft_gnl_get_line(char *buffer)
{
	int		i;
	char	*str;

	if (!buffer[0])
		return (NULL);
	i = ft_gnl_strchr(buffer);
	if (i == -1)
		i = ft_strlen(buffer);
	str = ft_gnl_strndup(buffer, i);
	if (!str)
		return (NULL);
	return (str);
}

char	*ft_gnl_remove_old_line(char *buffer)
{
	char	*str;
	int		i;
	int		j;

	if (buffer == NULL)
		return (ft_gnl_remove_free(buffer));
	i = ft_gnl_strchr(buffer);
	if (i == -1)
		return (ft_gnl_remove_free(buffer));
	str = (char *)ft_calloc(ft_strlen(buffer) - i + 1, sizeof(char));
	if (!str)
		return (ft_gnl_remove_free(buffer));
	j = 0;
	while (buffer[++i])
		str[j++] = buffer[i];
	str[j] = '\0';
	free(buffer);
	return (str);
}

char	*ft_gnl_remove_free(char *buffer)
{
	free(buffer);
	return (NULL);
}
