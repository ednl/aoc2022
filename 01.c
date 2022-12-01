#include <stdio.h>   // fgetc, getline, printf
#include <stdlib.h>  // calloc, free, qsort

static int desc(const void* p, const void* q)
{
    return *(const int*)q - *(const int*)p;  // fails with INT_MIN
}

int main(void)
{
    FILE *f = fopen("input01.txt", "r");
    if (!f)
        return 1;

    // Count elves = empty lines + 1
    size_t N = 1;
    int c, lf = 0;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            if (lf++)
                N++;
        } else
            lf = 0;
    }

    // Make array for calorie sums
    int *elf = malloc(N * sizeof *elf);
    if (elf == NULL)
        return 2;

    char *buf = NULL;
    size_t bufsz = 0, i = 0;
    ssize_t len = 0;
    int calories = 0;

    rewind(f);
    while (len != -1) {
        len = getline(&buf, &bufsz, f);
        if (len > 1) {
            // line with number
            calories += atoi(buf);
        } else {
            // empty line or EOF
            if (i < N)
                elf[i++] = calories;
            calories = 0;
        }
    }
    fclose(f);
    free(buf);

    if (i != N) {
        free(elf);
        return 3;
    }

    qsort(elf, N, sizeof *elf, desc);
    printf("Part 1: %d\n", elf[0]);
    printf("Part 2: %d\n", elf[0] + elf[1] + elf[2]);

    free(elf);
    return 0;
}
