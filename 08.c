#include <stdio.h>    // fopen, fclose, fgetc, printf
#include <stdbool.h>  // bool, true

#define N (99)
static char tree[N][N];
static bool vidi[N][N];

static void look(int x0, int y0, int dx, int dy)
{
    int top = tree[x0][y0];
    for (int i = 1, x = x0 + dx, y = y0 + dy; i < N - 1; ++i, x += dx, y += dy)
        if (tree[x][y] > top) {
            top = tree[x][y];
            vidi[x][y] = true;
        }
}

int main(void)
{
    FILE *f = fopen("input08.txt", "r");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            tree[i][j] = (char)(fgetc(f));
        fgetc(f);  // newline
    }
    fclose(f);

    // Part 1: look in from the outside, check visibility of every tree
    for (int i = 1; i < N - 1; ++i) {
        look(    i,     0,  0,  1);
        look(    i, N - 1,  0, -1);
        look(    0,     i,  1,  0);
        look(N - 1,     i, -1,  0);
    }
    int allvisible = 4 * (N - 1);  // edges
    for (int i = 1; i < N - 1; ++i)
        for (int j = 1; j < N - 1; ++j)
            allvisible += vidi[i][j];
    printf("Part 1: %d\n", allvisible);  // 1805

    // Part 2: look out from the inside, check scenic quality (line of sight in all 4 directions)
    int best = 1;
    for (int i = 1; i < N - 1; ++i)
        for (int j = 1; j < N - 1; ++j) {
            int k, scenic = 1, house = tree[i][j];
            for (k = j - 1; k > 0 && tree[i][k] < house; --k);
            scenic *= j - k;  // left
            for (k = j + 1; k < N - 1 && tree[i][k] < house; ++k);
            scenic *= k - j;  // right
            for (k = i - 1; k > 0 && tree[k][j] < house; --k);
            scenic *= i - k;  // up
            for (k = i + 1; k < N - 1 && tree[k][j] < house; ++k);
            scenic *= k - i;  // down
            if (scenic > best)
                best = scenic;
        }
    printf("Part 2: %d\n", best);  // 444528
    return 0;
}
