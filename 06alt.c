/**
 * Advent of Code 2022
 * Day 6: Tuning Trouble, alternative implementation
 * https://adventofcode.com/2022/day/6
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>

#define N (4095)  // character count of full signal in input file
static char signal[N + 1];  // size + string delimiter \0

static int find(const int len)
{
    static int bin['z' - 'a' + 1] = {0}, prev = 0, mark = 0;  // frequency bins
    int dup = 0;                                  // duplicates counter
    for (int i = 0; i < len - prev; ++i, ++mark)  // extend window to new length
        dup += ++bin[signal[mark] - 'a'] == 2;    // 1->2 : duplicate added
    prev = len;                                   // remember len for next function call
    for (; dup && mark < N; ++mark)               // loop until no more duplicates
        dup += (++bin[signal[mark] - 'a'] == 2) - (--bin[signal[mark - len] - 'a'] == 1);
    return mark;                                  // start-of-message marker
}

int main(void)
{
    FILE *f = fopen("input06.txt", "r");
    fgets(signal, sizeof signal, f);
    fclose(f);
    printf("%d\n%d\n", find(4), find(14));  // 1542 3153
    return 0;
}
