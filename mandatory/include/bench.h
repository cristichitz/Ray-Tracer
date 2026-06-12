#ifndef BENCH_H
# define BENCH_H

# include "rt_cpu.h"

/*
** Headless benchmark of the CPU render loop, instrumented with Linux
** perf_event hardware counters (a C port of the Aalto PPC perf wrapper).
** Renders `frames` frames into an off-screen buffer and prints low-level
** metrics: cycles, instructions, IPC, cache misses, branch misses, page
** faults, plus per-ray normalisations. Returns 0 on success.
*/
int	run_benchmark(t_data *data, int frames);

#endif
    