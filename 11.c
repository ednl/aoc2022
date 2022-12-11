/**
 * Advent of Code 2022
 * Day 11: Monkey in the Middle
 * https://adventofcode.com/2022/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on Mac Mini M1 using Bash oneliner:
 *   for((i=0;i<20;++i));do ./a.out>/dev/null;done;for((i=0;i<10;++i));do ./a.out|tail -n1|awk '{print $2}';done
 * gives a runtime for my input file (not the example) of 4.4 ms.
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
#define RING   (40)  // max number of items of all monkeys added up

typedef struct {
    int activity;         // sum of all buffer lengths during play = number of items inspected
    int len, head, tail;  // content length, insert at head index, remove from tail index
    int64_t val[RING];    // circular buffer array
} Ring;

typedef struct _Monkey {
    char op;      // operator: 0=add,1=mult,2=square
    int param;    // parameter for add or mult
    int test;     // divisibility test number
    int yes, no;  // target monkey numbers for divisible=yes/no
} Monkey;

static Ring init[MONKEYS], item[MONKEYS];
static Monkey monkey[MONKEYS];
static int simcount;    // number of monkeys in input (example=4, input=8)
static int64_t common;  // LCM of div-test numbers (example=96577, input=8953560)

static void push(Ring *const r, const int64_t newval)
{
    if (r->len == RING)
        return;
    r->val[r->head++] = newval;  // add item to front of the queue
    r->head %= RING;             // adjust head index
    r->len++;                    // one more in ring buffer
}

static int64_t pop(Ring *const r)
{
    if (!r->len)
        return -1;
    int64_t oldval = r->val[r->tail++];  // remove item from back of ring buffer
    r->tail %= RING;                     // adjust tail index
    r->len--;                            // one fewer in ring buffer
    return oldval;
}

static void read(const char *const name)
{
    // Reset
    simcount = 0;
    common = 1;
    for (int i = 0; i < MONKEYS; ++i) {
        init[i] = (Ring){0};
        monkey[i] = (Monkey){0};
    }
    // Parse input
    Monkey *sim = monkey;
    Ring *it = init;
    FILE *f = fopen(name, "r");
    int val;
    while (simcount < MONKEYS && fscanf(f, "Monkey %d: Starting items:", &val) == 1) {
        int c = ',';
        while (it->len < RING && c == ',' && fscanf(f, " %d", &val) == 1) {
            push(it, val);  // also increments it->len
            c = fgetc(f);  // either another comma or else newline
        }
        char param[4];
        if (fscanf(f, " Operation: new = old %c %3s ", &sim->op, param) == 2) {
            if (param[0] == 'o') {
                sim->op = '^';
                sim->param = 2;  // only for display
            } else
                sim->param = atoi(param);
        }
        if (fscanf(f, " Test: divisible by %d"
            " If true: throw to monkey %d"
            " If false: throw to monkey %d ", &sim->test, &sim->yes, &sim->no) == 3)
            common *= sim->test;  // LCM of div-test numbers (all prime & unique, so: product)
        ++simcount;
        ++sim;  // next monkey
        ++it;   // next item collection
    }
    fclose(f);
}

static void show(void)
{
    printf("\n");
    const Monkey *sim = monkey;
    const Ring *it = item;
    for (int i = 0; i < simcount; ++i, ++sim, ++it) {
        printf("%d: %c%2d /%2d?->%d,%d (", i, (char)sim->op, sim->param, sim->test, sim->yes, sim->no);
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
    for (int i = 0; i < simcount; ++i)
        item[i] = init[i];
    // Display
    const bool isexample = simcount == 4;
    const bool ispart1 = rounds == 20;
    if (isexample) {
        if (ispart1)
            show();  // show initial configuration of example, but only before part 1
        else
            printf("\n");  // extra newline between parts 1 & 2 of example
    }
    // Play all rounds
    while (rounds--) {
        Monkey *sim = monkey;
        Ring *it = item;
        for (int i = 0; i < simcount; ++i, ++sim, ++it) {
            it->activity += it->len;
            while (it->len) {
                int64_t worry = pop(it);
                switch (sim->op) {
                    case '+': worry += sim->param; break;
                    case '*': worry *= sim->param; break;
                    case '^': worry *= worry; break;
                }
                if (ispart1)
                    worry /= 3;  // part 1: worry level is divided by 3
                else
                    worry %= common;  // found another way to keep my worry levels manageable
                push(item + (worry % sim->test ? sim->no : sim->yes), worry);
            }
        }
    }
    // Display
    if (isexample) {
        for (int i = 0; i < simcount; ++i)
            printf("Monkey %d inspected items %d times.\n", i, item[i].activity);
        show();  // show final configuration of example
    }
    // Which 2 monkeys were the most active?
    int a0, a1;
    a0 = a1 = item[0].activity;
    for (int i = 1; i < simcount; ++i) {
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
    printf("Time: %.1f ms\n", stoptimer_ms());
    return 0;
}
