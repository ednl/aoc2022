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
            case AX: s += 4; t += 3; break;  // 1: AA = draw 1+3, 2: AC = loss 3+0
            case AY: s += 8; t += 4; break;  // 1: AB = win  2+6, 2: AA = draw 1+3
            case AZ: s += 3; t += 8; break;  // 1: AC = loss 3+0, 2: AB = win  2+6
            case BX: s += 1; t += 1; break;  // 1: BA = loss 1+0, 2: BA = loss 1+0
            case BY: s += 5; t += 5; break;  // 1: BB = draw 2+3, 2: BB = draw 2+3
            case BZ: s += 9; t += 9; break;  // 1: BC = win  3+6, 2: BC = win  3+6
            case CX: s += 7; t += 2; break;  // 1: CA = win  1+6, 2: CB = loss 2+0
            case CY: s += 2; t += 6; break;  // 1: CB = loss 2+0, 2: CC = draw 3+3
            case CZ: s += 6; t += 7; break;  // 1: CC = draw 3+3, 2: CA = win  1+6
        }
    fclose(f);
    printf("%d %d\n", s, t);  // 11767 13886
    return 0;
}
