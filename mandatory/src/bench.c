/*
** bench.c - headless benchmark + perf_event_open instrumentation.
**
** A C port of the Aalto PPC profiler (perf.h + profile.py): open a group of
** hardware/software performance counters, enable them around the hot loop,
** read scaled totals afterwards, then print a human-readable analysis using
** the exact wording and number formatting of the PPC grader. The ray tracer's
** "useful work" unit is the ray (analog of PPC's arithmetic operations).
**
** Counters that cannot be measured (e.g. when perf_event_open is blocked by
** perf_event_paranoid) simply have their bullet dropped, just like PPC.
*/

#define _GNU_SOURCE
#include "bench.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#ifdef __linux__
# include <linux/perf_event.h>
# include <sys/ioctl.h>
# include <sys/syscall.h>
# include <asm/unistd.h>
#endif

/* Total number of get_ray_color() invocations, i.e. rays cast (incl. bounces).
** Incremented in render_cpu2.c; reset by the benchmark before timing. */
unsigned long long	g_ray_count = 0;

#define MAX_EVENTS 8

typedef struct s_perf
{
	int			n;
	int			fds[MAX_EVENTS];
	uint64_t	ids[MAX_EVENTS];
	const char	*names[MAX_EVENTS];
	uint64_t	vals[MAX_EVENTS];
	double		scale;
	int			available;
}	t_perf;

static double	now_ms(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6);
}

/* ====================================================================== */
/*  perf_event_open machinery (Linux only)                                */
/* ====================================================================== */

#ifdef __linux__
static long	perf_open(struct perf_event_attr *attr, int group_fd)
{
	return (syscall(__NR_perf_event_open, attr, 0, -1, group_fd, 0UL));
}

/* Adds one counter to the group. The first counter becomes the group leader;
** all others are subordinate so a single read() returns the whole group. */
static int	add_event(t_perf *p, uint32_t type, uint64_t config,
		const char *name)
{
	struct perf_event_attr	attr;
	int						fd;

	if (p->n >= MAX_EVENTS)
		return (-1);
	memset(&attr, 0, sizeof(attr));
	attr.size = sizeof(attr);
	attr.type = type;
	attr.config = config;
	attr.disabled = (p->n == 0);
	attr.exclude_kernel = 1;
	attr.exclude_hv = 1;
	if (p->n == 0)
		attr.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID
			| PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;
	fd = (int)perf_open(&attr, p->n == 0 ? -1 : p->fds[0]);
	if (fd == -1)
		return (-1);
	ioctl(fd, PERF_EVENT_IOC_ID, &p->ids[p->n]);
	p->fds[p->n] = fd;
	p->names[p->n] = name;
	p->n++;
	return (0);
}

static void	perf_init(t_perf *p)
{
	memset(p, 0, sizeof(*p));
	p->scale = 1.0;
	/* Leader must succeed; if not, perf is blocked (paranoid/permissions). */
	if (add_event(p, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES,
			"cycles") != 0)
		return ;
	add_event(p, PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS,
		"instructions");
	add_event(p, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES,
		"cache_refs");
	add_event(p, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES,
		"cache_misses");
	add_event(p, PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
		"branches");
	add_event(p, PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES,
		"branch_misses");
	add_event(p, PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CPU_MIGRATIONS,
		"cpu_migrations");
	p->available = 1;
}

