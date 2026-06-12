#include "box.h"

bool	hit_box(void *base, t_ray ray, t_interval t, t_hit_record *rec)
{
	t_box	*self;
	t_hit_record	tmp;
	bool			hit;
	float			closest;
	int				i;

	self = (t_box *)base;
	tmp.set_face_normal = ft_set_face_normal;
	hit = false;
	closest = t.max;
	i = 0;
	while (i < 6)
	{
		if (self->sides[i]->base.hit(self->sides[i], ray, interval_init(t.min, closest), &tmp))
		{
			hit = true;
			closest = tmp.t;
			*rec = tmp;
		}
		i++;
	}
	return (hit);
}

t_box	*make_box(t_vec3 a, t_vec3 b, t_material mat)
{
	t_box *boxi;
	t_vec3 min;
	t_vec3 max;

	t_vec3 dx;
	t_vec3 dy;
	t_vec3 dz;

	boxi = malloc(sizeof(t_box));
	if (!boxi)
		return (NULL);

	min = make_vec(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
	max = make_vec(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));

	dx = make_vec(max.x - min.x, 0.0f, 0.0f);
	dy = make_vec(0.0f, max.y - min.y, 0.0f);
	dz = make_vec(0.0f, 0.0f, max.z - min.z);

	boxi->sides[0] = make_quad(make_vec(min.x, min.y, max.z), dx, dy, mat); // front
	boxi->sides[1] = make_quad(make_vec(max.x, min.y, max.z), scale(dz, -1), dy, mat); // right
	boxi->sides[2] = make_quad(make_vec(max.x, min.y, min.z), scale(dx, -1), dy, mat); // back

	boxi->sides[3] = make_quad(make_vec(min.x, min.y, min.z), dz, dy, mat); // left
	boxi->sides[4] = make_quad(make_vec(min.x, max.y, max.z), dx, scale(dz, -1), mat); // top
	boxi->sides[5] = make_quad(make_vec(min.x, min.y, min.z), dx, dz, mat); // bottom

	boxi->base.hit = hit_box;
	return (boxi);
}