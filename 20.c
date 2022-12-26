#include <stdio.h>

static const int data[] = {1, 2, -3, 3, -2, 0, 4};
static const int len = sizeof(data) / sizeof(*data), len1 = len - 1;
static int pos[len], next[len], prev[len];

int main(void)
{
    for (int i = 0; i < len; ++i) {
        pos[i] = i;
        next[i] = i + 1;
        prev[i] = i - 1;
    }

    for (int i = 0; i < len; ++i) {
        int n = data[pos[i]];
        if (n < 0)
            n = (n % len1) + len1;
        pos[i] = (pos[i] + n) % len;
    }

    printf("-7 %% 4 = %d\n", -7 % 4);

    return 0;
}
