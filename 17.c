/**
 * Advent of Code 2022
 * Day 17: Pyroclastic Flow
 * https://adventofcode.com/2022/day/17
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * gives a shortest runtime for my input file (not the example) of [TBD] µs.
 * On a Raspberry Pi 4 with the CPU in performance mode: [TBD] µs.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>
#include <string.h>    // memcpy
#include <stdint.h>    // int8_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>
#include "startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "example17.txt"
#define JETS (40)  // number of left/right brackets in example file
#else
#define NAME "input17.txt"
#define JETS (10091)  // number of left/right brackets in input file
#endif

#define JETAVG (('<' + '>')>>1)  // ord('<')=60, ord('>')=62, avg=61, '<'-61=-1 (go left), '>'-61=+1 (go right)
#define X0     (2)     // rocks start 2 from left wall
#define Y0     (3)     // rocks start 3 up from highest rock already down
#define RKNUM  (5)     // number of different falling rocks
#define RKDIM  (4)     // rocks are 4x4
#define CHAMW  (7)     // chamber width (=playing field width)
#define CHAMH  (1024)  // chamber height (=playing field height)
#define PART1  (2022)  // number of rocks falling down in part 1
#define PART2  (INT64_C(1000000000000))  // number of rocks falling down in part 2

#define MARGIN (16)              // safety margin from top
#define MAXH   (CHAMH - MARGIN)  // threshold to start moving down 4096-16=4080
#define REBASE (MAXH - 64)       // take as new base (floor) 4080-64=4016
#define BLOCK  (CHAMH - REBASE)  // move lines down 4096-4016=80

typedef struct {
    int8_t row[RKDIM];  // rock is max 4 rows high, mask[0] is leading edge when falling down
} Mask;

typedef struct {
    int id, n, w, h;   // id (0..RKNUM-1), number of masks (n=CHAMW-w+1), width, height
    Mask mask[CHAMW];  // max 7 different shifted masks
} Rock;

typedef struct {
    int64_t dropped, top;
    int rockindex, jetindex;
} State;

// Rock shapes as defined by puzzle, but leading edge first (so, "upside down")
static const char* shape[] = {
    ("####"),
    (".#.."
     "###."
     ".#.."),
    ("###."
     "..#."
     "..#."),
    ("#..."
     "#..."
     "#..."
     "#..."),
    ("##.."
     "##..")
};

static Rock rock[RKNUM];
static int8_t jet[JETS];
static int8_t chamber[CHAMH];
static int64_t top, base;
static State state[8192];

static Rock str2rock(const char* const s, const int id)
{
    Rock rk = {.id=id};
    const char* pc = s;
    int r = 0, c = 0, bits = 0;
    while (*pc != '\0' && r < RKDIM) {
        bits <<= 1;
        if (*pc == '#') {
            bits |= 1;
            if (c == rk.w)
                ++rk.w;  // width = max col+1 where bit=1
        }
        if (++c == RKDIM) {
            rk.mask[0].row[r++] = (int8_t)(bits << (CHAMW - RKDIM));  // shift against left wall
            c = bits = 0;
        }
        ++pc;
    }
    if (c && bits && r < RKDIM)  // catch malformed rockshape[]
        rk.mask[0].row[r++] = (int8_t)(bits << (CHAMW - RKDIM));
    rk.h = r;  // height = number of rows
    rk.n = CHAMW - rk.w + 1;  // number of possible shifts L/R in the chamber
    for (int i = 1; i < rk.n; ++i)
        for (int j = 0; j < rk.h; ++j)
            rk.mask[i].row[j] = rk.mask[i - 1].row[j] >> 1;  // other masks are each shifted 1 to the right
    return rk;
}

static int makerocks(void)
{
    int i = 0;
    while (i < RKNUM && i < (int)(sizeof(shape) / sizeof(*shape))) {
        rock[i] = str2rock(shape[i], i);
#if EXAMPLE == 1
        printf("id=%d n=%d w=%d h=%d\n", rock[i].id, rock[i].n, rock[i].w, rock[i].h);
        for (int r = 0; r < rock[i].h; ++r) {
            for (int n = 0; n < rock[i].n; ++n) {
                printf(" ");
                for (int8_t bit = (int8_t)(1 << (CHAMW - 1)); bit; bit >>= 1)
                    printf("%c", rock[i].mask[n].row[r] & bit ? '#' : '.');
            }
            printf("\n");
        }
        printf("\n");
#endif
        ++i;
    }
    return i;
}

static int read(const char* const s)
{
    FILE* f = fopen(s, "r");
    if (!f)
        return 0;
    int i = 0, c;
    while (i < JETS && (c = fgetc(f)) != EOF)
        if (c == '<' || c == '>')
            jet[i++] = (int8_t)(c - JETAVG);  // '<'=-1, '>'=+1
    fclose(f);
    return i;
}

#if EXAMPLE == 1
static void showline(const int64_t y)
{
    printf("|");
    for (int8_t bit = (int8_t)(1 << (CHAMW - 1)); bit; bit >>= 1)
        printf("%c", chamber[y - base] & bit ? '#' : '.');
    printf("|");
}

static void show(const int64_t first, int64_t last)
{
    if (first < last || last < 0)
        return;
    showline(first);
    printf(" y=%"PRId64"\n", first);
    for (int64_t y = first - 1; y >= last + 1; --y) {
        showline(y);
        printf("\n");
    }
    if (last != first) {
        showline(last);
        printf(" y=%"PRId64"\n", last);
    }
    if (last == base)
        printf("+-------+\n");
}
#endif

static void settle(const Rock* const r, const int x, const int64_t y)
{
    int64_t h = y - base;
    for (int i = 0; i < r->h; ++i)
        chamber[h++] |= r->mask[x].row[i];
    h += base;
    if (h > top)
        top = h;
}

static bool isfree(const Rock* const r, const int x, const int64_t y)
{
    if (!r || x < 0 || x >= r->n)
        return false;
    int64_t h = y - base;
    if (h < 0 || h > CHAMH - r->h)
        return false;
    for (int i = 0; i < r->h; ++i)
        if (chamber[h++] & r->mask[x].row[i])
            return false;
    return true;
}

int main(void)
{
    starttimer();
    int rocks = makerocks();
    int jets = read(NAME);

#if EXAMPLE == 1
    printf("rocks=%d jets=%d\n\n", rocks, jets);
#endif

    int rockindex = 0, jetindex = 0;
    int64_t dropped = 0;
    for (; dropped < PART1; ++dropped) {
        const Rock* const r = rock + rockindex;
        int x = X0 + jet[jetindex];  // first move always possible
        if (++jetindex == jets)
            jetindex = 0;
        for (int i = 0; i < Y0; ++i) {  // empty chamber down to previous highest
            int x1 = x + jet[jetindex];
            if (++jetindex == jets)
                jetindex = 0;
            if (x1 >= 0 && x1 < r->n)
                x = x1;
        }
        int64_t y = top;  // margin of 3 already skipped
        bool moving = true;
        while (moving) {
            int64_t y1 = y - 1;
            if ((moving = isfree(r, x, y1))) {  // last move was shift, so now drop
                y = y1;
                int x1 = x + jet[jetindex];  // only shift if drop successful
                if (++jetindex == jets)
                    jetindex = 0;
                if (isfree(r, x1, y))
                    x = x1;
            }
        }
        settle(r, x, y);
        if (top - base > MAXH) {
            memcpy(chamber, chamber + REBASE, BLOCK);
            memset(chamber + BLOCK, 0, REBASE);
            base += REBASE;
        }
        if (++rockindex == rocks)
            rockindex = 0;
#if EXAMPLE == 1
        if (dropped < 10)
            show(top, base);
#endif
    }
    printf("Part 1: %"PRId64"\n", top);  // example=3068 input=3083
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
