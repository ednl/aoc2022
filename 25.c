/**
 * Advent of Code 2022
 * Day 25: Full of Hot Air
 * https://adventofcode.com/2022/day/25
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
#include <string.h>

#define EXAMPLE 1
#if EXAMPLE == 1
#define NAME "example25.txt"
#else
#define NAME "input25.txt"
#endif

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    int i = 0, c, buf[32];
    int snafu[32] = {0};
    while ((c = fgetc(f)) != -1) {
        switch (c) {
            case '=': c = -2; break;
            case '-': c = -1; break;
            case '0': c =  0; break;
            case '1': c =  1; break;
            case '2': c =  2; break;
        }
        if (c <= 2) {
            buf[i++] = c;
        } else {  // newline
            int j = 0;
            while (i) {
                snafu[j++] += buf[--i];
            }
        }
    }
    fclose(f);
    int part1 = 0, p = 1;
    for (i = 0; i < 6; ++i) {
        printf("%2d: %4d x %2d = %4d\n", i, p, snafu[i], snafu[i] * p);
        part1 += snafu[i] * p;
        p *= 5;
    }
    printf("Part 1: %d\n", part1);  // example=2=-1=0 (4890), input=?

    return 0;
}
