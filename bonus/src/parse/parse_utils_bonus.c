/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 18:53:17 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/12 18:53:17 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_bonus.h"
#include "libft.h"

int	parse_err(char *msg)
{
	printf("Error\n%s\n", msg);
	return (0);
}

size_t	split_len(char **s)
{
	size_t	n;

	n = 0;
	if (!s)
		return (0);
	while (s[n])
		n++;
	return (n);
}

/*
** Parse "x,y,z" into a cl_float3. make_float3 zeroes the 4th (w) lane so the
** struct uploads cleanly to the GPU.
*/
int	set_vec3(cl_float3 *v, char *s)
{
	char	**p;

	p = ft_split(s, ',');
	if (!p)
		return (0);
	if (split_len(p) != 3)
	{
		ft_free_split(p);
		return (parse_err("Expected three comma-separated values."));
	}
	*v = make_float3(ft_strtof(p[0], NULL), ft_strtof(p[1], NULL),
			ft_strtof(p[2], NULL));
	ft_free_split(p);
	return (1);
}

/*
** Parse an "r,g,b" triple (0-255) into a normalised [0,1] colour.
*/
int	set_color(cl_float3 *c, char *s)
{
	char	**p;

	p = ft_split(s, ',');
	if (!p)
		return (0);
	if (split_len(p) != 3)
	{
		ft_free_split(p);
		return (parse_err("Expected three comma-separated colour channels."));
	}
	*c = make_float3(ft_strtof(p[0], NULL) / 255.0f,
			ft_strtof(p[1], NULL) / 255.0f, ft_strtof(p[2], NULL) / 255.0f);
	ft_free_split(p);
	return (1);
}
