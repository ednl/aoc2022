/**
 * Advent of Code 2022
 * Day 10: Cathode-Ray Tube
 * https://adventofcode.com/2022/day/10
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, fgetc, fscanf, printf
#include <stdlib.h>  // abs, div

#define CLOCK_OFF (20)
#define CLOCK_MOD (40)
#define CLOCK_Q1   (1)
#define CLOCK_Q2   (6)
#define VERT       (6)
#define HORZ      (40)
#define PIXELS (VERT * HORZ)

static int cycle = CLOCK_OFF, regX = 1, sigsum = 0, pixel = 0;

static void tick(void)
{
    ++cycle;
    div_t poi = div(cycle, CLOCK_MOD);
    if (!poi.rem && poi.quot >= CLOCK_Q1 && poi.quot <= CLOCK_Q2)
        sigsum += (cycle - CLOCK_OFF) * regX;  // for cycle=[20,60,100,140,180,220]
    div_t beam = div(pixel, HORZ);
    if (beam.quot < VERT) {
        if (!beam.rem)
            printf("\n");
        printf("%c", abs(beam.rem - regX) <= 1 ? '#' : '.');
    }
    ++pixel;
}

int main(void)
{
    FILE *f = fopen("input10.txt", "r");  // or: example10.txt
    if (!f)
        return 1;
    int a;
    char buf[8];
    while (pixel < PIXELS && !feof(f)) {
        tick();
        switch (fgetc(f)) {
            case 'n': fscanf(f, "%s ", buf); break;
            case 'a':
                tick();
                if (fscanf(f, "%s %d ", buf, &a) == 2)
                    regX += a;
                break;
        }
    }
    fclose(f);
    printf("\n\nPart 1: %d\n", sigsum);  // ex=13140, inp=15020, Part 2: EFUGLPAP
    return 0;
}
