/*
** metal_bridge.m — Objective-C (ARC) bridge between the C miniRT runtime
**                  and Apple Metal GPU compute.
**
** Compile with:  clang -ObjC -fobjc-arc
** Link with:     -framework Metal -framework Foundation
**
** Design notes:
**   • All Metal state lives as file-scope ARC __strong statics so it persists
**     for the program lifetime without explicit retain/release.
**   • The pixel buffer uses MTLResourceStorageModeShared → zero-copy on M2:
**     the GPU writes and the CPU reads the same physical memory.
**   • render_frame_metal() blocks until the GPU finishes (waitUntilCompleted)
**     so the caller can immediately memcpy the result into MLX42's pixel buf.
**   • Object type is identified by comparing the hit function pointer to the
**     known addresses of hit_sphere/hit_cylinder/hit_plane/hit_quad.
*/

#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include <string.h>
#include <stdio.h>
#include <mach/mach_time.h>

#include "rt_cpu.h"
#include "metal_bridge.h"
#include "metal_types.h"
#include "sphere.h"
#include "cylinder.h"
#include "plane.h"
#include "quad.h"

/*
** External hit-function pointers used to identify object types.
** All four are non-static, so their addresses are visible here.
*/
extern bool	hit_sphere(void *base, t_ray ray, t_interval t, t_hit_record *rec);
extern bool	hit_cylinder(void *base, t_ray ray, t_interval t, t_hit_record *rec);
extern bool	hit_plane(void *base, t_ray ray, t_interval t, t_hit_record *rec);
extern bool	hit_quad(void *base, t_ray ray, t_interval t, t_hit_record *rec);

/* ======================================================================== */
/*  File-scope Metal state (owned by ARC, lives for the program lifetime)   */
/* ======================================================================== */

static __strong id<MTLDevice>               s_device   = nil;
static __strong id<MTLCommandQueue>         s_queue    = nil;
static __strong id<MTLComputePipelineState> s_pipeline = nil;
static __strong id<MTLBuffer>               s_px_buf   = nil;  /* pixel output */
static __strong id<MTLBuffer>               s_cam_buf  = nil;  /* MetalCam     */
static __strong id<MTLBuffer>               s_obj_buf  = nil;  /* MetalObj[]   */
static __strong id<MTLBuffer>               s_cnt_buf  = nil;  /* uint obj_cnt */

static uint32_t s_px_w   = 0;   /* cached pixel buffer dimensions */
static uint32_t s_px_h   = 0;
static uint32_t s_obj_cap = 0;  /* allocated capacity of s_obj_buf (# objs)  */

/* ======================================================================== */
/*  Public: metal_init()                                                     */
/* ======================================================================== */

int metal_init(void)
{
	NSError *err = nil;

	/* 1. Default Metal device (M2 GPU). */
	s_device = MTLCreateSystemDefaultDevice();
	if (!s_device)
	{
		fprintf(stderr, "[Metal] No Metal device found.\n");
		return (0);
	}

	/* 2. Command queue. */
	s_queue = [s_device newCommandQueue];
	if (!s_queue)
	{
		fprintf(stderr, "[Metal] Failed to create command queue.\n");
		return (0);
	}

	/*
	** 3. Load the precompiled Metal shader library.
	** render.metallib is built by the Makefile and must be present at this
	** relative path when the binary is run from the project root.
	*/
	NSString *lib_path = @"./mandatory/metal/render.metallib";
	NSURL    *lib_url  = [NSURL fileURLWithPath:lib_path];
	id<MTLLibrary> lib = [s_device newLibraryWithURL:lib_url error:&err];
	if (!lib)
	{
		fprintf(stderr, "[Metal] Failed to load render.metallib: %s\n",
			[[err localizedDescription] UTF8String]);
		fprintf(stderr, "[Metal] Make sure to run ./miniRT from the project root.\n");
		return (0);
	}

	/* 4. Look up the compute kernel function. */
	id<MTLFunction> fn = [lib newFunctionWithName:@"ray_trace"];
	if (!fn)
	{
		fprintf(stderr, "[Metal] 'ray_trace' kernel not found in library.\n");
		return (0);
	}

	/* 5. Build the compute pipeline state. */
	s_pipeline = [s_device newComputePipelineStateWithFunction:fn error:&err];
	if (!s_pipeline)
	{
		fprintf(stderr, "[Metal] Pipeline creation failed: %s\n",
			[[err localizedDescription] UTF8String]);
		return (0);
	}

	/*
	** 6. Allocate fixed-size shared buffers for camera and object count.
	** MTLResourceStorageModeShared = same physical memory for CPU and GPU
	** on Apple Silicon — no copy needed in either direction.
	*/
	s_cam_buf = [s_device newBufferWithLength:sizeof(t_metal_cam)
	                                  options:MTLResourceStorageModeShared];
	s_cnt_buf = [s_device newBufferWithLength:sizeof(uint32_t)
	                                  options:MTLResourceStorageModeShared];
	if (!s_cam_buf || !s_cnt_buf)
	{
		fprintf(stderr, "[Metal] Buffer allocation failed.\n");
		return (0);
	}

	fprintf(stderr, "[Metal] Initialized on: %s\n",
		[[s_device name] UTF8String]);
	return (1);
}

