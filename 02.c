#include <stdio.h>  // fopen, fclose, fscanf, printf
#include <stdbool.h>

#define N (2500)  // my input has 2500 lines
static char play[N][2];

static bool read(void)
{
    FILE *f = fopen("input02.txt", "r");
    if (!f)
        return false;
    int i = 0;
    char h0, h1;
    while (i < N && fscanf(f, "%c %c ", &h0, &h1) == 2) {
        play[i][0] = h0;
        play[i][1] = h1;
        i++;
    }
    fclose(f);
    if (i != N)
        return false;
    return true;
}

static int part1(void)
{
    int score = 0;
    for (int i = 0; i < N; ++i) {
        const char h0 = play[i][0] - 'A';  // 0=rock, 1=paper, 2=scissors
        const char h1 = play[i][1] - 'X';
        switch ((h1 - h0 + 3) % 3) {
            case 0: score += h1;     break;  // h1-h0= 0: draw    (AA, BB, CC)
            case 1: score += h1 + 3; break;  // h1-h0=+1: h1 wins (AB, BC, CA)
            case 2: score += h1 - 3; break;  // h1-h0=-1: h0 wins (AC, BA, CB)
        }
    }
    return score + N * 4;  // compensate for score minus 4
}

static int part2(void)
{
    int score = 0;
    for (int i = 0; i < N; ++i) {
        const char h0 = play[i][0] - 'A';  // 0=rock, 1=paper, 2=scissors
        switch (play[i][1]) {
            case 'X': score += (h0 + 2) % 3 - 3; break;  // X: h0 wins (AC, BA, CB)
            case 'Y': score +=  h0;              break;  // Y: draw    (AA, BB, CC)
            case 'Z': score += (h0 + 1) % 3 + 3; break;  // Z: h1 wins (AB, BC, CA)
        }
    }
    return score + N * 4;  // compensate for score minus 4
}

int main(void)
{
    if (!read())
        return 1;
    printf("Part 1: %d\n", part1());
    printf("Part 2: %d\n", part2());
    return 0;
}
