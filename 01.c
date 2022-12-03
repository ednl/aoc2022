/* Advent of Code 2022
 * Day 1: Calorie Counting
 * https://adventofcode.com/2022/day/1
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, getline, printf
#include <stdlib.h>  // atoi, free

int main(void)
{
    int calories = 0;  // total calories per elf (separated by blank line)
    int top[3] = {0};  // top 3 elves with most calories
    char *buf = NULL;
    size_t bufsz;
    FILE *f = fopen("input01.txt", "r");
    while (!feof(f))  // extra loop after last line to process last elf
        if (getline(&buf, &bufsz, f) > 1)  // number on line?
            calories += atoi(buf);
        else {  // blank line or EOF
            if (calories > top[0]) {
                top[2] = top[1];
                top[1] = top[0];
                top[0] = calories;
            } else if (calories > top[1]) {
                top[2] = top[1];
                top[1] = calories;
            } else if (calories > top[2])
                top[2] = calories;
            calories = 0;  // reset for next elf
        }
    fclose(f);
    free(buf);
    printf("%d\n%d\n", top[0], top[0] + top[1] + top[2]);  // 69795 208437
    return 0;
}
