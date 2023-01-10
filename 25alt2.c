/**
 * Advent of Code 2022
 * Day 25: Full of Hot Air (alt 2)
 * https://adventofcode.com/2022/day/25
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * gives a shortest runtime for my input file (not the example) of 34 µs.
 * On a Raspberry Pi 4 with the CPU in performance mode: ?? µs.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>
#include <string.h>
#include "startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "example25.txt"
#else
#define NAME "input25.txt"
#endif

#define N (32)
#define F "31"

// add[x][y][0] = sum of two digits x and y
// add[x][y][1] = carry/borrow for sum of two digits x and y
static const char add[5][5][2] = {
    {"1-", "2-", "=0", "-0", "00"}, // =+=, =+-, =+0, =+1, =+2
    {"2-", "=0", "-0", "00", "10"}, // -+=, -+-, -+0, -+1, -+2
    {"=0", "-0", "00", "10", "20"}, // 0+=, 0+-, 0+0, 0+1, 0+2
    {"-0", "00", "10", "20", "=1"}, // 1+=, 1+-, 1+0, 1+1, 1+2
    {"00", "10", "20", "=1", "-1"}, // 2+=, 2+-, 2+0, 2+1, 2+2
};

static size_t rev(const char* const src, char* const dst)
{
    size_t i = strlen(src), j = 0;
    while (i)
        dst[j++] = src[--i];
    dst[j] = '\0';
    return j;
}

static size_t ix(const char snafudigit)
{
    switch (snafudigit) {
        case '-': return 1;
        case '0': return 2;
        case '1': return 3;
        case '2': return 4;
        default : return 0;
    }
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;

    char buf[N], num[N], sum[N] = {0}, a, c = '0';
    size_t n, m = 0, i, j, k;
    while (fscanf(f, "%"F"s", buf) == 1) {
        n = rev(buf, num);
        i = 0;
        while (i < n || i < m) {
            j = i < n ? ix(num[i]) : ix('0');
            k = i < m ? ix(sum[i]) : ix('0');
            a = add[j][k][0];
            char cc = add[j][k][1];
            j = ix(a);
            k = ix(c);
            a = add[j][k][0];
            j = ix(add[j][k][1]);
            k = ix(cc);
            c = add[j][k][0];
            sum[i++] = a;
        }
        if (c != '0')
            sum[i++] = c;
        if (i > m)
            m = i;
    }
    fclose(f);
    rev(sum, num);
    printf("Part 1: %s\n", num);  // example="2=-1=0" (4890), input="2-=2-0=-0-=0200=--21"
    printf("Time: %.0f us\n", stoptimer_us());  // M1: 34 µs, Pi4: ?? µs
    return 0;
}
