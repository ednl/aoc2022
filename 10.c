/**
 * Advent of Code 2022
 * Day 10: Cathode-Ray Tube
 * https://adventofcode.com/2022/day/9
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, fgetc, fscanf, printf
#include <stdlib.h>  // abs, div

#define SIG_OFF (20)
#define SIG_MOD (40)
#define SIG_Q1   (1)
#define SIG_Q2   (6)
#define VERT     (6)
#define HORZ    (40)
#define PIXELS (VERT * HORZ)

static void tick(int *const cycle, const int X, int *const sigsum, int *const pixel)
{
    ++*cycle;
    // Part 1
    div_t dial = div(*cycle + SIG_OFF, SIG_MOD);  // 20th, 60th, 100th, 140th, 180th, and 220th cycles
    if (!dial.rem && dial.quot >= SIG_Q1 && dial.quot <= SIG_Q2)
        *sigsum += *cycle * X;
    // Part 2
    div_t beam = div(*pixel, HORZ);
    if (beam.quot < VERT) {
        if (!beam.rem)
            printf("\n");
        printf("%c", abs(beam.rem - X) <= 1 ? '#' : '.');
    }
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
    printf("\n\nPart 1: %d\n", sigsum);  // ex=13140, inp=15020
    // Part 2: EFUGLPAP
    return 0;
}
