/**
 * Advent of Code 2022
 * Day 11: Monkey in the Middle
 * https://adventofcode.com/2022/day/11
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>     // fopen, fclose, fscanf, printf
#include <stdlib.h>    // atoi
#include <stdbool.h>   // bool, true, false
#include <stdint.h>    // uint64_t, UINT64_C
#include <inttypes.h>  // PRIu64

#define MONKEYS    (8)
#define INI_ITEMS  (8)
#define ITEMS     (40)

typedef struct _Input {
    int size, q[INI_ITEMS];
} Input;

typedef struct _Monkey {
    int activity;        // number of items inspected
    int param, mod, op;  // op: 0=add,1=mult,2=square
    int to[2];           // to[0] = target for test=false, to[1] for test=true
    int head, tail, size, q[ITEMS];  // queue
} Monkey;

static Input data[MONKEYS];
static Monkey monkey[MONKEYS];
static int64_t common = 1;

static int input(const char *const name)
{
    int i, n = 0;
    FILE *f = fopen(name, "r");
    while (fscanf(f, "Monkey %d: Starting items:", &i) == 1) {
        Input *const a = data + i;
        Monkey *const b = monkey + i;
        int k = ',';
        while (k == ',' && fscanf(f, " %d", &k) == 1 && a->size < INI_ITEMS) {
            a->q[a->size++] = k;
            k = fgetc(f);
        }
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
        int by, isdiv, notdiv;
        if (fscanf(f, " Test: divisible by %d"
            " If true: throw to monkey %d"
            " If false: throw to monkey %d ", &by, &isdiv, &notdiv) == 3) {
            b->mod = by;
            b->to[0] = notdiv;  // not divisible => mod>0 => !(mod)=false => index=0
            b->to[1] = isdiv;   //  is divisible => mod=0 => !(mod)= true => index=1
        }
        n++;
    }
    fclose(f);
    return n;
}

static void reset(const int monkeys)
{
    for (int i = 0; i < monkeys; ++i) {
        Input *const a = data + i;
        Monkey *const b = monkey + i;
        for (int j = 0; j < a->size; ++j)
            b->q[j] = a->q[j];
        b->tail = 0;
        b->head = b->size = a->size;
    }
}

// static int input(const char *const name)
// {
//     int i, n = 0;
//     common = 1;
//     FILE *f = fopen(name, "r");
//     while (fscanf(f, "Monkey %d: Starting items:", &i) == 1) {
//         if (i < 0 || i >= MONKEYS)
//             break;
//         Monkey *const a = monkey + i;
//         a->activity = a->cur = a->len = 0;
//         int k = ',';
//         while (k == ',' && fscanf(f, " %d", &k) == 1 && a->len < ITEMS) {
//             a->q[a->len++] = (uint64_t)k;
//             k = fgetc(f);
//         }
//         if (a->len == ITEMS)
//             break;
//         a->ins = a->len;
//         char c, buf[4];
//         if (fscanf(f, " Operation: new = old %c %3s ", &c, buf) == 2) {
//             switch (c) {
//                 case '+': a->op = 0; break;
//                 case '*': a->op = buf[0] == 'o' ? 2 : 1; break;
//             }
//             a->param = (uint64_t)atoi(buf);
//         }
//         size_t factor, isdiv, notdiv;
//         if (fscanf(f, " Test: divisible by %zu"
//             " If true: throw to monkey %zu"
//             " If false: throw to monkey %zu ", &factor, &isdiv, &notdiv) == 3) {
//             a->mod = factor;
//             common *= factor;
//             a->to[0] = notdiv;  // not divisible => mod>0 => !(mod)=false => index=0
//             a->to[1] = isdiv;   //  is divisible => mod=0 => !(mod)= true => index=1
//         }
//         n++;
//     }
//     fclose(f);
//     return n;
// }

static void show(const int monkeys)
{
    for (int i = 0; i < monkeys; ++i) {
        Monkey *a = monkey + i;
        printf("%d: ", i);
        switch (a->op) {
            case 0: printf("+%2"PRIu64, a->param); break;
            case 1: printf("*%2"PRIu64, a->param); break;
            case 2: printf("^ 2"); break;
        }
        printf(" /%2"PRIu64"->%zu,%zu (", a->mod, a->to[1], a->to[0]);
        for (size_t j = 0; j < a->len; ++j) {
            if (j)
                printf(",");
            printf("%"PRIu64, a->q[(a->cur + j) % ITEMS]);
        }
        printf(")\n");
    }
}

static void round(const int monkeys, bool part1)
{
    for (int i = 0; i < monkeys; ++i) {
        Monkey *const a = monkey + i;
        a->activity += a->len;
        while (a->len) {
            uint64_t item = a->q[a->cur++];
            a->cur %= ITEMS;
            a->len--;
            switch (a->op) {
                case 0: item += a->param; break;
                case 1: item *= a->param; break;
                case 2: item *= item; break;
            }
            if (part1)
                item /= 3;
            else
                item %= common;
            Monkey *const b = monkey + a->to[!(item % a->mod)];  // is divisible => mod=0 => !(mod)=true => index=1
            b->q[b->ins++] = item;
            b->ins %= ITEMS;
            b->len++;
        }
    }
}

// Sort uint64_t array in descending order
static int descending(const void *a, const void *b)
{
    const uint64_t p = *(const uint64_t*)a;
    const uint64_t q = *(const uint64_t*)b;
    return (p < q) - (q < p);
}

static uint64_t active(const int monkeys)
{
    uint64_t arr[MONKEYS];
    for (int i = 0; i < monkeys; ++i) {
        arr[i] = monkey[i].activity;
        printf("Monkey %d inspected items %"PRIu64" times.\n", i, arr[i]);
    }
    qsort(arr, (const size_t)monkeys, sizeof(*arr), descending);
    return arr[0] * arr[1];
}

int main(void)
{
    int monkeys = input("input11.txt");
    show(monkeys);
    for (int i = 1; i <= 20; ++i)
        round(monkeys, true);
    printf("Part 1: %"PRIu64"\n", active(monkeys));  // example=10605, input=118674

    monkeys = input("input11.txt");
    for (int i = 1; i <= 10000; ++i)
        round(monkeys, false);
    printf("Part 2: %"PRIu64"\n", active(monkeys));  // example=2713310158, input=32333418600
    return 0;
}
