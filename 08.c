#include <stdio.h>    // fopen, fclose, fgetc, printf
#include <stdlib.h>   // abs
#include <stdbool.h>  // bool, true

#define N (99)
static char forest[N][N];
static bool visible[N][N];

// Part 1: look in from the outside, check visibility of every tree
static void lookin(int x0, int y0, int dx, int dy)
{
    int maxh = forest[x0][y0];
    for (int i = 1, x = x0 + dx, y = y0 + dy; i < N - 1; ++i, x += dx, y += dy)
        if (forest[x][y] > maxh) {
            maxh = forest[x][y];
            visible[x][y] = true;
        }
}

// Part 2: look out from the inside, check how far you can look
static int lookout(int x0, int y0, int dx, int dy)
{
    int x = x0 + dx, y = y0 + dy, maxh = forest[x0][y0];
    for (; x > 0 && y > 0 && x < N - 1 && y < N - 1 && forest[x][y] < maxh; x += dx, y += dy);
    return abs(dx ? x - x0 : y - y0);
}

int main(void)
{
    FILE *f = fopen("input08.txt", "r");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            forest[i][j] = (char)(fgetc(f));
        fgetc(f);  // newline
    }
    fclose(f);

    for (int i = 1; i < N - 1; ++i) {
        lookin(i, 0, 0, 1);
        lookin(i, N - 1, 0, -1);
        lookin(0, i, 1, 0);
        lookin(N - 1, i, -1, 0);
    }
    int allvisible = 4 * (N - 1);  // edges
    for (int i = 1; i < N - 1; ++i)
        for (int j = 1; j < N - 1; ++j)
            allvisible += visible[i][j];
    printf("Part 1: %d\n", allvisible);  // 1805

    int maxscenic = 1;
    for (int i = 1; i < N - 1; ++i)
        for (int j = 1; j < N - 1; ++j) {
            int scenic = 1;
            if (i > 1)
                scenic *= lookout(i, j, -1, 0);
            if (i < N - 2)
                scenic *= lookout(i, j, 1, 0);
            if (j > 1)
                scenic *= lookout(i, j, 0, -1);
            if (j < N - 2)
                scenic *= lookout(i, j, 0, 1);
            if (scenic > maxscenic)
                maxscenic = scenic;
        }
    printf("Part 2: %d\n", maxscenic);  // 444528
    return 0;
}