/* ======================================================================== */
/*  Buffer helpers                                                           */
/* ======================================================================== */

/*
** Reallocate the pixel output buffer only when the window dimensions change.
** On M2 with MTLResourceStorageModeShared, the buffer's .contents pointer is
** directly readable by the CPU after the GPU finishes — zero extra copy.
*/
static void	ensure_pixel_buf(uint32_t w, uint32_t h)
{
	if (s_px_buf && s_px_w == w && s_px_h == h)
		return ;
	s_px_buf = [s_device newBufferWithLength:(NSUInteger)(w * h * 4)
	                                 options:MTLResourceStorageModeShared];
	s_px_w = w;
	s_px_h = h;
}

/*
** Reallocate the object buffer if the scene has grown beyond current capacity.
** We add headroom (+16) to avoid reallocating on every single-object addition.
*/
static void	ensure_obj_buf(uint32_t count)
{
	if (s_obj_buf && s_obj_cap >= count)
		return ;
	s_obj_cap = count + 16;
	s_obj_buf = [s_device newBufferWithLength:(NSUInteger)(s_obj_cap * sizeof(t_metal_obj))
	                                  options:MTLResourceStorageModeShared];
}

/* ======================================================================== */
/*  Data packing: C structs → GPU-friendly flat structs                     */
/* ======================================================================== */

/* Lift a t_vec3 (C struct) into a t_mfloat3 (GPU-compatible scalar struct). */
static t_mfloat3	mf3(t_vec3 v)
{
	t_mfloat3	out;

	out.x = v.x;
	out.y = v.y;
	out.z = v.z;
	return (out);
}

/* Pack all camera / render-settings from t_data into MetalCam. */
static void	pack_cam(t_metal_cam *cam, const t_data *data)
{
	cam->origin   = mf3(data->origin);
	cam->pixel00  = mf3(data->pixel00_loc);
	cam->px_w     = mf3(data->px_w);
	cam->px_h     = mf3(data->px_h);
	cam->ambient  = mf3(data->world.ambient);
	cam->background = mf3(data->world.background);
	cam->width    = data->width;
	cam->height   = data->height;
	cam->max_depth        = data->max_depth;
	cam->samples_per_pixel = data->samples_per_pixel;
	cam->frame_count = data->frame_count;
	if (data->set_light)
	{
		cam->has_light        = 1;
		cam->light_pos        = mf3(data->light.center);
		cam->light_color      = mf3(data->light.colour);
		cam->light_brightness = data->light.brightness;
	}
	else
	{
		cam->has_light        = 0;
		cam->light_brightness = 0.0f;
	}
}

/*
** Pack the scene objects from t_data into a flat MetalObj array.
** Object type is identified by comparing the hit function pointer to the
** known non-static hit_* functions linked into the binary.
** Returns the number of objects successfully packed.
*/
static uint32_t	pack_objects(t_metal_obj *objs, const t_data *data)
{
	uint32_t	count;
	size_t		i;
	t_hittable	*h;
	t_metal_obj	*o;
	t_vec3		em;

	count = 0;
	i = 0;
	while (i < data->world.objects->len)
	{
		h = (t_hittable *)ft_vec_get(data->world.objects, i);
		i++;
		if (!h)
			continue ;
		o = &objs[count];
		memset(o, 0, sizeof(*o));
		if (h->hit == hit_sphere)
		{
			t_sphere *s = (t_sphere *)h;
			o->type   = MOBJ_SPHERE;
			o->center = mf3(s->center);
			o->radius = s->radius;
			o->albedo = mf3(s->mat.tex.albedo);
			if (s->mat.emitted)
			{
				em = s->mat.emitted(&s->mat, 0.0f, 0.0f, s->center);
				if (em.x > 0.0f || em.y > 0.0f || em.z > 0.0f)
				{
					o->is_emissive = 1;
					o->emission    = mf3(em);
				}
			}
		}
		else if (h->hit == hit_cylinder)
		{
			t_cylinder *c = (t_cylinder *)h;
			o->type   = MOBJ_CYLINDER;
			o->center = mf3(c->center);
			o->normal = mf3(c->normal);
			o->radius = c->radius;
			o->height = c->height;
			o->albedo = mf3(c->mat.tex.albedo);
			if (c->mat.emitted)
			{
				em = c->mat.emitted(&c->mat, 0.0f, 0.0f, c->center);
				if (em.x > 0.0f || em.y > 0.0f || em.z > 0.0f)
				{
					o->is_emissive = 1;
					o->emission    = mf3(em);
				}
			}
		}
		else if (h->hit == hit_plane)
		{
			t_plane *p = (t_plane *)h;
			o->type   = MOBJ_PLANE;
			o->center = mf3(p->Q);
			o->normal = mf3(p->normal);
			o->D      = p->D;
			o->albedo = mf3(p->mat.tex.albedo);
			if (p->mat.emitted)
			{
				em = p->mat.emitted(&p->mat, 0.0f, 0.0f, p->Q);
				if (em.x > 0.0f || em.y > 0.0f || em.z > 0.0f)
				{
					o->is_emissive = 1;
					o->emission    = mf3(em);
				}
			}
		}
		else if (h->hit == hit_quad)
		{
			t_quad *q = (t_quad *)h;
			o->type   = MOBJ_QUAD;
			o->center = mf3(q->Q);
			o->normal = mf3(q->normal);
			o->u      = mf3(q->u);
			o->v      = mf3(q->v);
			o->w      = mf3(q->w);
			o->D      = q->D;
			o->albedo = mf3(q->mat.tex.albedo);
			if (q->mat.emitted)
			{
				em = q->mat.emitted(&q->mat, 0.0f, 0.0f, q->Q);
				if (em.x > 0.0f || em.y > 0.0f || em.z > 0.0f)
				{
					o->is_emissive = 1;
					o->emission    = mf3(em);
				}
			}
		}
		else
			continue ;  /* unknown type (e.g. t_box wrapper) — skip */
		count++;
	}
	return (count);
}

