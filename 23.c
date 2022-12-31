/**
 * Advent of Code 2022
 * Day 23: Unstable Diffusion
 * https://adventofcode.com/2022/day/23
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * gives a shortest runtime for my input file (not the example) of [TBD] µs.
 * On a Raspberry Pi 4 with the CPU in performance mode: [TBD] µs.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>

#define EXAMPLE 1
#if EXAMPLE == 1
#define NAME "aocinput/2022-23-example.txt"
#else
#define NAME "aocinput/2022-23-input.txt"
#endif

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;

    fclose(f);

    int part1 = 0;
    printf("Part 1: %d\n", part1);  // example=110, input=?

    return 0;
}
