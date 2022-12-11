/**
 * Advent of Code 2022
 * Day 11: Monkey in the Middle
 * https://adventofcode.com/2022/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on Mac Mini M1 using Bash oneliner:
 *   for((i=0;i<20;++i));do ./a.out>/dev/null;done;for((i=0;i<10;++i));do ./a.out|tail -n1|awk '{print $2}';done
 * gives a runtime for my input file (not the example) of 3.6 ms.
 * Same on a Raspberry Pi 4 with the CPU in performance mode: 23.0 ms.
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
#define RING   (32)  // max number of items per monkey (power of 2=fast)

typedef struct {
    int activity;         // sum of all buffer lengths during play = number of items inspected
    int len, head, tail;  // content length, insert at head index, remove from tail index
    int64_t val[RING];    // circular buffer array
} Ring;

typedef struct _Monkey {
    Ring *yes, *no;  // target monkey numbers for divisible=yes/no
    int param;       // parameter for add or mult
    int test;        // divisibility test number
    char op;         // operator: 0=add,1=mult,2=square
} Monkey;

static Ring init[MONKEYS], item[MONKEYS];
static Monkey monkey[MONKEYS];
static int monkeys;     // number of monkeys in input (example=4, input=8)
static int64_t common;  // LCM of div-test numbers (example=96577, input=8953560)

static void read(const char *const name)
{
    // Reset
    monkeys = 0;
    common = 1;
    // Parse input
    char param[4];
    Monkey *m = monkey;
    Ring *it = init;
    FILE *f = fopen(name, "r");
    while (!feof(f)) {
        for (int i = 0; i < 27; ++i)
            fgetc(f);  // "Monkey X:\n  Starting items:"
        *it = (Ring){0};
        int c = ',';
        while (c == ',') {
            fscanf(f, "%d", &c);
            // push(it, c);   // also increments it->len
            it->val[it->head++] = c;
            c = fgetc(f);  // either another comma or else newline
            fgetc(f);      // space
        }
        it->len = it->head;
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
        m->yes = item + c;
        for (int i = 0; i < 31; ++i)
            fgetc(f);  // "\n    If false: throw to monkey "
        fscanf(f, "%d", &c);
        m->no = item + c;
        fgetc(f);  // newline
        fgetc(f);  // blank line
        ++monkeys;
        ++m;  // next monkey
        ++it;   // next item collection
    }
    fclose(f);
}

static void show(void)
{
    printf("\n");
    const Monkey *m = monkey;
    const Ring *it = item;
    for (int i = 0; i < monkeys; ++i, ++m, ++it) {
        printf("%d: %c%2d /%2d?->%d,%d (", i, m->op, m->param, m->test, (int)(m->yes - item), (int)(m->no - item));
        for (int j = 0; j < it->len; ++j) {
            if (j)
                printf(",");
            printf("%"PRId64, it->val[(it->tail + j) % RING]);
        }
        printf(")\n");
    }
    printf("\n");
}

static int64_t play(int rounds)
{
    // Reset
    for (int i = 0; i < monkeys; ++i)
        item[i] = init[i];
    // Display
    const bool example = monkeys == 4;
    const bool part2 = rounds != 20;
    if (example) {
        if (part2)
            printf("\n");  // extra newline between parts 1 & 2 of example
        else
            show();  // show initial configuration of example, but only before part 1
    }
    // Play all rounds
    while (rounds--) {
        Monkey *m = monkey;
        Ring *it = item;
        for (int i = 0; i < monkeys; ++i, ++m, ++it) {
            it->activity += it->len;
            while (it->len) {
                int64_t worry = it->val[it->tail++];  // remove item from back of ring buffer
                it->tail %= RING;                     // adjust tail index
                it->len--;                            // one fewer in ring buffer
                switch (m->op) {
                    case '+': worry += m->param; break;
                    case '*': worry *= m->param; break;
                    case '^': worry *= worry; break;
                }
                if (part2)
                    worry %= common;  // found another way to keep my worry levels manageable
                else
                    worry /= 3;  // part 1: worry level is divided by 3
                Ring *const dst = worry % m->test ? m->no : m->yes;
                dst->val[dst->head++] = worry;  // add item to front of the queue
                dst->head %= RING;              // adjust head index
                dst->len++;                     // one more in ring buffer
            }
        }
    }
    // Display
    if (example) {
        for (int i = 0; i < monkeys; ++i)
            printf("Monkey %d inspected items %d times.\n", i, item[i].activity);
        show();  // show final configuration of example
    }
    // Which 2 monkeys were the most active?
    int a0 = item[0].activity, a1 = 0;
    for (int i = 1; i < monkeys; ++i) {
        const int a2 = item[i].activity;
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
    read("example11.txt");
    printf("Part 1: %"PRId64"\n", play(20));     // 10605
    printf("Part 2: %"PRId64"\n", play(10000));  // 2713310158

    starttimer();
    printf("\n---------- INPUT FILE ----------\n");
    read("input11.txt");
    printf("Part 1: %"PRId64"\n", play(20));     // 118674
    printf("Part 2: %"PRId64"\n", play(10000));  // 32333418600
    printf("Time: %.2f ms\n", stoptimer_ms());
    return 0;
}
