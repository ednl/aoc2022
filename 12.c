/**
 * Advent of Code 2022
 * Day 12: Hill Climbing Algorithm
 * https://adventofcode.com/2022/day/12
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, fgetc, printf
#include <stdbool.h>  // bool
#include "startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "example12.txt"
#define ROWS (5)  // lines in example file
#define COLS (8)  // line width in example file
#else
#define NAME "input12.txt"
#define ROWS  (41)   // lines in input file
#define COLS  (159)  // line width in input file
#endif
#define QSIZE (ROWS * COLS)

typedef struct {
    int x, y;
} Vec;

static const Vec dir[4] = {{1,0},{0,1},{-1,0},{0,-1}};
static const Vec error = {-1,-1};

static Vec queue[QSIZE];
static int qhead, qtail, qlen;

static char map[ROWS][COLS];
static bool explored[ROWS][COLS];
static Vec backtrack[ROWS][COLS];

static void enqueue(const Vec pos)
{
    if (qlen == QSIZE)
        return;
    queue[qhead++] = pos;
    if (qhead == QSIZE)
        qhead = 0;
    qlen++;
}

static Vec dequeue(void)
{
    if (qlen == 0)
        return error;
    const int i = qtail;
    if (++qtail == QSIZE)
        qtail = 0;
    return queue[i];
}

static bool isequal(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

static Vec add(const Vec a, const Vec b)
{
    return (Vec){a.x + b.x, a.y + b.y};
}

static char elevation(const Vec pos)
{
    const char h = map[pos.y][pos.x];
    if (h == 'S') return 'a';
    if (h == 'E') return 'z';
    return h;
}

static bool reachable(const Vec pos, char base, bool ispart2)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= COLS || pos.y >= ROWS)
        return false;
    if (explored[pos.y][pos.x])
        return false;
    // M1/x86: char=signed, ARM: char=unsigned
    // so, no sure way of comparing climb = elev - base to +/-2
    const char elev = elevation(pos);
    return ispart2 ? elev > base - 2 : elev < base + 2;
}

static int bfs(const Vec start, const Vec goal)
{
    const bool ispart2 = isequal(goal, error);
    // Reset
    qtail = qhead = qlen = 0;  // empty queue
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j) {
            explored[i][j] = false;
            backtrack[i][j] = error;
        }
    // Init
    explored[start.y][start.x] = true;
    enqueue(start);
    // Search
    while (qlen) {
        Vec curpos = dequeue();
        const char curelev = elevation(curpos);
        if (ispart2 ? curelev == 'a' : isequal(curpos, goal)) {  // found shortest path
            int steps = 0;
            while (!isequal(curpos, start)) {
                const Vec nextpos = backtrack[curpos.y][curpos.x];
                if (isequal(nextpos, curpos) || isequal(nextpos, error))  // avoid potential deadlock
                    return -1;
                curpos = nextpos;
                ++steps;
            }
            return steps;
        }
        for (size_t i = 0; i < sizeof(dir) / sizeof(*dir); ++i) {
            const Vec nextpos = add(curpos, dir[i]);
            if (reachable(nextpos, curelev, ispart2)) {
                explored[nextpos.y][nextpos.x] = true;  // label nextpos as explored
                backtrack[nextpos.y][nextpos.x] = curpos;  // set curpos as parent of nextpos
                enqueue(nextpos);
            }
        }
    }
    return -1;
}

int main(void)
{
    starttimer();
    Vec start = error, end = error;
    FILE *f = fopen(NAME, "r");
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            int c = fgetc(f);
            map[i][j] = (char)c;
            if (c == 'S')
                start = (Vec){.x=j, .y=i};
            else if (c == 'E')
                end = (Vec){.x=j, .y=i};
        }
        fgetc(f);  // newline
    }
    printf("Part 1: %d\n", bfs(start, end));  // example=31, input=504
    printf("Part 2: %d\n", bfs(end, error));  // example=29, input=500
    printf("Time: %.0f us\n", stoptimer_us());  // fastest: Mac Mini M1 = 251 us, Raspberry Pi 4 = 791 us
    return 0;
}
