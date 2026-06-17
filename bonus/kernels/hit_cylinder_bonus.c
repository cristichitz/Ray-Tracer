/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hit_cylinder_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdohanic <cdohanic@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:05:27 by cdohanic          #+#    #+#             */
/*   Updated: 2026/06/17 17:05:27 by cdohanic         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "init_kernel.h"

bool  hit_cap(t_object self, float3 center, float3 normal, t_ray ray,
              t_interval ray_t, t_hit_record *rec)
{
  float denom = dot(normal, ray.dir);
  if (fabs(denom) < 1e-8f)
    return (false);

  float t = dot(normal, center - ray.origin) / denom;
  if (!interval_surrounds(&ray_t, t))
    return (false);

  float3 p = ray_at(&ray, t);
  float3 to_center = p - center;
  if (dot(to_center, to_center) > self.radius * self.radius)
    return (false);

  rec->t = t;
  rec->p = p;
  rec->mat = self.material;
  ft_set_face_normal(rec, ray, normal);
  return (true);
}

bool  hit_cylinder(t_object self, t_ray ray, t_interval ray_t, t_hit_record *rec)
{
  t_hit_record temp_rec;
  bool  hit_anything = false;
  float closest_so_far = ray_t.max;
  float half_h = self.height / 2.0f;

  float3 w = ray.origin - self.center;
  float dv = dot(ray.dir, self.normal);
  float wv = dot(w, self.normal);
  float a = dot(ray.dir, ray.dir) - dv * dv;
  float half_b = dot(ray.dir, w) - dv * wv;
  float c = dot(w, w) - wv * wv - self.radius * self.radius;
  float discriminant = half_b * half_b - a * c;

  if (discriminant >= 0)
  {
    float sqrtd = sqrt(discriminant);
    float root = (-half_b - sqrtd) / a;
    float m = wv + root * dv;
    bool valid = interval_surrounds(&ray_t, root) && m >= -half_h && m <= half_h;
    if (!valid)
    {
      root = (-half_b + sqrtd) / a;
      m = wv + root * dv;
      valid = interval_surrounds(&ray_t, root) && m >= -half_h && m <= half_h;
    }
    if (valid)
    {
      hit_anything = true;
      closest_so_far = root;
      rec->t = root;
      rec->p = ray_at(&ray, root);
      float3 outward = normalize((rec->p - self.center) - self.normal * m);
      ft_set_face_normal(rec, ray, outward);
      rec->mat = self.material;
    }
  }

  t_interval cap_interval = ray_t;
  cap_interval.max = closest_so_far;
  float3 top_center = self.center + self.normal * half_h;
  if (hit_cap(self, top_center, self.normal, ray, cap_interval, &temp_rec))
  {
    hit_anything = true;
    cap_interval.max = temp_rec.t;
    *rec = temp_rec;
  }
  float3 bottom_center = self.center - self.normal * half_h;
  if (hit_cap(self, bottom_center, self.normal * -1.0f, ray, cap_interval, &temp_rec))
  {
    hit_anything = true;
    *rec = temp_rec;
  }
  return (hit_anything);
}
