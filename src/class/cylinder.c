/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: timurray <timurray@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 13:01:08 by timurray          #+#    #+#             */
/*   Updated: 2026/03/07 12:13:26 by timurray         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"
#include "plane.h"

// TODO: hollow cyclinder... Should be a closed cyclinder.

bool hit_cap(t_cylinder *self, t_vec3 center, t_vec3 normal, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
    float denom;
    float t;
    t_vec3 p;
    t_vec3 to_center;

    denom = dot(normal, ray.dir);
    if (fabs(denom) < 1e-8)
        return (false);

    // t = dot(normal, center - ray.origin) / denom
    t = dot(normal, sub(center, ray.origin)) / denom;

    if (!ray_t.surrounds(&ray_t, t))
        return (false);
    
    p = ray.at(&ray, t);
    to_center = sub(p, center);

    // Check if the intersection point is within the cylinder's radius
    if (dot(to_center, to_center) > (self->radius * self->radius))
        return (false);

    rec->t = t;
    rec->p = p;
    rec->mat = self->mat;
    rec->set_face_normal(rec, ray, normal);
    
    return (true);
}
bool hit_cylinder(void *base, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
    t_cylinder *self = (t_cylinder *)base;
    t_hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = ray_t.max;

	temp_rec = *rec;
    
    t_vec3 w = sub(ray.origin, self->center);
    float dv = dot(ray.dir, self->normal);
    float wv = dot(w, self->normal);
    float a = dot(ray.dir, ray.dir) - dv * dv;
    float half_b = dot(ray.dir, w) - dv * wv;
    float c = dot(w, w) - wv * wv - self->radius * self->radius;
    float discriminant = half_b * half_b - a * c;

    if (discriminant >= 0)
    {
        float sqrtd = sqrt(discriminant);
        float root = (-half_b - sqrtd) / a;
        float m = wv + root * dv;
        bool valid_lateral = false;

        if (ray_t.surrounds(&ray_t, root) && m >= -self->height / 2 && m <= self->height / 2)
            valid_lateral = true;
        else
        {
            root = (-half_b + sqrtd) / a;
            m = wv + root * dv;
            if (ray_t.surrounds(&ray_t, root) && m >= -self->height / 2 && m <= self->height / 2)
                valid_lateral = true;
        }

        if (valid_lateral)
        {
            hit_anything = true;
            closest_so_far = root;
            
            rec->t = root;
            rec->p = ray.at(&ray, root);
            t_vec3 outward_normal = norm(sub(sub(rec->p, self->center), scale(self->normal, m)));
            rec->set_face_normal(rec, ray, outward_normal);
            rec->mat = self->mat;
        }
    }
    t_interval cap_interval = ray_t;
    cap_interval.max = closest_so_far;

    t_vec3 top_center = add(self->center, scale(self->normal, self->height / 2.0f));
    if (hit_cap(self, top_center, self->normal, ray, cap_interval, &temp_rec))
    {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        cap_interval.max = closest_so_far; // Shrink interval again for bottom cap
        *rec = temp_rec;
    }

    t_vec3 bottom_center = sub(self->center, scale(self->normal, self->height / 2.0f));
    t_vec3 bottom_normal = scale(self->normal, -1.0f);
    if (hit_cap(self, bottom_center, bottom_normal, ray, cap_interval, &temp_rec))
    {
        hit_anything = true;
        *rec = temp_rec;
    }

    return (hit_anything);
}

t_cylinder *make_cylinder(t_cylinder cylinder)
{
	t_cylinder *c;
	float D;

	c = malloc(sizeof(t_cylinder));
	if (!c)
		return (NULL);
	*c = cylinder;
	c->base.hit = hit_cylinder;
	D = dot(c->normal, c->center);

	c->D_top = D + (c->height / 2.0f);
	return (c);
}