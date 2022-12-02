#include <stdio.h>  // fopen, fclose, fscanf, printf

#define N (2500)
static char play[N][2];

typedef struct {
    int p0, p1;
} Score;

// Score for 2 players where rock(A)=1, paper(B)=2, scissors(C)=3, draw=+3, win=+6
// but minus 4 everywhere, so: rock(A)=0, paper(B)=1, scissors(C)=2, loss=-3, win=+3
static Score judge(const int part, const char hands[2])
{
    const char h0 = hands[0] - 'A';  // 0=rock, 1=paper, 2=scissors
    if (part == 1) {
        const char h1 = hands[1] - 'X';  // 0=rock, 1=paper, 2=scissors
        switch ((h1 - h0 + 3) % 3) {
            case 0: return (Score){h0    , h1    };  // h1-h0= 0: draw    (AA, BB, CC)
            case 1: return (Score){h0 - 3, h1 + 3};  // h1-h0=+1: h1 wins (AB, BC, CA)
            case 2: return (Score){h0 + 3, h1 - 3};  // h1-h0=-1: h0 wins (AC, BA, CB)
        }
    } else if (part == 2) {
        switch (hands[1]) {
            case 'X': return (Score){h0 + 3, (h0 + 2) % 3 - 3};  // X: h0 wins (AC, BA, CB)
            case 'Y': return (Score){h0    ,  h0             };  // Y: draw    (AA, BB, CC)
            case 'Z': return (Score){h0 - 3, (h0 + 1) % 3 + 3};  // Z: h1 wins (AB, BC, CA)
        }
    }
    return (Score){0, 0};  // avoid compiler warning, should be unreachable
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
