#include <stdio.h>   // fopen, fclose, fgets, fscanf, printf

#define LINECOUNT   (8)  // crates are stacked 8 high in initial config
#define LINEWIDTH  (40)  // 35 chars + \n + \0 + safety margin
#define COLOFFSET   (1)  // first crate ID is at char pos 1
#define COLWIDTH    (4)  // stacks are 4 chars apart
#define STACKCOUNT  (9)  // there are 9 stacks (columns) of crates
#define STACKSIZE  (64)  // maximum stack size = LINECOUNT * STACKCOUNT - empty spots
#define MOVES     (512)  // 501 crate move instructions in input file + safety margin

static char ini[LINECOUNT][LINEWIDTH];
static char stack[STACKCOUNT][STACKSIZE];
static int sp[STACKCOUNT];
static struct Move {
    int crates, src, dst;
} move[MOVES];

// Read input file, return number of crate moving instructions
static int read(void)
{
    FILE *f = fopen("input05.txt", "r");
    // Read initial configuration
    for (int i = 0; i < LINECOUNT; ++i)
        fgets(ini[i], LINEWIDTH, f);
    // Skip 2 lines (but do not reuse ini[])
    char buf[LINEWIDTH];
    fgets(buf, LINEWIDTH, f);
    fgets(buf, LINEWIDTH, f);
    // Count & save crate moving instructions
    int i = 0, a, b, c;
    while (i < MOVES && fscanf(f, "move %d from %d to %d ", &a, &b, &c) == 3)
        move[i++] = (struct Move){a, --b, --c};  // zero based stacks array
    fclose(f);
    return i;
}

// Parse initial config to stacks
static void reset(void)
{
    for (int i = 0; i < STACKCOUNT; ++i) {
        sp[i] = 0;
        int col = COLOFFSET + COLWIDTH * i;
        for (int j = LINECOUNT; j > 0; --j) {  // stacks start at the bottom
            char c = ini[j - 1][col];
            if (c == ' ')  // stop when nothing more in this column
                break;
            stack[i][sp[i]++] = c;  // add to stack
        }
    }
}

// Print crate IDs from top of each stack
static void result(int part)
{
    printf("Part %d: ", part);
    for (int i = 0; i < STACKCOUNT; ++i)
        printf("%c", stack[i][sp[i] - 1]);
    printf("\n");
}

int main(void)
{
    int moves = read();

    // Part 1: move crates one by one, from top of stack to top of stack
    reset();
    for (int m = 0; m < moves; ++m) {
        int n = move[m].crates, i = move[m].src, j = move[m].dst;
        while (n--)
            stack[j][sp[j]++] = stack[i][--sp[i]];
    }
    result(1);

    // Part 2: move n crates at a time from stack to stack
    reset();
    for (int m = 0; m < moves; ++m) {
        int n = move[m].crates, i = move[m].src, j = move[m].dst;
        int k = sp[i] - n;  // start from n crates down on source stack i
        sp[i] = k;  // remove crates from source stack i
        while (n--)
            stack[j][sp[j]++] = stack[i][k++];
    }
    result(2);
    return 0;
}
