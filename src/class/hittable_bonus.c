#include "rt_bonus.h"

// Ray object
t_vec3 ray_at(t_ray *self, float t)
{
  t_vec3 result;
  // camera_center + t * dir
  result = add(self->origin, scale(self->dir, t));
  return (result);
}

t_ray make_ray(t_vec3 origin, t_vec3 direction) {
  t_ray r;

  r.origin = origin;
  r.dir = direction;
  r.at = ray_at;
  return (r);
}

// Hit record normal vector
void  ft_set_face_normal(t_hit_record *self, t_ray r, t_vec3 outward_normal)
{
  // true if the ray comes from outside the sphere 
  // (as in the ray doesn t come from inside like in a lightbulb)
  self->front_face = dot(r.dir, outward_normal) < 0;
  if (self->front_face != true)
    outward_normal = scale(outward_normal, -1.0f);
  self->normal = outward_normal;
}

// List of hittable objects
int add_object(t_hittable_list *self, void *object)
{
  if (ft_vec_push(self->objects, object) != 1)
    return (EXIT_FAILURE);
  return (EXIT_SUCCESS);
}

void destroy_objects(t_hittable_list *self)
{
  ft_vec_free(self->objects); 
}

bool hit_objects(t_hittable_list *self, t_ray ray, t_interval t, t_hit_record *rec)
{
  t_hit_record temp_rec;
  temp_rec.set_face_normal = ft_set_face_normal;

  bool hit_anything = false;
  float closest_so_far = t.max;

  uint32_t i;
  t_hittable *object;
  i = 0;
  while (i < self->objects->len)
  {
    object = ft_vec_get(self->objects, i);
    if (object->hit(object, ray, interval_init(t.min, closest_so_far), &temp_rec))
    {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      *rec = temp_rec;
    }
    i++;
  }
  return hit_anything; 
}

int  init_world(t_hittable_list  *world)
{
  ft_vec_new(world->objects, 0, sizeof(void *));

  world->add = add_object;
  world->destroy = destroy_objects;
  world->hit = hit_objects;
  return (EXIT_SUCCESS);
}
