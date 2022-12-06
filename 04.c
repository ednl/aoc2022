/**
 * Advent of Code 2022
 * Day 4: Camp Cleanup
 * https://adventofcode.com/2022/day/4
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, fscanf, printf
#include <stdlib.h>  // atoi

int main(void)
{
    int a, b, c, d, part1 = 0, part2 = 0;
    FILE *f = fopen("input04.txt", "r");
    while (fscanf(f, "%d-%d,%d-%d", &a, &b, &c, &d) == 4)
        if (a <= d && b >= c) {
            ++part2;
            part1 += (a >= c && b <= d) || (a <= c && b >= d);
        }
    fclose(f);
    printf("%d\n%d\n", part1, part2);  // 584 933
    return 0;
}