/* ======================================================================== */
/*  Public: render_frame_metal()                                             */
/* ======================================================================== */

void	render_frame_metal(void *param)
{
	t_data		*data;
	uint32_t	obj_count;

	if (!s_pipeline || !s_cam_buf || !s_cnt_buf)
		return ;   /* Metal not initialised — no-op */

	data = (t_data *)param;

	/* ---- timing start ---- */
	uint64_t t0 = mach_absolute_time();

	ensure_pixel_buf(data->width, data->height);
	ensure_obj_buf((uint32_t)data->world.objects->len + 1);

	/* Upload camera data. */
	pack_cam((t_metal_cam *)s_cam_buf.contents, data);

	/* Upload scene objects. */
	obj_count = pack_objects((t_metal_obj *)s_obj_buf.contents, data);
	*(uint32_t *)s_cnt_buf.contents = obj_count;

	/* ------------------------------------------------------------------ */
	/*  Dispatch compute kernel                                             */
	/* ------------------------------------------------------------------ */
	id<MTLCommandBuffer>        cmd = [s_queue commandBuffer];
	id<MTLComputeCommandEncoder> enc = [cmd computeCommandEncoder];

	[enc setComputePipelineState:s_pipeline];
	[enc setBuffer:s_px_buf  offset:0 atIndex:0];  /* pixel output  */
	[enc setBuffer:s_cam_buf offset:0 atIndex:1];  /* camera data   */
	[enc setBuffer:s_obj_buf offset:0 atIndex:2];  /* scene objects */
	[enc setBuffer:s_cnt_buf offset:0 atIndex:3];  /* object count  */

	/*
	** Thread groups of 16×16 (= 256 threads) — a typical efficient size for
	** 2-D image kernels on Apple GPU. The kernel guards against out-of-bounds
	** pixels (gid.x >= width || gid.y >= height), so a padded grid is safe.
	*/
	MTLSize threads = MTLSizeMake(16, 16, 1);
	MTLSize grid    = MTLSizeMake(
		(data->width  + 15) / 16,
		(data->height + 15) / 16,
		1);
	[enc dispatchThreadgroups:grid threadsPerThreadgroup:threads];
	[enc endEncoding];
	[cmd commit];

	/*
	** Block until the GPU finishes. This matches the synchronous contract
	** of render_frame()/render_frame_direct() on the CPU side — by the time
	** this function returns, data->img->pixels can be updated.
	*/
	[cmd waitUntilCompleted];

	/*
	** Copy the finished RGBA pixels from the shared Metal buffer into the
	** MLX42 image pixel buffer. On M2, s_px_buf.contents and img->pixels
	** are both CPU-accessible; this is a plain memory copy (~0.1 ms for 640×360).
	*/
	{
		mlx_image_t *img = (mlx_image_t *)data->img;
		memcpy(img->pixels,
		       s_px_buf.contents,
		       (size_t)(data->width * data->height * 4));
	}

	/* ---- timing end ---- */
	{
		uint64_t t1 = mach_absolute_time();
		static mach_timebase_info_data_t tb = {0, 0};
		if (tb.denom == 0)
			mach_timebase_info(&tb);
		double ms = (double)(t1 - t0) * tb.numer / tb.denom / 1e6;
		fprintf(stderr, "[Metal] %ux%u  %u spp  depth %u  %u objs  %.1f ms\n",
			data->width, data->height,
			data->samples_per_pixel, data->max_depth,
			obj_count, ms);
	}
}


/* ======================================================================== */
/*  Public: metal_cleanup()                                                  */
/* ======================================================================== */

void	metal_cleanup(void)
{
	/*
	** Nil-ing ARC __strong statics releases the Metal objects.
	** Order: pipeline first (depends on device), device last.
	*/
	s_pipeline = nil;
	s_cnt_buf  = nil;
	s_obj_buf  = nil;
	s_cam_buf  = nil;
	s_px_buf   = nil;
	s_queue    = nil;
	s_device   = nil;
}
