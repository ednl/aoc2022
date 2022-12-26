#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define EXAMPLE 1
#if EXAMPLE == 1
#define NAME "example17.txt"
#define JETS (40)  // number of left/right brackets in example file
#else
#define NAME "input17.txt"
#define JETS (10091)  // number of left/right brackets in input file
#endif

#define LEDGE  (1<<6) // left edge = last valid position on the left (chamber wall is at bit 7)
#define REDGE  (1>>0) // right edge = last valid position on the right (chamber wall is at bit -1)
#define JETAVG (('<' + '>')>>1)  // ord('<')=60, ord('>')=62, avg=61, '<'-61=-1 (go left), '>'-61=+1 (go right)
#define ROCKS  (5)    // number of different falling rocks
#define WIDTH  (7)    // chamber width (=playing field width)
#define HEIGHT (4096) // chamber height (=playing field height)
#define PART1  (2022) // number of rocks falling down in part 1
#define PART2  (INT64_C(1000000000000)) // number of rocks falling down in part 2

typedef struct {
    int mask[4];  // rock is max 4 rows high, mask[0] is leading edge when falling down
} Rock;

static const Rock rock[ROCKS][WIDTH] = {
    {{0170},{0074},{0036},{0017}},
    {{0040,0160,0040},{0020,0070,0020},{0010,0034,0010},{0004,0016,0004},{0002,0007,0002}},
    {{0160,0020,0020},{0070,0010,0010},{0034,0004,0004},{0016,0002,0002},{0007,0001,0001}},
    {{0100,0100,0100,0100},{0040,0040,0040,0040},{0020,0020,0020,0020},{0010,0010,0010,0010},{0004,0004,0004,0004},{0002,0002,0002,0002},{0001,0001,0001,0001}},
    {{0140,0140},{0060,0060},{0030,0030},{0014,0014},{0006,0006},{0003,0003}},
};
static const int rockpos[ROCKS] = {4, 5, 5, 7, 6};  // different L/R positions of rock in chamber
static const int rocksize[ROCKS] = {1, 3, 3, 4, 2};  // rock height

static int chamber[HEIGHT];
static int jet[JETS];
static int tall[WIDTH];  // rock column height

static bool isfree(const int r, const int x, const int y)
{
    for (int i = 0; i < rocksize[r]; ++i)
        if (chamber[y + i] & rock[r][x].mask[i])
            return false;
    return true;
}

static void show(const int h)
{
    printf("+-------+\n");
    for (int i = 0; i <= h; ++i) {
        printf("|");
        for (int bit = LEDGE; bit; bit >>= 1)
            printf("%c", chamber[i] & bit ? '#' : '.');
        printf("|\n");
    }
    printf("\n");
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    for (int i = 0; i < JETS; ++i)
        jet[i] = fgetc(f) - JETAVG;  // -1 or +1
    fclose(f);

    int currock = 0, curjet = 0, top = 0;
    for (int rocks = 0; rocks < PART1 && top < HEIGHT; ++rocks) {
        int x = 2, y = top + 3;
        bool move = true;
        while (move) {
            int t = x + jet[curjet];
            if (++curjet == JETS)
                curjet = 0;
            if (t >= 0 && t < rockpos[currock] && isfree(currock, t, y))
                x = t;
            t = y - 1;
            if ((move = t >= 0 && isfree(currock, x, t)))
                y = t;
        }
        for (int i = 0; i < rocksize[currock]; ++i)
            chamber[y++] |= rock[currock][x].mask[i];
        if (y > top)
            top = y;
        if (++currock == ROCKS)
            currock = 0;
    }
    printf("Part 1: %d\n", top);  // example=3068, input=3083
    int base = 20;
    for (int i = 20)
    // printf("Part 2: %d\n", top);  // example=1514285714288, input=?
    return 0;
}
