#include "rt_bonus.h"
#include <string.h>
#include <sys/stat.h>

/*
** Offline render-to-disk mode (run the bonus with --render).
**
** Interactive playback is driven by the keyboard (R) and the GPU has to keep up
** in real time, so quality is capped. For a final render we instead want to
** trade time for quality: play the whole cinematic automatically, accumulate
** many samples per frame, and write every frame to disk as a numbered image.
** A C program can't practically mux an MP4, but writing a frame sequence is
** trivial; assemble it afterwards with, e.g.:
**
**   ffmpeg -framerate 60 -i frames/frame_%05d.ppm -pix_fmt yuv420p out.mp4
**
** PPM (P6) is used because it needs no library (MLX42's bundled lodepng ships
** without its encoder). Files are big but ffmpeg compresses them away.
*/

/* True if --render appears anywhere on the command line. */
int  render_mode_on(int argc, char **argv)
{
  int  i;

  i = 1;
  while (i < argc)
    if (!strcmp(argv[i++], "--render"))
      return (1);
  return (0);
}

/* Write the current image as frames/frame_NNNNN.ppm (RGB, alpha dropped). */
static void  save_frame_ppm(t_data *data, int n)
{
  char     path[64];
  FILE     *f;
  uint8_t  *px;
  int      i;
  int      total;

  snprintf(path, sizeof(path), "frames/frame_%05d.ppm", n);
  f = fopen(path, "wb");
  if (!f)
  {
    perror("render: fopen");
    return ;
  }
  fprintf(f, "P6\n%d %d\n255\n", data->frame.width, data->frame.height);
  px = data->img->pixels;
  total = data->frame.width * data->frame.height;
  i = 0;
  while (i < total)
    fwrite(&px[i++ * 4], 1, 3, f);
  fclose(f);
}

/* The cinematic is over once nothing is animating and the pile has settled. */
static int  animation_done(t_data *data)
{
  t_rubik  *r;

  r = &data->rubik;
  return (!r->explode_active && !r->orbit_active && !r->active
    && r->q_count == 0 && !data->phys.running);
}

/*
** One offline frame: advance the animation a single step, accumulate RENDER_SPP
** samples into a converged image, and write it out. The window still mirrors
** each frame as a live preview. Exits once the animation finishes.
*/
void  render_loop(void *param)
{
  t_data  *data;
  int      s;

  data = (t_data *)param;
  if (!data->render_started)
  {
    mkdir("frames", 0755);
    start_explode(data);
    data->render_started = 1;
  }
  step_rubik(data);
  physics_step(data);
  update_stage(data);
  update_view(data);
  s = -1;
  while (++s < RENDER_SPP)
  {
    data->frame_index = s;
    render_frame(data);
  }
  save_frame_ppm(data, data->render_frame_no++);
  printf("render: wrote frame %d\n", data->render_frame_no - 1);
  if (animation_done(data) || data->render_frame_no >= RENDER_MAX_FRAMES)
  {
    printf("render: done, %d frames in frames/\n", data->render_frame_no);
    mlx_close_window(data->mlx);
  }
}
