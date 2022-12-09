/**
 * Advent of Code 2022
 * Day 9: Rope Bridge
 * https://adventofcode.com/2022/day/9
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include <stdlib.h>   // atoi, abs
#include <stdbool.h>  // bool, true

#define EXAMPLE 0  // 0=use input file, 1=example 1, 2=example 2
#if EXAMPLE == 1
static const char *inp[] = {
    "R 4",
    "U 4",
    "L 3",
    "D 1",
    "R 4",
    "D 1",
    "L 5",
    "R 2"
};
static const int N = sizeof inp / sizeof(char*);
#elif EXAMPLE == 2
static const char* inp[] = {
    "R 5",
    "U 8",
    "L 8",
    "D 3",
    "R 17",
    "D 10",
    "L 25",
    "U 20"
};
static const int N = sizeof inp / sizeof(char*);
#else
#define N (2000)  // lines in input file = 2000
#define M    (8)  // max line length = 5 ("R 19\n") + 1 string term \0 + 2 char margin
static char inp[N][M];
#endif

#define K1  (2)  // number of knots for part 1
#define K2 (10)  // number of knots for part 2

typedef struct _Vec {
    int x, y;
} Vec;
typedef struct _Move {
    Vec dir;
    int len;
} Move;

static Move move[N];  // translated instructions from input
static Vec knot[K2];  // string of knots (K1 <= K2)
static bool* seen;    // grid to keep track of visited locations
static Vec dim;       // grid dimensions

// Sign of x: x > 0 => +1, x == 0 => 0, x < 0 => -1
static int sign(int x)
{
    return (x > 0) - (x < 0);
}

// Element-wise sign (-1,0,+1) of difference between vectors 'a' and 'b', return new vector
static Vec sgndot(const Vec a, const Vec b)
{
    return (Vec){sign(a.x - b.x), sign(a.y - b.y)};
}

// Element-wise absolute value of difference between vectors 'a' and 'b', return new vector
static Vec absdot(const Vec a, const Vec b)
{
    return (Vec){abs(a.x - b.x), abs(a.y - b.y)};
}

// Modify vector 'a' by adding vector 'b'
static void addto(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

// Add vectors 'a' and 'b', return new vector
static Vec add(const Vec a, const Vec b)
{
    return (Vec){a.x + b.x, a.y + b.y};
}

// Multiply vector 'a' by scalar 'b', return new vector
static Vec mult(const Vec a, const int b)
{
    return (Vec){a.x * b, a.y * b};
}

// Are locations 'a' and 'b' adjacent or overlapping?
static bool adjacent(const Vec a, const Vec b)
{
    return abs(a.x - b.x) <= 1 && abs(a.y - b.y) <= 1;
}

// Are locations 'a' and 'b' overlapping?
static bool equal(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

// Allocate memory for grid spanned by vectors 'a' and 'b'
static void initgrid(const Vec a, const Vec b)
{
    dim = add(absdot(a, b), (Vec){1, 1});  // dim = diff + 1
    seen = malloc((size_t)(dim.x * dim.y) * sizeof *seen);
}

// Mark location 'a' as visited
static void visit(const Vec a)
{
    seen[a.y * dim.x + a.x] = true;
}

// Has location 'a' been visited?
static bool visited(const Vec a)
{
    return seen[a.y * dim.x + a.x];
}

// Count all visited locations
static int countgrid(void)
{
    int sum = 0;
    for (int i = 0; i < dim.x * dim.y; ++i)
        sum += seen[i];
    return sum;
}

// Find index of the first knot with position equal to 'pos', or -1=not found
static int findknot(const Vec pos, const int knotcount)
{
    for (int i = 0; i < knotcount; ++i)
        if (equal(pos, knot[i]))
            return i;
    return -1;
}

// Draw example grids same way as on adventofcode.com
static void showgrid(const Vec startpos, const int knotcount)
{
    for (int y = 0; y < dim.y; ++y) {
        for (int x = 0; x < dim.x; ++x) {
            const Vec p = (Vec){x, y};
            const int k = findknot(p, knotcount);
            if (k == 0) printf("H");
            else if (k > 0 && k < knotcount - 1) printf("%d", k);
            else if (k == knotcount - 1) printf("T");
            else if (equal(p, startpos)) printf("s");
            else printf("%c", visited(p) ? '#' : '.');
        }
        printf("\n");
    }
    printf("\n");
}

// Move tail towards head until adjacent, return true if moved
static bool follow(const Vec head, Vec *const tail, const bool islast)
{
    bool hasmoved = false;
    while (!adjacent(head, *tail)) {
        addto(tail, sgndot(head, *tail));
        hasmoved = true;
        if (islast)
            visit(*tail);
    }
    return hasmoved;
}

// Complete simulation of 'knotcount' knots starting at location 'startpos'
static void simulate(const Vec startpos, const int knotcount)
{
    // Init
    for (int i = 0; i < knotcount; ++i)
        knot[i] = startpos;
    for (int i = 0; i < dim.x * dim.y; ++i)
        seen[i] = false;
    visit(startpos);
    // Do the thing
    for (int i = 0; i < N; ++i) {                // for every move instruction
        for (int j = 0; j < move[i].len; ++j) {  // take one step at a time (!!!)
            addto(&knot[0], move[i].dir);        // move the head
            for (int k = 1; k < knotcount && follow(knot[k - 1], &knot[k], k == knotcount - 1); ++k)
                ;                                // stop at first knot that hasn't moved
        }
#if EXAMPLE
        printf("%s\n", inp[i]);
        showgrid(startpos, knotcount);
#endif
    }
}

int main(void)
{
#if !EXAMPLE
    // Read input file
    FILE *f = fopen("input09.txt", "r");
    if (!f)
        return 1;
    for (int i = 0; i < N && !feof(f); ++i)
        fgets((char*)&inp[i], M, f);
    fclose(f);
#endif

    // Parse input
    for (int i = 0; i < N; ++i) {
        switch (inp[i][0]) {
            case 'L': move[i].dir.x = -1; break;  // left
            case 'R': move[i].dir.x =  1; break;  // right
            case 'U': move[i].dir.y = -1; break;  // up
            case 'D': move[i].dir.y =  1; break;  // down
        }
        move[i].len = atoi(&inp[i][2]);  // range [1..19]
    }

    // Determine grid dimensions
    Vec pos = (Vec){0, 0}, min = (Vec){0, 0}, max = (Vec){0, 0};
    for (int i = 0; i < N; ++i) {
        addto(&pos, mult(move[i].dir, move[i].len));
        if (pos.x < min.x) min.x = pos.x;
        if (pos.x > max.x) max.x = pos.x;
        if (pos.y < min.y) min.y = pos.y;
        if (pos.y > max.y) max.y = pos.y;
    }
    pos = (Vec){-min.x, -min.y};  // adjusted starting position for zero-based grid
    initgrid(min, max);

    simulate(pos, K1);
    printf("Part 1: %d\n", countgrid());  // ex1=13, ex2=88, input=6745

    simulate(pos, K2);
    printf("Part 2: %d\n", countgrid());  // ex1= 1, ex2=36, input=2793

    free(seen);
    return 0;
}
