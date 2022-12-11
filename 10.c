/**
 * Advent of Code 2022
 * Day 10: Cathode-Ray Tube
 * https://adventofcode.com/2022/day/10
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, fscanf, printf
#include <stdlib.h>  // abs
#include "startstoptimer.h"

#define HALF (20)         // half horizontal scanline width
#define FULL (HALF << 1)  // horizontal pixels = 40

int main(void)
{
    starttimer();
    FILE *f = fopen("input10.txt", "r");      // or: example10.txt
    char buf[5];                              // opcodes are length 4 + 1 string terminator \0
    int cycle = HALF, regX = 1, sigsum = 0, pixel = 0, beam;
    while (fscanf(f, "%4s ", buf) == 1) {
        if (!(++cycle % FULL))                // 1 tick for every word => noop=1, addx=2
            sigsum += (cycle - HALF) * regX;  // clock=20,60,100,140,180,220
        if (!(beam = pixel++ % FULL))         // horizontal position of the pixel beam
            printf("\n");                     // start with blank line
        printf("%c", abs(beam - regX) <= 1 ? '#' : ' ');  // part 2: EFUGLPAP
        if (!(buf[0] & (1 << 6)))             // ch < 64 for numbers (also negative sign)
            regX += atoi(buf);                // only update after complete instruction cycle
    }
    fclose(f);
    printf("\n\nPart 1: %d\n", sigsum);       // part 1: ex=13140, inp=15020
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
