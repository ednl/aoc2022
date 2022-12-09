#include <stdio.h>    // fopen, fclose, fgets, printf
#include <stdlib.h>   // atoi, abs
#include <stdbool.h>  // bool, true

#define EXAMPLE 0
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

// Returns: x > 0 => +1, x == 0 => 0, x < 0 => -1
static int sign(int x)
{
    return (x > 0) - (x < 0);
}

static void addto(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

static Vec add(const Vec a, const Vec b)
{
    return (Vec){a.x + b.x, a.y + b.y};
}

static Vec sub(const Vec a, const Vec b)
{
    return (Vec){a.x - b.x, a.y - b.y};
}

static Vec mult(const Vec a, const int b)
{
    return (Vec){a.x * b, a.y * b};
}

static Vec absdot(const Vec a)
{
    return (Vec){abs(a.x), abs(a.y)};
}

static Vec sgndot(const Vec a)
{
    return (Vec){sign(a.x), sign(a.y)};
}

static bool adjacent(const Vec a, const Vec b)
{
    return abs(a.x - b.x) <= 1 && abs(a.y - b.y) <= 1;
}

static bool equal(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

static void initgrid(const Vec a, const Vec b)
{
    dim = add(absdot(sub(b, a)), (Vec){1, 1});
    seen = malloc((size_t)(dim.x * dim.y) * sizeof *seen);
}

static void visit(const Vec a)
{
    seen[a.y * dim.x + a.x] = true;
}

static bool visited(const Vec a)
{
    return seen[a.y * dim.x + a.x];
}

static int countgrid(void)
{
    int sum = 0;
    for (int i = 0; i < dim.x * dim.y; ++i)
        sum += seen[i];
    return sum;
}

static int findknot(const Vec pos, const int knotcount)
{
    for (int i = 0; i < knotcount; ++i)
        if (equal(pos, knot[i]))
            return i;
    return -1;
}

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

static void follow(const int index, const bool islast)
{
    const Vec head = knot[index - 1];
    Vec tail = knot[index];
    while (!adjacent(head, tail)) {
        addto(&tail, sgndot(sub(head, tail)));
        if (islast)
            visit(tail);
    }
    knot[index] = tail;
}

static void simulate(const Vec startpos, const int knotcount)
{
    for (int i = 0; i < knotcount; ++i)
        knot[i] = startpos;
    for (int i = 0; i < dim.x * dim.y; ++i)
        seen[i] = false;
    visit(startpos);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < move[i].len; ++j) {
            addto(&knot[0], move[i].dir);
            for (int k = 1; k < knotcount; ++k)
                follow(k, k == knotcount - 1);
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
    initgrid(min, max);
    pos = (Vec){-min.x, -min.y};  // adjusted starting position for zero-based grid

    simulate(pos, K1);
    printf("Part 1: %d\n", countgrid());  // ex1=13, ex2=88, input=6745

    simulate(pos, K2);
    printf("Part 2: %d\n", countgrid());  // ex1= 1, ex2=36, input=2793

    free(seen);
    return 0;
}
