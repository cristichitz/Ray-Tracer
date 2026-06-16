/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checks.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:08:19 by timurray          #+#    #+#             */
/*   Updated: 2026/06/11 13:30:15 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parse.h"

int	valid_filename(char *filename, const char *ext)
{
	const char		*dot;
	unsigned int	ext_len;
	unsigned int	dot_len;

	dot = ft_strrchr(filename, '.');
	if (!dot || dot == filename)
	{
		print_error("Please input valid .rt files.");
		return (0);
	}
	ext_len = ft_strlen(ext);
	dot_len = ft_strlen(dot);
	if (!(ft_strncmp(dot, ext, ext_len) == 0) || !(ft_strncmp(ext, dot,
				dot_len) == 0))
	{
		print_error("Incorrect file type, only .rt accepted.");
		return (0);
	}
	else
		return ((dot[ext_len] == '\0'));
}

int	set_pts(t_vec3 *pt, char *params, int (*f)(float *n, char *p))
{
	char	**parts;
	int		ok;

	if (!params)
		return (return_print_error("Missing point parameter.", 0));
	if (!valid_csv(params, 2))
		return (0);
	ok = 1;
	parts = ft_split(params, ',');
	if (!parts)
		return (0);
	if (split_len(parts) != 3)
		ok = 0;
	else if (!f(&pt->x, parts[0]) || !f(&pt->y, parts[1]) || !f(&pt->z,
			parts[2]))
		ok = 0;
	ft_free_split(parts);
	return (ok);
}
