#include <stdio.h>

#define N (4095)  // character count of full signal in input file
static char signal[N + 1];  // make room for string delimiter \0

static int find(const int len)
{
    static int bin[26] = {0};  // frequency bins for a..z
    static int prev = 0;       // previous len
    static int mark = 0;       // remember mark
    int dup = 0;               // duplicate counter
    for (int i = 0; i < len - prev; ++i, ++mark)  // extend window to new length
        if (++bin[signal[mark] - 'a'] == 2)  // 1->2 : duplicate added
            ++dup;
    while (dup && mark < N) {
        dup += (++bin[signal[mark] - 'a'] == 2) - (--bin[signal[mark - len] - 'a'] == 1);
        ++mark;
    }
    prev = len;   // remember for next function call
    return mark;  // start-of-message
}

int main(void)
{
    FILE *f = fopen("input06.txt", "r");
    fgets(signal, sizeof signal, f);
    fclose(f);
    printf("%d\n%d\n", find(4), find(14));  // 1542 3153
    return 0;
}
