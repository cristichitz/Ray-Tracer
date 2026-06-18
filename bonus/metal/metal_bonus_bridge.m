/* ************************************************************************** */
/*                                                                            */
/*   metal_bonus_bridge.m                                                     */
/*                                                                            */
/*   Objective-C Metal bridge for the bonus GPU path tracer.                  */
/*   Replaces bonus/src/gpu and bonus/src/cl (OpenCL) with Metal.            */
/*                                                                            */
/* ************************************************************************** */

#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include "rt_bonus.h"

/* check_null is defined in cl_util_bonus.c which is excluded from this build */
void	check_null(const void *ptr)
{
	if (ptr == NULL)
	{
		printf("NULL pointer error\n");
		exit(EXIT_FAILURE);
	}
}

/* ------------------------------------------------------------------ */
/*  Uniform block – must match the Metal shader's t_uniforms          */
/* ------------------------------------------------------------------ */

typedef struct s_metal_uniforms
{
	t_image		frame;
	int			obj_count;
	int			frame_index;
	int			nnodes;
	int			plane_first;
	int			plane_count;
}				t_metal_uniforms;

/* ------------------------------------------------------------------ */
/*  Static Metal state – ARC-managed, lives for the process lifetime  */
/* ------------------------------------------------------------------ */

static id<MTLDevice>               g_device;
static id<MTLCommandQueue>         g_queue;
static id<MTLComputePipelineState> g_pipeline;

/* GPU buffers */
static id<MTLBuffer>               g_pixel_buf;    /* uchar4 per pixel   */
static id<MTLBuffer>               g_accum_buf;    /* float4 per pixel   */
static id<MTLBuffer>               g_obj_buf;      /* t_object[]         */
static id<MTLBuffer>               g_node_buf;     /* t_bvh_node[]       */
static id<MTLBuffer>               g_prim_buf;     /* int[] prim indices */
static id<MTLBuffer>               g_uniform_buf;  /* t_metal_uniforms   */

/* ------------------------------------------------------------------ */
/*  Helpers                                                           */
/* ------------------------------------------------------------------ */

static void	alloc_pixel_buffers(int w, int h)
{
	size_t	pixel_count;

	pixel_count = (size_t)w * h;
	g_pixel_buf = [g_device newBufferWithLength:pixel_count * 4
		options:MTLResourceStorageModeShared];
	g_accum_buf = [g_device newBufferWithLength:pixel_count * sizeof(float) * 4
		options:MTLResourceStorageModeShared];
}

/* ------------------------------------------------------------------ */
/*  Public API                                                        */
/* ------------------------------------------------------------------ */

int	metal_bonus_init(t_data *data)
{
	NSError		*error;
	NSString	*path;
	NSURL		*url;
	id<MTLLibrary>	lib;

	g_device = MTLCreateSystemDefaultDevice();
	if (!g_device)
		return (0);
	fprintf(stderr, "[Metal-Bonus] Initialized on: %s\n",
		[[g_device name] UTF8String]);
	g_queue = [g_device newCommandQueue];
	path = [[NSBundle mainBundle] pathForResource:@"render_bonus"
		ofType:@"metallib"];
	if (!path)
		path = @"bonus/metal/render_bonus.metallib";
	url = [NSURL fileURLWithPath:path];
	lib = [g_device newLibraryWithURL:url error:&error];
	if (!lib)
	{
		fprintf(stderr, "[Metal-Bonus] Failed to load metallib: %s\n",
			[[error localizedDescription] UTF8String]);
		return (0);
	}
	id<MTLFunction> fn = [lib newFunctionWithName:@"render_kernel"];
	if (!fn)
	{
		fprintf(stderr, "[Metal-Bonus] render_kernel not found\n");
		return (0);
	}
	g_pipeline = [g_device newComputePipelineStateWithFunction:fn error:&error];
	if (!g_pipeline)
	{
		fprintf(stderr, "[Metal-Bonus] Pipeline error: %s\n",
			[[error localizedDescription] UTF8String]);
		return (0);
	}
	alloc_pixel_buffers(data->frame.width, data->frame.height);
	g_uniform_buf = [g_device newBufferWithLength:sizeof(t_metal_uniforms)
		options:MTLResourceStorageModeShared];
	return (1);
}

void	metal_bonus_cleanup(t_data *data)
{
	(void)data;
	g_pixel_buf   = nil;
	g_accum_buf   = nil;
	g_obj_buf     = nil;
	g_node_buf    = nil;
	g_prim_buf    = nil;
	g_uniform_buf = nil;
	g_pipeline    = nil;
	g_queue       = nil;
	g_device      = nil;
}

