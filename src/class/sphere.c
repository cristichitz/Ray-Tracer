#include "sphere.h"

bool	hit_sphere(void *base, t_ray ray, t_interval t, t_hit_record *rec)
{
	t_sphere	*self;
	t_vec3		oc;
	float		a;
	float		h;
	float		c;
	float		discriminant;
	float		sqrtd;
	float		root;
	t_vec3		outward_normal;

	self = (t_sphere *)base;
	oc = sub(self->center, ray.origin);
	a = dot(ray.dir, ray.dir);
	h = dot(ray.dir, oc);
	c = dot(oc, oc) - self->radius * self->radius;
	discriminant = h * h - a * c;
	if (discriminant < 0)
		return (false); // ray doesn't hit sphere.
	sqrtd = sqrt(discriminant);
	root = (h - sqrtd) / a;
	if (!t.surrounds(&t, root))
	{
		root = (h + sqrtd) / a;
		if (!t.surrounds(&t, root))
			return (false);
	}
	rec->t = root;
	// point at t in the ray direction
	rec->p = ray.at(&ray, rec->t);
	rec->mat = self->mat;
	// Point - sphere_center is already a normal vector
	// Dividing by the radius makes it unit length aswell
	outward_normal = divide(sub(rec->p, self->center), self->radius);
	rec->set_face_normal(rec, ray, outward_normal);

	return (true);
}

// t_sphere	*make_sphere(t_vec3 center, float radius)
t_sphere	*make_sphere(t_sphere sphere)
{
	t_sphere	*s;

	s = malloc(sizeof(t_sphere));
	if (!s)
		return (NULL);
	*s = sphere;
	s->base.hit = hit_sphere;
	return (s);
}
