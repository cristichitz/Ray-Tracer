#ifndef RT_CPU_H
# define RT_CPU_H

# include "MLX42/MLX42.h"
# include "cam.h"
# include "cylinder.h"
# include "light.h"
# include "sphere.h"
# include "plane.h"
# include "types.h"

# define WIDTH 640
#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

typedef t_vec					t_list;

typedef struct s_hittable_list	t_hittable_list;
typedef struct s_hittable_list
{
	t_list						*objects;
	int							(*add)(t_hittable_list *self, void *object);
	void						(*destroy)(t_hittable_list *self);
	bool						(*hit)(t_hittable_list *self, t_ray r,
								t_interval t, t_hit_record *rec);
}								t_hittable_list;

typedef struct s_data
{
	void						*mlx;
	void						*img;

	uint32_t					width;
	uint32_t					height;

	uint32_t					samples_per_pixel;
	float						pixel_samples_scale;

	t_cam						cam;
	t_ambient_light				ambient_light;
	t_light						light;

	float						aspect_ratio;
	float						viewport_w;
	float						viewport_h;
	float						focal_length;

	t_vec3						origin;
	t_vec3						pixel00_loc;

	t_vec3						horizontal;
	t_vec3						vertical;

	t_vec3						px_w;
	t_vec3						px_h;

	bool						set_cam;
	bool						set_ambient_light;
	bool						set_light;

	// List of objects
	t_hittable_list				world;
}								t_data;

// World
int								init_world(t_hittable_list *world);

// MLX Loop
int								render_frame(t_data *data);
int								close_app(t_data *data);
int								key_hook(int keycode, t_data *data);

// Can be made inline if we add -O1 optimization for the compilation
// Random number generator
float							random_float(float min, float max);
// Constants
float							degrees_to_radians(float degrees);

#endif
