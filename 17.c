#include <stdio.h>
#include <stdint.h>  // int8_t
#include <stdbool.h>

#define shr(x) ((x) >>= 1)  // bitshift right by 1
#define shl(x) ((x) <<= 1)  // bitshift left by 1

#define RJET   (2)      // right jet, bit 2 selects for right angle bracket: ord('<')=60, ord('>')=62, 60^62=2
#define ROCKS  (5)      // number of different falling rocks
#define WIDTH  (7)      // chamber width (=playing field width)
#define HEIGHT (4048)   // chamber width (=playing field width)
#define LEDGE  (1 << 6) // left edge, last valid position on the left (chamber wall is at bit 7)
#define REDGE  (1 >> 0) // right edge, last valid position on the right (chamber wall is at bit -1)
#define JETS   (10091)  // number of brackets in input file
#define PART1  (2022)   // number of rocks falling down in part 1

static const int8_t rock[ROCKS][4] = {
    {036},  // |..@@@@. = 00 011 110

    {010,   // |...@... = 00 001 000
     034,   // |..@@@.. = 00 011 100
     010},  // |...@... = 00 001 000

    {034,   // |..@@@.. = 00 011 100  // leading edge when falling
     004,   // |....@.. = 00 000 100
     004},  // |....@.. = 00 000 100

    {020,   // |..@.... = 00 010 000
     020,   // |..@.... = 00 010 000
     020,   // |..@.... = 00 010 000
     020},  // |..@.... = 00 010 000

    {030,   // |..@@... = 00 011 000
     030},  // |..@@... = 00 011 000

};

static const int height[ROCKS] = {1, 3, 3, 4, 2};
static int top[WIDTH];  // highest rock in the chamber per vertical line
static int highest;     // highest rock in the chamber overall
static bool rjet[JETS];
static int8_t chamber[4048];

int main(void)
{
    FILE* f = fopen("input17.txt", "r");
    if (!f)
        return 1;
    for (int i = 0; i < JETS; ++i)
        rjet[i] = fgetc(f) & RJET;
    fclose(f);

    for (int rocks = 0; rocks < PART1; ++rocks) {
        //
    }
    printf("Part 1: %d\n", top);
    return 0;
}
