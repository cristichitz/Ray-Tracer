/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 12:44:43 by timurray          #+#    #+#             */
/*   Updated: 2025/08/19 13:52:09 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

char	*ft_read_line_check(char *read_line)
{
	if (!read_line)
	{
		read_line = (char *)malloc(1);
		if (!read_line)
			return (NULL);
		read_line[0] = '\0';
	}
	return (read_line);
}

char	*ft_read_line(char *read_line, int fd)
{
	int		count;
	char	*temp;
	char	*buf;

	buf = (char *)malloc(BUFFER_SIZE + 1);
	if (!buf)
		return (NULL);
	read_line = ft_read_line_check(read_line);
	while (!ft_n_exist(read_line, '\n'))
	{
		count = read(fd, buf, BUFFER_SIZE);
		if (count <= 0)
			break ;
		buf[count] = '\0';
		temp = ft_strjoin(read_line, buf);
		if (!temp)
			return (NULL);
		free(read_line);
		read_line = temp;
	}
	free(buf);
	return (read_line);
}

int	ft_get_n_index(const char *s, int c)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == (const char)c)
			return (i);
		i++;
	}
	return (i);
}

char	*ft_n_exist(const char *s, int c)
{
	while (*s)
	{
		if (*s == (const char)c)
			return ((char *)s);
		s++;
	}
	if (c == '\0' && *s == (const char)c)
		return ((char *)s);
	return (NULL);
}

char	*get_next_line(int fd)
{
	static char	*read_line;
	char		*line;
	char		*temp;
	int			n_index;
	int			length;

	if (fd < 0 || BUFFER_SIZE < 1)
		return (NULL);
	read_line = ft_read_line(read_line, fd);
	if (!read_line || *read_line == '\0')
	{
		free(read_line);
		read_line = NULL;
		return (NULL);
	}
	n_index = ft_get_n_index(read_line, '\n');
	line = ft_substr(read_line, 0, n_index + 1);
	if (!line)
		return (NULL);
	length = (ft_strlen(read_line) - n_index);
	temp = ft_substr(read_line, n_index + 1, length);
	free(read_line);
	read_line = temp;
	return (line);
}
