#include <stdio.h>   // fopen, fclose, getline, printf
#include <stdlib.h>  // free
#include <unistd.h>  // isatty, fileno

// Read from stdin if input is piped or redirected, otherwise from file
// Returns: signal size = length of first line (without line delimiter \n)
static int input(char **buf)
{
    size_t bufsz;
    ssize_t len;
    if (isatty(fileno(stdin))) {
        FILE *f = fopen("input06.txt", "r");
        if (!f)
            return 0;
        len = getline(buf, &bufsz, f);
        fclose(f);
    } else {
        len = getline(buf, &bufsz, stdin);
    }
    return len > 1 ? (int)(len - 1) : 0;
}

// Look at substring with char index from [start - 1] to [start - len]
// Returns: how many chars to skip ahead for next check
// E.g. for len == 4:
//   if last 2 chars are the same then skip ahead len - 1 = 3
//   else if any of the last 3 chars are the same, then skip ahead len - 2 = 2
//   else if any of the last 4 chars are the same, then skip ahead len - 3 = 1
//   else start-of-message marker is found (skip=0)
static int skip(const char *signal, const int len, const int start)
{
    for (int i = 2; i <= len; ++i) {
        char c = signal[start - i];  // compare to char at index [start - 2] to [start - len]
        for (int k = 1; k < i; ++k)  // zero-based, so last char index in window = [start - 1]
            if (signal[start - k] == c)
                return len + 1 - i;
    }
    return 0;  // found start-of-message
}

// Find start-of-message = index of next char after header of length len with no duplicate chars
static int find(const char *signal, const int size, const int len)
{
    static int prev = 0;      // value of len on previous function call
    static int start = 0;     // start-of-message index value

    if (signal == NULL || size == 0 || len == 0 || size < len)
        return 0;             // error
    if (prev == 0 || prev > len || start < len)
        start = len;          // reset to first possible start-of-message
    else
        start += len - prev;  // next possible start-of-message
    prev = len;               // remember len for next function call

    int i;
    while (start < size && (i = skip(signal, len, start)))  // loop until skip==0
        start += i;
    return start < size ? start : 0;  // 0=not found
}

int main(void)
{
    char *signal = NULL;
    int N = input(&signal);
    printf("Part 1: %d\n", find(signal, N,  4));  // 1542
    printf("Part 2: %d\n", find(signal, N, 14));  // 3153
    free(signal);
    return 0;
}
