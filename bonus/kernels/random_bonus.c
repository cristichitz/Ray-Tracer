/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:00:42 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 17:01:19 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

uint	pcg_hash(uint *seed)
{
	uint	state;
	uint	word;

	state = *seed;
	*seed = state * 747796405u + 2891336453u;
	word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return ((word >> 22u) ^ word);
}

float	random_float(float min, float max, uint *seed)
{
	float	random_unorm;

	random_unorm = (float)pcg_hash(seed) / (float)0xFFFFFFFFu;
	return (min + (max - min) * random_unorm);
}

float3	random_vec_between(float min, float max, uint *seed)
{
	return ((float3)(random_float(min, max, seed), random_float(min, max, seed),
		random_float(min, max, seed)));
}

float3	random_unit_vector(uint *seed)
{
	float3	p;
	float	lensq;

	while (1)
	{
		p = random_vec_between(-1.0f, 1.0f, seed);
		lensq = dot(p, p);
		if (1e-32f < lensq && lensq <= 1.0f)
			return (p / sqrt(lensq));
	}
}

bool	near_zero(float3 a)
{
	float	s;

	s = 1e-8;
	return ((fabs(a.x) < s) && (fabs(a.y) < s) && (fabs(a.z) < s));
}
