/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/14 15:35:30 by vgroux            #+#    #+#             */
/*   Updated: 2022/11/30 18:04:32 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_count_words(char const *s, char c)
{
	char	last;
	int		i;
	int		j;

	last = c;
	i = 0;
	j = 0;
	while (s[i] != '\0')
	{
		if (last == c && s[i] != c)
		{
			j++;
		}
		last = s[i];
		i++;
	}
	return (j);
}

static char	*ft_dup_word(const char *s, size_t start, size_t end)
{
	char	*word;
	int		i;

	i = 0;
	word = (char *)malloc((end - start + 1) * sizeof(char));
	if (!word)
		return (NULL);
	while (start < end)
	{
		word[i] = (char)s[start];
		i++;
		start++;
	}
	word[i] = '\0';
	return (word);
}

char	**ft_split(const char *s, char c)
{
	char	**strs;
	size_t	i;
	size_t	j;
	int		startword;

	strs = (char **)malloc((ft_count_words(s, c) + 1) * sizeof(char *));
	if (!s || !strs)
		return (NULL);
	i = -1;
	j = 0;
	startword = -1;
	while (++i <= ft_strlen(s))
	{
		if (s[i] != c && startword < 0)
			startword = i;
		else if ((s[i] == c || i == ft_strlen(s)) && startword >= 0)
		{
			strs[j] = ft_dup_word(s, startword, i);
			if (!(strs[j++]))
				return (ft_free_arr(strs));
			startword = -1;
		}
	}
	strs[j] = NULL;
	return (strs);
}
