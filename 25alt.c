/**
 * Advent of Code 2022
 * Day 25: Full of Hot Air
 * https://adventofcode.com/2022/day/25
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * gives a shortest runtime for my input file (not the example) of 41 µs.
 * On a Raspberry Pi 4 with the CPU in performance mode: 85 µs.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>
#include <stdlib.h>  // div
#include "startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "example25.txt"
#define N (8)
#else
#define NAME "input25.txt"
#define N (32)
#endif

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    int len = 0, i = 0, j, c, buf[N], snafu[N] = {0};
    while ((c = fgetc(f)) != -1) {
        switch (c) {
            case '=': buf[i++] = -2; break;
            case '-': buf[i++] = -1; break;
            case '0': buf[i++] =  0; break;
            case '1': buf[i++] =  1; break;
            case '2': buf[i++] =  2; break;
            default:
                if (i > len)
                    len = i;
                j = 0;
                while (i)
                    snafu[j++] += buf[--i];  // add digits to snafu sum
                break;
        }
    }
    fclose(f);

    i = 0;
    j = 1;
    while (i < len) {
        if (snafu[i] > 2 || snafu[i] < -2) {
            if (snafu[i] > 4 || snafu[i] < -4) {
                div_t t = div(snafu[i], 5);
                snafu[i] = t.rem;
                snafu[j] += t.quot;
            }
            if (snafu[i] > 2) {
                snafu[i] -= 5;
                ++snafu[j];
            } else if (snafu[i] < -2) {
                snafu[i] += 5;
                --snafu[j];
            }
            if (j == len)
                ++len;
        }
        ++i;
        ++j;
    }

    printf("Part 1: ");  // example="2=-1=0" (4890), input="2-=2-0=-0-=0200=--21"
    for (i = len - 1; i >= 0; --i)
        switch (snafu[i]) {
            case -2: printf("="); break;
            case -1: printf("-"); break;
            case  0: printf("0"); break;
            case  1: printf("1"); break;
            case  2: printf("2"); break;
        }
    printf("\n");
    printf("Time: %.0f us\n", stoptimer_us());  // M1: 41 µs, Pi4: 85 µs
    return 0;
}
