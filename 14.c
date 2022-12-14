/**
 * Advent of Code 2022
 * Day 14: Regolith Reservoir
 * https://adventofcode.com/2022/day/14
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   for((i=0;i<50;++i));do ./a.out>/dev/null;done;for((i=0;i<10;++i));do ./a.out|tail -n1|awk '{print $2}';done|sort -n|head -n1
 * gives a shortest runtime for my input file (not the example) of [TBD] us.
 * On a Raspberry Pi 4 with the CPU in performance mode: [TBD] ms.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void)
{
    int n = 0, x, y, xmin = 500, xmax = 500, ymin = 500, ymax = 0;
    FILE* f = fopen("input14.txt", "r");
    while (!feof(f) && fscanf(f, "%d,%d", &x, &y) == 2) {
        ++n;
        if (x < xmin) xmin = x;
        if (x > xmax) xmax = x;
        if (y < ymin) ymin = y;
        if (y > ymax) ymax = y;
        int c = fgetc(f);
        if (c == ' ')
            for (int i = 0; i < 3; ++i)
                fgetc(f);
    }
    printf("n=%d x=%d-%d y=%d-%d\n", n, xmin, xmax, ymin, ymax);
    int x0 = xmin - 1, dx = xmax - xmin + 3, dy = ymax + 2;  // y0=0
    bool* cave = calloc((size_t)(dx * dy), sizeof(bool));
    rewind(f);
    //TODO
    fclose(f);
    free(cave);
    return 0;
}
