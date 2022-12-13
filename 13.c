/**
 * Advent of Code 2022
 * Day 13: Distress Signal
 * https://adventofcode.com/2022/day/13
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   for((i=0;i<50;++i));do ./a.out>/dev/null;done;for((i=0;i<10;++i));do ./a.out|tail -n1|awk '{print $2}';done|sort -n|head -n1
 * gives a shortest runtime for my input file (not the example) of 463 us.
 * On a Raspberry Pi 4 with the CPU in performance mode: [TBD] ms.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>   // getline
#include <stdlib.h>  // free
#include <stdbool.h>
#include <ctype.h>   // isdigit
#include "startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "example13.txt"
#define N (16)  // 16 packets in example file
#else
#define NAME "input13.txt"
#define N (300)  // 300 packets in input file
#endif

#define M (N + 2)  // plus 2 divider packets

typedef struct _Elm {
    struct _Elm* next;  // next element on same level, next=NULL: last element
    struct _Elm* list;  // list=NULL: this is a value or empty list, list!=NULL: this is a list
    int value;  // -1: empty list, >=0: value
    bool divider;
} Elm;

static Elm* packet[M];  // room for all packets in input file plus 2 divider packets
static int refcount;

static Elm* create(Elm* const prev)
{
    Elm* e = calloc(1, sizeof(Elm));
    if (e)
        ++refcount;
    if (prev)            // there is another
        prev->next = e;  // continue linked list
    return e;
}

static void delete(Elm* e)
{
    if (!e)
        return;
    delete(e->list);
    delete(e->next);
    free(e);
    --refcount;
}

static Elm* parse(char** ps)
{
    Elm* first = create(NULL);                // start with new element (delete if empty list)
    if (!first)
        return NULL;
    Elm* cur = first, *prev = NULL;
    int x, len = 0;
    while (**ps != '\n' && **ps != '\0') {    // should return earlier from ']'
        switch (*(*ps)++) {                   // also go to next char
        case '[':                             // new element is a list
            ++len;
            if (!(cur->list = parse(ps)))     // parse sub list, can be empty
                cur->value = -1;              // element is empty list
            break;
        case ']':                             // end of list
            if (!len) {
                free(first);
                --refcount;
                return NULL;
            }
            return first;                     // return link to first element
        case ',':                             // there is another
            prev = cur;                       // remember current as previous
            cur = create(prev);               // prepare new element with link to previous
            if (!cur)
                return first;
            break;
        default:                              // by now it must be a number
            ++len;
            x = *(*ps - 1) - '0';             // adjust for earlier ++, convert to value
            while (isdigit(**ps))             // check next char
                x = x * 10 + *(*ps)++ - '0';  // add digit to value, step to next char
            cur->value = x;
            break;
        }
    }
    return first;                            // return element that is always created
}

static void val2list(Elm* const e)
{
    if (!e)
        return;
    if (e->list || e->value == -1)  // is already a list
        return;
    if (!(e->list = create(NULL)))  // out of memory
        return;
    e->list->value = e->value;
    e->value = 0;
}

static int cmp(Elm* const l, Elm* const r)
{
    if (!l && !r)                          // left and right have both run out
        return 0;                          // list is equal, check later if sub list
    else if (!l && r)                      // left has run out, right has not
        return -1;                         // sorted (left < right)
    else if (l && !r)                      // left has not run out, right has
        return 1;                          // not sorted (left > right)

    bool ll = l->list || l->value == -1;   // is left a list? (can be empty)
    bool rl = r->list || r->value == -1;   // is right a list? (can be empty)

    if (!ll && rl) {                       // left is not a list, right is
        val2list(l);                       // make left a list
        ll = true;
    } else if (ll && !rl) {                // left is a list, right is not
        val2list(r);                       // make right a list
        rl = true;
    }

    if (ll && rl) {                        // left and right are both lists
        int c = cmp(l->list, r->list);     // check sub list
        if (c)                             // only return if sub lists not equal
            return c;                      // sorted or not sorted (left <> right)
    } else {                               // left and right are both values
        if (l->value < r->value)           // left value is smaller than right value
            return -1;                     // sorted (left < right)
        else if (l->value > r->value)      // left value is greater than right value
            return 1;                      // not sorted (left > right)
    }
    return cmp(l->next, r->next);          // compare next elements in the list
}

static int qcmp(const void* a, const void* b)
{
    Elm* const l = *(Elm* const *)a;
    Elm* const r = *(Elm* const *)b;
    return cmp(l, r);
}

#if EXAMPLE == 1
static void showlist(const Elm* const e)
{
    if (!e)
        return;
    if (e->list || e->value == -1) {
        printf("[");
        showlist(e->list);
        printf("]");
    } else
        printf("%d", e->value);
    if (e->next) {
        printf(",");
        showlist(e->next);
    }
}

static void showpacket(int i)
{
    if (i < 0 || i >= M)
        return;
    printf("%3d: ", i + 1);
    if (packet[i]->divider)
        printf("----- ");
    showlist(packet[i]);
    printf("\n");
}
#endif

static int read(const char* const name)
{
    FILE* f = fopen(name, "r");
    if (!f)
        return 0;
    int i = 0;
    char* line = NULL;
    size_t bufsz = 0;
    ssize_t len;
    while (i < N && (len = getline(&line, &bufsz, f)) > 0) {
        if (len == 1)
            continue;
        char* s = line;
        packet[i++] = parse(&s);
    }
    fclose(f);
    free(line);
    return i;
}

static Elm* listlist(int value)
{
    Elm* e = create(NULL);
    if (!e)
        return NULL;
    e->divider = true;
    if ((e->list = create(NULL)))
        if ((e->list->list = create(NULL)))
            e->list->list->value = value;
    return e;
}

int main(void)
{
    starttimer();
    int n = read(NAME);
#if EXAMPLE == 1
    for (int i = 0; i < n; ++i)
        showpacket(i);
#endif

    int part1 = 0;
    for (int i = 0; i < n; i += 2)
        if (cmp(packet[i], packet[i + 1]) == -1)
            part1 += (i >> 1) + 1;
    printf("Part 1: %d\n", part1);  // example=13, input=5717

    packet[n++] = listlist(2);
    packet[n++] = listlist(6);
#if EXAMPLE == 1
    printf("\n");
    showpacket(n - 2);
    showpacket(n - 1);
    printf("\n");
#endif

    qsort(packet, (size_t)n, sizeof(*packet), qcmp);
#if EXAMPLE == 1
    for (int i = 0; i < n; ++i)
        showpacket(i);
#endif

    int part2 = 1;
    for (int i = 0; i < n; ++i)
        if (packet[i]->divider)
            part2 *= i + 1;
    printf("Part 2: %d\n", part2);  // example=140, input=25935

    for (int i = 0; i < n; ++i)
        delete(packet[i]);
    if (refcount != 0)
        printf("MEMORY LEAK!\n");

    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