void	metal_bonus_resize(t_data *data)
{
	g_pixel_buf = nil;
	g_accum_buf = nil;
	alloc_pixel_buffers(data->frame.width, data->frame.height);
}

/* Upload scene objects + BVH to Metal buffers */
void	metal_bonus_upload_scene(t_data *data)
{
	size_t	obj_size;
	size_t	node_size;
	size_t	prim_size;

	obj_size = sizeof(t_object) * data->obj_count;
	g_obj_buf = [g_device newBufferWithBytes:data->objects
		length:obj_size options:MTLResourceStorageModeShared];
	node_size = sizeof(t_bvh_node) * (size_t)(data->bvh.nnodes + 1);
	g_node_buf = [g_device newBufferWithBytes:data->bvh.nodes
		length:node_size options:MTLResourceStorageModeShared];
	prim_size = sizeof(int) * (size_t)(data->obj_count + 1);
	g_prim_buf = [g_device newBufferWithBytes:data->bvh.prim
		length:prim_size options:MTLResourceStorageModeShared];
}

void	metal_bonus_render(t_data *data)
{
	t_metal_uniforms	*uni;
	MTLSize				grid;
	MTLSize				group;
	NSUInteger			tw;
	NSUInteger			th;

	if (!g_obj_buf)
		metal_bonus_upload_scene(data);
	if (data->scene_dirty)
	{
		bvh_build(data);
		metal_bonus_upload_scene(data);
		data->scene_dirty = 0;
	}
	/* Fill uniforms */
	uni = (t_metal_uniforms *)[g_uniform_buf contents];
	uni->frame       = data->frame;
	uni->obj_count   = (int)data->obj_count;
	uni->frame_index = data->frame_index;
	uni->nnodes      = data->bvh.nnodes;
	uni->plane_first = data->bvh.nprim;
	uni->plane_count = data->bvh.plane_count;
	/* Dispatch */
	id<MTLCommandBuffer> cmd = [g_queue commandBuffer];
	id<MTLComputeCommandEncoder> enc = [cmd computeCommandEncoder];
	[enc setComputePipelineState:g_pipeline];
	[enc setBuffer:g_pixel_buf   offset:0 atIndex:0];
	[enc setBuffer:g_obj_buf     offset:0 atIndex:1];
	[enc setBuffer:g_accum_buf   offset:0 atIndex:2];
	[enc setBuffer:g_node_buf    offset:0 atIndex:3];
	[enc setBuffer:g_prim_buf    offset:0 atIndex:4];
	[enc setBuffer:g_uniform_buf offset:0 atIndex:5];
	tw = g_pipeline.threadExecutionWidth;
	th = g_pipeline.maxTotalThreadsPerThreadgroup / tw;
	grid = MTLSizeMake(data->frame.width, data->frame.height, 1);
	group = MTLSizeMake(tw, th, 1);
	[enc dispatchThreads:grid threadsPerThreadgroup:group];
	[enc endEncoding];
	[cmd commit];
	[cmd waitUntilCompleted];
	/* Copy pixels to MLX image */
	memcpy(data->img->pixels, [g_pixel_buf contents],
		(size_t)data->frame.width * data->frame.height * 4);
}

/* ------------------------------------------------------------------ */
/*  GPU wrappers that the rest of the bonus code calls                */
/* ------------------------------------------------------------------ */

int	init_gpu(t_data *data)
{
	if (!metal_bonus_init(data))
	{
		printf("No Metal device detected.\n");
		exit(EXIT_FAILURE);
	}
	bvh_build(data);
	metal_bonus_upload_scene(data);
	return (0);
}

int	init_objects(t_data *data)
{
	metal_bonus_upload_scene(data);
	return (0);
}

void	render_frame(t_data *data)
{
	metal_bonus_render(data);
}

void	clean_gpu(t_data *data)
{
	metal_bonus_cleanup(data);
}

void	resize_gpu_buffers(t_data *data)
{
	metal_bonus_resize(data);
}

void	cleanup(void *param)
{
	t_data	*data;

	data = (t_data *)param;
	clean_gpu(data);
	free(data->objects);
	data->objects = NULL;
	free(data->bvh.nodes);
	data->bvh.nodes = NULL;
	free(data->bvh.prim);
	data->bvh.prim = NULL;
	printf("Cleaning up and exiting...\n");
}
