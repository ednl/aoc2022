#include <stdio.h>  // fopen, fclose, fscanf, printf

#define N (2500)
static char play[N][2];

typedef struct {
    int p0, p1;
} Score;

static Score judge(const int part, const char hands[2])
{
    // Translation
    const char h0 = hands[0] - 'A';  // [A..C] => [0..2]
    char h1 = hands[1] - 'X';        // [X..Z] => [0..2]
    if (part == 2) {
        switch (h1) {
            case 0: h1 = (h0 + 2) % 3; break;  // X: h1=h0-1 => h1-h0=-1 => player 0 wins
            case 1: h1 =  h0;          break;  // Y: h1=h0   => h0-h1= 0 => draw
            case 2: h1 = (h0 + 1) % 3; break;  // Z: h1=h0+1 => h0-h1=+1 => player 1 wins
        }
    }
    // Compare hands, give score minus 4 (rock=1, papers=2, scissors=3, draw=+3)
    switch ((h1 - h0 + 3) % 3) {
        case 2: return (Score){h0 + 3, h1 - 3};  // -1: player 0 wins: AC, BA, CB
        case 1: return (Score){h0 - 3, h1 + 3};  // +1: player 1 wins: AB, BC, CA
    }
    return (Score){h0, h1};  // draw: AA, BB, CC
}

static void add(Score *s, const Score t)
{
    s->p0 += t.p0;
    s->p1 += t.p1;
}

int main(void)
{
    FILE *f = fopen("input02.txt", "r");
    if (!f)
        return 1;

    int i = 0;
    char c0, c1;
    while (i < N && fscanf(f, "%c %c ", &c0, &c1) == 2) {
        play[i][0] = c0;
        play[i][1] = c1;
        i++;
    }
    fclose(f);
    if (i != N)
        return 2;

    Score score = {0};
    for (i = 0; i < N; ++i)
        add(&score, judge(1, play[i]));
    printf("Part 1: %d\n", score.p1 + N * 4);  // compensate for score minus 4

    score = (Score){0, 0};
    for (i = 0; i < N; ++i)
        add(&score, judge(2, play[i]));
    printf("Part 2: %d\n", score.p1 + N * 4);  // compensate for score minus 4
    return 0;
}
