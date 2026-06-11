#include "rt_bonus.h"
#include "libft.h"

// sp  x,y,z  diameter  r,g,b
int  set_sphere(t_data *data, char **p)
{
  cl_float3 center;
  cl_float3 color;
  float     diameter;

  if (split_len(p) != 4)
    return (parse_err("sp: expected 'sp center diameter colour'."));
  if (!set_vec3(&center, p[1]) || !set_color(&color, p[3]))
    return (0);
  diameter = ft_strtof(p[2], NULL);
  return (add_object(data,
      make_obj_sphere(center, diameter / 2.0f, material_init(color, 0))));
}

// pl  x,y,z  nx,ny,nz  r,g,b
int  set_plane(t_data *data, char **p)
{
  cl_float3 point;
  cl_float3 normal;
  cl_float3 color;

  if (split_len(p) != 4)
    return (parse_err("pl: expected 'pl point normal colour'."));
  if (!set_vec3(&point, p[1]) || !set_vec3(&normal, p[2])
    || !set_color(&color, p[3]))
    return (0);
  return (add_object(data,
      make_obj_plane(point, norm(normal), material_init(color, 0))));
}

// cy  x,y,z  nx,ny,nz  diameter  height  r,g,b
int  set_cylinder(t_data *data, char **p)
{
  cl_float3 center;
  cl_float3 axis;
  cl_float3 color;
  float     diameter;
  float     height;

  if (split_len(p) != 6)
    return (parse_err("cy: expected 'cy center axis diameter height colour'."));
  if (!set_vec3(&center, p[1]) || !set_vec3(&axis, p[2])
    || !set_color(&color, p[5]))
    return (0);
  diameter = ft_strtof(p[3], NULL);
  height = ft_strtof(p[4], NULL);
  return (add_object(data, make_obj_cylinder(center, norm(axis),
          diameter / 2.0f, height, material_init(color, 0))));
}
