/**
 * Advent of Code 2022
 * Day 11: Monkey in the Middle
 * https://adventofcode.com/2022/day/11
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>     // fopen, fclose, fscanf, printf
#include <stdlib.h>    // atoi
#include <stdbool.h>   // bool
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include "startstoptimer.h"

#define MONKEYS (8)  // max number of monkeys
#define ITEMS   (8)  // max number of items each monkey has initially
#define QSIZE  (40)  // max number of items of all monkeys

typedef struct _Input {
    int size, q[ITEMS];
} Input;

typedef struct _Monkey {
    int id;                    // monkey number
    int activity;              // number of items inspected
    int op, param;             // operator: 0=add,1=mult,2=square
    int test;                  // divisibility test
    int size, head, tail;      // queue size, insert at head, remove from tail
    struct _Monkey *yes, *no;  // target monkeys for divisible=yes/no
    int64_t q[QSIZE];          // queue (circular buffer)
} Monkey;

static Input data[MONKEYS];
static Monkey monkey[MONKEYS];
static int M;           // number of monkeys in input (example=4, input=8)
static int64_t common;  // example=96577, input=8953560

static void read(const char *const name)
{
    M = 0;
    common = 1;
    int i, m = 0;
    FILE *f = fopen(name, "r");
    while (fscanf(f, "Monkey %d: Starting items:", &i) == 1) {
        Input *const a = data + i;
        a->size = 0;
        int k = ',';
        while (k == ',' && fscanf(f, " %d", &k) == 1 && a->size < ITEMS) {
            a->q[a->size++] = k;
            k = fgetc(f);
        }
        Monkey *const b = monkey + i;
        b->id = i;
        char c, buf[4];
        if (fscanf(f, " Operation: new = old %c %3s ", &c, buf) == 2)
            switch (c) {
                case '+':
                    b->op = 0;  // add
                    b->param = atoi(buf);
                    break;
                case '*':
                    if (buf[0] != 'o') {
                        b->op = 1;  // mult
                        b->param = atoi(buf);
                    } else
                        b->op = 2;  // square
                    break;
            }
        int test, yes, no;
        if (fscanf(f, " Test: divisible by %d"
            " If true: throw to monkey %d"
            " If false: throw to monkey %d ", &test, &yes, &no) == 3) {
            common *= (int64_t)test;  // LCM of all divisibility tests (all prime, so: product)
            b->test = test;
            b->yes = monkey + yes;  // pointer to monkey when divisibility test succeeds
            b->no = monkey + no;    // pointer to monkey when divisibility test fails
        }
        ++m;
    }
    fclose(f);
    M = m;  // set global number of monkeys in input
}

static void show(void)
{
    printf("\n");
    for (int i = 0; i < M; ++i) {
        Monkey *a = monkey + i;
        printf("%d: ", i);
        switch (a->op) {
            case 0: printf("+%2d", a->param); break;
            case 1: printf("*%2d", a->param); break;
            case 2: printf("^ 2"); break;
        }
        printf(" /%2d?->%d,%d (", a->test, a->yes->id, a->no->id);
        for (int j = 0; j < a->size; ++j) {
            if (j)
                printf(",");
            printf("%"PRId64, a->q[(a->tail + j) % QSIZE]);
        }
        printf(")\n");
    }
    printf("\n");
}

static int64_t pop(Monkey *const a)
{
    int64_t val = a->q[a->tail++];  // remove item from back of the queue
    a->tail %= QSIZE;               // adjust tail index
    a->size--;                      // one fewer in queue
    return val;
}

static void push(Monkey *const a, const int64_t val)
{
    a->q[a->head++] = val;  // add item to front of the queue
    a->head %= QSIZE;       // adjust head index
    a->size++;              // one more in queue
}

// Sort int array in descending order
static int descending(const void *a, const void *b)
{
    const int p = *(const int*)a;
    const int q = *(const int*)b;
    return (p < q) - (q < p);
}

static int64_t play(int rounds)
{
    // Reset
    for (int i = 0; i < M; ++i) {
        Input *const a = data + i;
        Monkey *const b = monkey + i;
        b->activity = b->tail = 0;
        b->head = b->size = a->size;
        for (int j = 0; j < a->size; ++j)
            b->q[j] = a->q[j];
    }
    // Play all rounds
    const bool ispart1 = rounds == 20;
    if (M == 4) {
        if (ispart1)
            show();  // show initial configuration of example, but only before part 1
        else
            printf("\n");  // extra newline between parts 1 & 2 of example
    }
    while (rounds--)
        for (int i = 0; i < M; ++i) {
            Monkey *const a = monkey + i;
            a->activity += a->size;
            while (a->size) {
                int64_t item = pop(a);
                switch (a->op) {
                    case 0: item += a->param; break;
                    case 1: item *= a->param; break;
                    case 2: item *= item; break;
                }
                if (ispart1)
                    item /= 3;       // part 1: worry level is divided by 3
                else
                    item %= common;  // found another way to keep my worry levels manageable
                push(item % a->test ? a->no : a->yes, item);
            }
        }
    // Which 2 monkeys were the most active?
    int arr[MONKEYS];
    for (int i = 0; i < M; ++i) {
        arr[i] = monkey[i].activity;
        if (M == 4)  // show activity of example
            printf("Monkey %d inspected items %d times.\n", i, arr[i]);
    }
    if (M == 4)
        show();  // show final configuration of example
    qsort(arr, (const size_t)M, sizeof(*arr), descending);
    return arr[0] * arr[1];
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
