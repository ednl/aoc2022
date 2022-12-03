#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *f = fopen("input03.txt", "r");
    char *buf = NULL, *c2, *c3, *p, *q;
    size_t bufsz;
    ssize_t len;
    int sum = 0;
    while ((len = getline(&buf, &bufsz, f)) != -1) {
        p = buf;             // start of compartment 1
        c2 = buf + len / 2;  // start of compartment 2
        c3 = buf + len - 1;  // end of compartment 2
        do {
            q = c2;
            while (*p != *q && q++ != c3);
        } while (*p != *q && p++ != c2);
        if (*p == *q) {
            int prio = *p > 'Z' ? *p - 'a' + 1 : *p - 'A' + 27;
            sum += prio;
        } else {
            printf("buf=%s len=%zd c2=%c c3=%c p=%c q=%c\n", buf, len, *c2, *c3, *p, *q);
            break;
        }
    }
    fclose(f);
    free(buf);
    printf("Part 1: %d\n", sum);
    return 0;
}
