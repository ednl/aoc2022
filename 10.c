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
#define LINE (HALF << 1)  // horizontal pixels = 40

int main(void)
{
    starttimer();
    FILE *f = fopen("aocinput/2022-10-input.txt", "r");          // or: example10.txt
    char buf[5];                                  // opcodes and operands are length <= 4, +\0
    int clock = 0, regX = 1, sigsum = 0, beam;    // beam = horizontal position
    while (fscanf(f, "%4s ", buf) == 1) {         // 1 clock tick for every word (so, 2 for addx)
        if ((beam = clock++ % LINE) == HALF - 1)  // beam 19 => clock 20
            sigsum += clock * regX;               // clock = 20,60,100,140,180,220th cycle
        if (!beam)                                // start with blank line
            printf("\n");
        printf("%c", abs(beam - regX) <= 1 ? '#' : ' ');  // part 2: EFUGLPAP
        if (!(buf[0] & (1 << 6)))                 // ascii < 64 for numbers (incl negative sign)
            regX += atoi(buf);                    // only update after complete instruction cycle
    }
    fclose(f);
    printf("\n\nPart 1: %d\n", sigsum);           // part 1: ex=13140, inp=15020
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
