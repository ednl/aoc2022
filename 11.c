/**
 * Advent of Code 2022
 * Day 11: Monkey in the Middle
 * https://adventofcode.com/2022/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   for((i=0;i<20;++i));do ./a.out>/dev/null;done;for((i=0;i<10;++i));do ./a.out|tail -n1|awk '{print $2}';done|sort -n|head -n1
 * gives a shortest runtime for my input file (not the example) of 2.08 ms.
 * On a Raspberry Pi 4 with the CPU in performance mode: 11.6 ms.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>     // fopen, fclose, fscanf, printf
#include <stdlib.h>    // atoi
#include <stdbool.h>   // bool
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include "startstoptimer.h"

#define MONKEYS (8)  // max number of monkeys
#define ITEMS  (32)  // max number of items per monkey (power of 2=fast)

typedef struct _Monkey {
    int64_t item[ITEMS];  // buffer array
    int len;              // buffer content length
    int activity;         // sum of all buffer lengths during play = number of items inspected
    int param;            // parameter for add or mult
    int test;             // divisibility test number
    struct _Monkey *yes, *no;  // target monkey numbers for divisible=yes/no
    char op;              // operator: 0=add,1=mult,2=square
} Monkey;

static int item[MONKEYS][ITEMS], len[MONKEYS];
static Monkey monkey[MONKEYS];
static int monkeycount;  // number of monkeys in input (example=4, input=8)
static int64_t common;   // LCM of div-test numbers (example=96577, input=8953560)

static void read(const char *const name)
{
    // Reset
    monkeycount = 0;
    common = 1;
    for (int i = 0; i < MONKEYS; ++i)
        len[i] = 0;
    // Parse input
    char param[4];
    Monkey *m = monkey;
    FILE *f = fopen(name, "r");
    while (!feof(f)) {
        for (int i = 0; i < 27; ++i)
            fgetc(f);  // "Monkey X:\n  Starting items:"
        int c = ',';
        while (c == ',') {
            fscanf(f, "%d", &c);
            item[monkeycount][len[monkeycount]++] = c;
            c = fgetc(f);  // either another comma or else newline
            fgetc(f);      // space
        }
        for (int i = 0; i < 22; ++i)
            fgetc(f);  // " Operation: new = old "
        fscanf(f, "%c %3s", &m->op, param);
        if (param[0] == 'o') {
            m->op = '^';
            m->param = 2;  // only for display
        } else
            m->param = atoi(param);
        for (int i = 0; i < 22; ++i)
            fgetc(f);  // "\n  Test: divisible by "
        fscanf(f, "%d", &m->test);
        common *= m->test;  // LCM of div-test numbers (all prime & unique, so: product)
        for (int i = 0; i < 30; ++i)
            fgetc(f);  // "\n    If true: throw to monkey "
        fscanf(f, "%d", &c);
        m->yes = monkey + c;
        for (int i = 0; i < 31; ++i)
            fgetc(f);  // "\n    If false: throw to monkey "
        fscanf(f, "%d", &c);
        m->no = monkey + c;
        fgetc(f);  // newline
        fgetc(f);  // blank line
        ++monkeycount;
        ++m;  // next monkey
    }
    fclose(f);
}

static void show(void)
{
    printf("\n");
    const Monkey *m = monkey;
    for (int i = 0; i < monkeycount; ++i, ++m) {
        printf("%d: %c%2d /%2d?->%d,%d (", i, m->op, m->param, m->test, (int)(m->yes - monkey), (int)(m->no - monkey));
        for (int j = 0; j < m->len; ++j) {
            if (j)
                printf(",");
            printf("%"PRId64, m->item[j]);
        }
        printf(")\n");
    }
    printf("\n");
}

static int64_t play(int rounds)
{
    // Reset
    for (int i = 0; i < monkeycount; ++i) {
        monkey[i].activity = 0;
        monkey[i].len = len[i];
        for (int j = 0; j < len[i]; ++j)
            monkey[i].item[j] = item[i][j];
    }
    // Display
    const bool example = monkeycount == 4;
    const bool part2 = rounds != 20;
    if (example) {
        if (part2)
            printf("\n");  // extra newline between parts 1 & 2 of example
        else
            show();  // show initial configuration of example, but only before part 1
    }
    // Play all rounds (replicate code to avoid branch)
    if (part2) {
        for (; rounds; --rounds) {
            Monkey *m = monkey;
            for (int i = 0; i < monkeycount; ++i, ++m) {
                m->activity += m->len;
                for (int j = 0; j < m->len; ++j) {
                    int64_t worry = m->item[j];
                    switch (m->op) {
                        case '+': worry += m->param; break;
                        case '*': worry *= m->param; break;
                        case '^': worry *= worry; break;
                    }
                    if (worry >= common)
                        worry %= common;  // part 2: found another way to keep my worry levels manageable
                    Monkey *const dst = worry % m->test ? m->no : m->yes;
                    dst->item[dst->len++] = worry;
                }
                m->len = 0;
            }
        }
    } else {
        for (; rounds; --rounds) {
            Monkey *m = monkey;
            for (int i = 0; i < monkeycount; ++i, ++m) {
                m->activity += m->len;
                for (int j = 0; j < m->len; ++j) {
                    int64_t worry = m->item[j];
                    switch (m->op) {
                        case '+': worry += m->param; break;
                        case '*': worry *= m->param; break;
                        case '^': worry *= worry; break;
                    }
                    worry /= 3;  // part 1: worry level is divided by 3
                    Monkey *const dst = worry % m->test ? m->no : m->yes;
                    dst->item[dst->len++] = worry;
                }
                m->len = 0;
            }
        }
    }
    // Display
    if (example) {
        for (int i = 0; i < monkeycount; ++i)
            printf("Monkey %d inspected items %d times.\n", i, monkey[i].activity);
        show();  // show final configuration of example
    }
    // Which 2 monkeys were the most active?
    int a0 = monkey[0].activity, a1 = 0;
    for (int i = 1; i < monkeycount; ++i) {
        const int a2 = monkey[i].activity;
        if (a2 > a0) {
            a1 = a0;
            a0 = a2;
        } else if (a2 > a1)
            a1 = a2;
    }
    return (int64_t)a0 * a1;
}

int main(void)
{
    printf("---------- EXAMPLE ----------\n");
    read("aocinput/2022-11-example.txt");
    printf("Part 1: %"PRId64"\n", play(20));     // 10605
    printf("Part 2: %"PRId64"\n", play(10000));  // 2713310158

    starttimer();
    printf("\n---------- INPUT FILE ----------\n");
    read("aocinput/2022-11-input.txt");
    printf("Part 1: %"PRId64"\n", play(20));     // 118674
    printf("Part 2: %"PRId64"\n", play(10000));  // 32333418600
    printf("Time: %.2f ms\n", stoptimer_ms());
    return 0;
}