static void	perf_start(t_perf *p)
{
	if (!p->available)
		return ;
	ioctl(p->fds[0], PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
	ioctl(p->fds[0], PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
}

static void	perf_stop(t_perf *p)
{
	if (p->available)
		ioctl(p->fds[0], PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
}

/* Reads the whole group in one go. If counters were time-multiplexed onto a
** limited number of hardware registers, time_enabled/time_running gives the
** scaling factor to estimate the full count. */
static void	perf_read(t_perf *p)
{
	uint64_t	buf[3 + 2 * MAX_EVENTS];
	uint64_t	nr;
	uint64_t	id;
	size_t		i;
	int			j;

	if (!p->available)
		return ;
	if (read(p->fds[0], buf, sizeof(buf)) <= 0)
	{
		p->available = 0;
		return ;
	}
	nr = buf[0];
	p->scale = (buf[2] > 0) ? (double)buf[1] / (double)buf[2] : 1.0;
	i = 0;
	while (i < nr)
	{
		id = buf[3 + 2 * i + 1];
		j = 0;
		while (j < p->n)
		{
			if (p->ids[j] == id)
				p->vals[j] = buf[3 + 2 * i];
			j++;
		}
		i++;
	}
}

static void	perf_close(t_perf *p)
{
	int	i;

	i = 0;
	while (i < p->n)
		close(p->fds[i++]);
}
#endif

static double	perf_get(t_perf *p, const char *name)
{
	int	i;

	i = 0;
	while (i < p->n)
	{
		if (strcmp(p->names[i], name) == 0)
			return ((double)p->vals[i] * p->scale);
		i++;
	}
	return (0.0);
}

/* ====================================================================== */
/*  Number formatting (ports of ppcgrader/quantity.py + info_utils.py)    */
/* ====================================================================== */

/* Rotating scratch buffers so several formatters can be used in one printf. */
static char	*nextbuf(void)
{
	static char	pool[12][64];
	static int	idx = 0;
	char		*b;

	b = pool[idx];
	idx = (idx + 1) % 12;
	return (b);
}

/* readable(): small numbers keep significant digits, big ones round. */
static void	rd_into(char *b, double x)
{
	double	ax;
	int		i;
	char	t[64];

	ax = fabs(x);
	if (ax >= 100.0)
		snprintf(b, 64, "%.0f", x);
	else if (ax >= 10.0)
		snprintf(b, 64, "%.1f", x);
	else
	{
		i = 0;
		if (x != 0.0)
			while (1)
			{
				snprintf(t, sizeof(t), "%.*f", i, x);
				if (atof(t) != 0.0)
					break ;
				i++;
			}
		snprintf(b, 64, "%.*f", i + 2, x);
	}
}

/* format_verbal_prefix(): million / billion / thousand. */
static const char	*fv(double x)
{
	char		*b;
	char		num[48];
	const char	*suf;
	double		v;

	b = nextbuf();
	suf = "";
	v = x;
	if (x > 1e9)
		(v = x / 1e9, suf = " billion");
	else if (x > 1e6)
		(v = x / 1e6, suf = " million");
	else if (x > 1e5)
		(v = x / 1e3, suf = " thousand");
	rd_into(num, v);
	snprintf(b, 64, "%s%s", num, suf);
	return (b);
}

/* Verbal prefix with a fixed number of decimals (used for thread count). */
static const char	*fvp(double x, int prec)
{
	char	*b;

	b = nextbuf();
	snprintf(b, 64, "%.*f", prec, x);
	return (b);
}

static const char	*fpct(double x)
{
	char	*b;
	char	num[48];

	b = nextbuf();
	rd_into(num, x);
	snprintf(b, 64, "%s%%", num);
	return (b);
}

static const char	*fhz(double x)
{
	char	*b;
	char	num[48];

	b = nextbuf();
	rd_into(num, x / 1e9);
	snprintf(b, 64, "%s GHz", num);
	return (b);
}

static const char	*fsec(double x)
{
	char	*b;

	b = nextbuf();
	snprintf(b, 64, "%.3f sec", x);
	return (b);
}

/* format_binary_prefix(): KiB / MiB / GiB (unit is "B" or "B/s"). */
static const char	*fbin(double x, const char *unit)
{
	char		*b;
	char		num[48];
	const char	*pfx;
	double		v;

	b = nextbuf();
	pfx = "";
	v = x;
	if (x > 10.0 * 1073741824.0)
		(v = x / 1073741824.0, pfx = "Gi");
	else if (x > 10.0 * 1048576.0)
		(v = x / 1048576.0, pfx = "Mi");
	else if (x > 10.0 * 1024.0)
		(v = x / 1024.0, pfx = "Ki");
	rd_into(num, v);
	snprintf(b, 64, "%s %s%s", num, pfx, unit);
	return (b);
}

/* ====================================================================== */
/*  Report (ports of ppcgrader/profile.py explain_* functions)            */
/* ====================================================================== */

static double	tv_sec(struct timeval a, struct timeval b)
{
	return ((double)(b.tv_sec - a.tv_sec)
		+ (double)(b.tv_usec - a.tv_usec) / 1e6);
}

static void	report_perf(t_perf *p, double wall, double cpu, double rays)
{
	double	cyc;
	double	ins;
	double	refs;
	double	miss;

	cyc = perf_get(p, "cycles");
	ins = perf_get(p, "instructions");
	printf(" · The total number of clock cycles was %s\n", fv(cyc));
	printf("   → CPU was running at %s.\n\n", fhz(cpu > 0 ? cyc / cpu : 0));
	printf(" · The CPU executed %s machine-language instructions\n",
		fv(ins));
	printf("   ≈ %s instructions per nanosecond (wallclock)\n",
		fv(ins / (wall * 1e9)));
	printf("   ≈ %s instructions per nanosecond (CPU time)\n",
		fv(ins / (cpu * 1e9)));
	printf("   ≈ %s instructions per clock cycle.\n", fv(ins / cyc));
	printf("   → It seems you used %s machine language instructions\n",
		fv(ins / rays));
	printf("     per ray.\n\n");
	printf(" · %s of the instructions were branches\n",
		fpct(100.0 * perf_get(p, "branches") / ins));
	printf("   and %s of them were mispredicted.\n\n",
		fpct(100.0 * perf_get(p, "branch_misses") / perf_get(p, "branches")));
	refs = perf_get(p, "cache_refs");
	miss = perf_get(p, "cache_misses");
	printf(" · Your code read %s times from last-level cache.\n", fv(refs));
	printf("   The miss rate was %s.\n", fpct(100.0 * miss / refs));
	printf("   ≈ %s ≈ %s of data transfer between LLC and L2.\n",
		fbin(refs * 64.0, "B"), fbin(refs * 64.0 / wall, "B/s"));
	printf("   ≈ %s ≈ %s of data transfer between RAM and LLC.\n\n",
		fbin(miss * 64.0, "B"), fbin(miss * 64.0 / wall, "B/s"));
}

typedef struct s_runinfo
{
	double			wall_ms;
	double			fmin_ms;
	struct rusage	r0;
	struct rusage	r1;
}	t_runinfo;

static void	report_tail(t_data *d, t_perf *p, int frames, t_runinfo *ri)
{
	double	wall;
	long	ctx;
	long	pf;

	wall = ri->wall_ms / 1000.0;
	ctx = (ri->r1.ru_nvcsw - ri->r0.ru_nvcsw)
		+ (ri->r1.ru_nivcsw - ri->r0.ru_nivcsw);
	pf = (ri->r1.ru_minflt - ri->r0.ru_minflt)
		+ (ri->r1.ru_majflt - ri->r0.ru_majflt);
	printf(" · During the runtime of the program,\n");
	if (p->available)
		printf("   %s CPU migrations (≈%s per sec) and\n",
			fv(perf_get(p, "cpu_migrations")),
			fv(perf_get(p, "cpu_migrations") / wall));
	printf("   %ld context switches (≈%s per sec) occurred.\n\n",
		ctx, fv((double)ctx / wall));
	printf(" · Your program experienced %ld page faults,\n", pf);
	printf("   with %s faults per second.\n\n", fv((double)pf / wall));
	if (!p->available)
	{
		printf(" · Hardware counters were unavailable, so cycles, "
			"instructions and\n   cache statistics are not shown. "
			"Unlock them with:\n");
		printf("   sudo sh -c \"echo 1 > "
			"/proc/sys/kernel/perf_event_paranoid\"\n");
	}
	(void)d;
	(void)frames;
}

static void	report_all(t_data *d, t_perf *p, int frames, t_runinfo *ri)
{
	double	wall;
	double	cpu;
	double	rays;
	double	primary;

	wall = ri->wall_ms / 1000.0;
	cpu = tv_sec(ri->r0.ru_utime, ri->r1.ru_utime)
		+ tv_sec(ri->r0.ru_stime, ri->r1.ru_stime);
	rays = (double)g_ray_count;
	primary = (double)d->width * d->height * d->samples_per_pixel * frames;
	printf("\nminiRT benchmark — %d frames @ %ux%u, spp=%u, depth=%u\n",
		frames, d->width, d->height, d->samples_per_pixel, d->max_depth);
	printf("avg %.1f ms/frame · %.2f fps (fastest %.1f ms)\n\n",
		ri->wall_ms / frames, 1000.0 * frames / ri->wall_ms, ri->fmin_ms);
	if (wall < 0.001)
		return ((void)printf(" · This benchmark took just a fraction of "
				"a millisecond, so it does not make sense to try to show "
				"more detailed statistics.\n"));
	printf(" · This benchmark cast %s rays in total\n", fv(rays));
	printf("   (%s of them primary camera rays).\n", fv(primary));
	printf("   → It seems you managed to perform at least\n");
	printf("   ≈ %s rays per second.\n\n", fv(rays / wall));
	printf(" · Your code used %s of wallclock time, and %s of CPU time\n",
		fsec(wall), fsec(cpu));
	printf("   → you used %s simultaneous hardware threads on average.\n\n",
		fvp(wall > 0 ? cpu / wall : 0, 1));
	if (p->available)
		report_perf(p, wall, cpu, rays);
	report_tail(d, p, frames, ri);
}

/* ====================================================================== */
/*  Benchmark driver                                                      */
/* ====================================================================== */

int	run_benchmark(t_data *data, int frames)
{
	t_perf		perf;
	t_runinfo	ri;
	double		t0;
	double		s;
	int			i;

	if (frames <= 0)
		frames = 30;
	data->headless = true;
	data->fb = malloc(sizeof(uint32_t) * data->width * data->height);
	if (!data->fb)
		return (print_error("benchmark: out of memory"), 1);
	memset(&perf, 0, sizeof(perf));
	render_frame(data);
	g_ray_count = 0;
#ifdef __linux__
	perf_init(&perf);
#endif
	getrusage(RUSAGE_SELF, &ri.r0);
#ifdef __linux__
	perf_start(&perf);
#endif
	t0 = now_ms();
	ri.fmin_ms = 1e30;
	i = 0;
	while (i++ < frames)
	{
		s = now_ms();
		render_frame(data);
		if (now_ms() - s < ri.fmin_ms)
			ri.fmin_ms = now_ms() - s;
	}
	ri.wall_ms = now_ms() - t0;
#ifdef __linux__
	perf_stop(&perf);
	perf_read(&perf);
#endif
	getrusage(RUSAGE_SELF, &ri.r1);
	report_all(data, &perf, frames, &ri);
#ifdef __linux__
	perf_close(&perf);
#endif
	free(data->fb);
	return (0);
}
