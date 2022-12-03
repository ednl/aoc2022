/* Advent of Code 2022
 * Day 2: Rock Paper Scissors
 * https://adventofcode.com/2022/day/2
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>

#define AX ('A' << 8 | 'X')
#define AY ('A' << 8 | 'Y')
#define AZ ('A' << 8 | 'Z')
#define BX ('B' << 8 | 'X')
#define BY ('B' << 8 | 'Y')
#define BZ ('B' << 8 | 'Z')
#define CX ('C' << 8 | 'X')
#define CY ('C' << 8 | 'Y')
#define CZ ('C' << 8 | 'Z')

int main(void)
{
    char buf[5];
    int s = 0, t = 0;
    FILE *f = fopen("input02.txt", "r");
    while (fgets(buf, sizeof buf, f) != NULL)
        switch (buf[0] << 8 | buf[2]) {
            case AX: s += 1+3; t += 3+0; break;  // 1: AA = draw, 2: AC = loss
            case AY: s += 2+6; t += 1+3; break;  // 1: AB = win , 2: AA = draw
            case AZ: s += 3+0; t += 2+6; break;  // 1: AC = loss, 2: AB = win
            case BX: s += 1+0; t += 1+0; break;  // 1: BA = loss, 2: BA = loss
            case BY: s += 2+3; t += 2+3; break;  // 1: BB = draw, 2: BB = draw
            case BZ: s += 3+6; t += 3+6; break;  // 1: BC = win , 2: BC = win
            case CX: s += 1+6; t += 2+0; break;  // 1: CA = win , 2: CB = loss
            case CY: s += 2+0; t += 3+3; break;  // 1: CB = loss, 2: CC = draw
            case CZ: s += 3+3; t += 1+6; break;  // 1: CC = draw, 2: CA = win
        }
    fclose(f);
    printf("%d\n%d\n", s, t);  // 11767 13886
    return 0;
}
