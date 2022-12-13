/**
 * Advent of Code 2022
 * Day 12: Hill Climbing Algorithm
 * https://adventofcode.com/2022/day/12
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, fgetc, printf
#include <stdbool.h>  // bool
#include "startstoptimer.h"

#define NAME "input12.txt"
#define ROWS  (41)   // lines in input file
#define COLS  (159)  // line width in input file
#define QSIZE (ROWS * COLS)

typedef struct {
    int x, y;
} Vec;

static const Vec dir[4] = {{1,0},{0,1},{-1,0},{0,-1}};
static const Vec error = {-1,-1};

static Vec queue[QSIZE];
static int qhead, qtail, qlen;

static char alt[ROWS][COLS];  // altitude map
static int dist[ROWS][COLS];  // distance travelled

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

static bool reachable(const Vec v, char base)
{
    if (v.x < 0 || v.y < 0 || v.x >= COLS || v.y >= ROWS)
        return false;
    if (dist[v.y][v.x] != -1)
        return false;
    return alt[v.y][v.x] < base + 2;  // climb <= 1
}

static int flood(const Vec start, const Vec goal)
{
    // Reset
    qtail = qhead = qlen = 0;  // empty queue
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            dist[i][j] = -1;
    // Init
    dist[start.y][start.x] = 0;
    enqueue(start);
    // Search
    while (qlen) {
        Vec v = dequeue();
        const char a = alt[v.y][v.x];
        const int d = dist[v.y][v.x];
        if (isequal(v, goal))  // found shortest path
            return d;
        for (size_t i = 0; i < sizeof(dir) / sizeof(*dir); ++i) {
            const Vec w = add(v, dir[i]);
            if (reachable(w, a)) {
                dist[w.y][w.x] = d + 1;
                enqueue(w);
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
            if (c == 'S') {
                start = (Vec){j, i};
                alt[i][j] = 'a';
            } else if (c == 'E') {
                end = (Vec){j, i};
                alt[i][j] = 'z';
            } else
                alt[i][j] = (char)c;
        }
        fgetc(f);  // newline
    }
    printf("Part 1: %d\n", flood(start, end));  // example=31, input=504
    // printf("Part 2: %d\n", flood(end, error));  // example=29, input=500
    printf("Time: %.0f us\n", stoptimer_us());  // fastest: Mac Mini M1 = 251 us, Raspberry Pi 4 = 791 us
    return 0;
}
