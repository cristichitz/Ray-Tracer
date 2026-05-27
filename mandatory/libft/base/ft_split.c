/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/04 15:47:59 by timurray          #+#    #+#             */
/*   Updated: 2025/08/18 14:48:50 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static size_t	ft_strcount(char *s, char c);
static char		**free_str(char **str, size_t i);
static char		*get_word(const char *start, size_t length);
static size_t	get_str_len(char const *s, char c);

char	**ft_split(char const *s, char c)
{
	char	**str;
	size_t	i;

	if (!s)
		return (NULL);
	str = (char **)malloc(((ft_strcount((char *)s, c)) + 1) * sizeof(char *));
	if (!str)
		return (NULL);
	i = 0;
	while (*s)
	{
		if (*s != c)
		{
			str[i] = get_word(s, get_str_len(s, c));
			s = s + get_str_len(s, c);
			if (!str[i++])
				return (free_str(str, i - 1));
		}
		else
			s++;
	}
	str[i] = NULL;
	return (str);
}

static size_t	get_str_len(char const *s, char c)
{
	size_t	len;

	len = 0;
	while (*s && *s != c)
	{
		s++;
		len++;
	}
	return (len);
}

static char	*get_word(const char *start, size_t length)
{
	char	*word;
	size_t	i;

	word = (char *)malloc(length + 1);
	if (!word)
		return (NULL);
	i = 0;
	while (i < length)
	{
		word[i] = start[i];
		i++;
	}
	word[i] = '\0';
	return (word);
}

static char	**free_str(char **str, size_t i)
{
	while (i--)
		free(str[i]);
	free(str);
	return (NULL);
}

static size_t	ft_strcount(char *s, char c)
{
	size_t	count;
	int		word_status;

	count = 0;
	word_status = 0;
	while (*s)
	{
		if (*s != c && !word_status)
		{
			word_status = 1;
			count++;
		}
		else if (*s == c)
			word_status = 0;
		s++;
	}
	return (count);
}
