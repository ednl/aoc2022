/**
 * Advent of Code 2022
 * Day 10: Cathode-Ray Tube
 * https://adventofcode.com/2022/day/9
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, fgetc, fscanf, printf
#include <stdlib.h>  // abs, div

#define VERT  (6)
#define HORZ (40)
#define PIXELS (VERT * HORZ)
static char screen[VERT][HORZ];

static void tick(int *const cycle, const int X, int *const sigsum, int *const pixel)
{
    ++*cycle;
    if ((*cycle + 20) % 40 == 0)  // 20th, 60th, 100th, 140th, 180th, and 220th cycles
        *sigsum += *cycle * X;
    div_t beam = div(*pixel, HORZ);
    if (beam.quot < VERT)
        screen[beam.quot][beam.rem] = abs(beam.rem - X) <= 1 ? '#' : '.';
    ++*pixel;
}

int main(void)
{
    // FILE *f = fopen("example10.txt", "r");
    FILE *f = fopen("input10.txt", "r");
    if (!f)
        return 1;
    int cycle = 0, X = 1, sigsum = 0, pixel = 0, a;
    char buf[8];
    while (pixel < PIXELS && !feof(f)) {
        tick(&cycle, X, &sigsum, &pixel);
        switch (fgetc(f)) {
            case 'n': fscanf(f, "%s ", buf); break;
            case 'a':
                tick(&cycle, X, &sigsum, &pixel);
                if (fscanf(f, "%s %d ", buf, &a) == 2)
                    X += a;
                break;
        }
    }
    fclose(f);
    printf("Part 1: %d\nPart 2:\n", sigsum);  // ex=13140, inp=15020
    for (int i = 0; i < VERT; ++i) {          // EFUGLPAP
        for (int j = 0; j < HORZ; ++j)
            printf("%c", screen[i][j]);
        printf("\n");
    }
    return 0;
}
