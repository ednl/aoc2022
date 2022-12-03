#include <stdio.h>   // fopen, fclose, getline, printf
#include <stdlib.h>  // free
#include <stdint.h>  // uint64_t

// Priority of item type
static int prio(const char item)
{
    if (item >= 'a' && item <= 'z')
        return item - 'a' + 1;   // [a..z] => [1..26]
    if (item >= 'A' && item <= 'Z')
        return item - 'A' + 27;  // [A..Z] => [27..58]
    return 0;  // error
}

// Set bit for each item in compartment (error=0, 'a'=1, 'b'=2, ..., 'A'=27, 'B'=28, ...)
static uint64_t index(const char* compartment, const ssize_t size)
{
    if (compartment == NULL || size <= 0)
        return 0;  // error
    uint64_t mask = 0;
    const char *item = compartment, *end = compartment + size;
    while (item != end)
        mask |= (1ull << prio(*item++));  // set bit in the correct index for this item type
    return mask;
}

// Get index number of highest priority item (0=error, 1='a', 2='b', ..., 27='A', 28='B', ...)
static int firstitem(uint64_t mask)
{
    int bit = 0;
    while (mask >>= 1)  // does not differentiate between mask=0 and mask=1 but both are errors
        bit++;
    return bit;
}

int main(void)
{
    FILE *f = fopen("input03.txt", "r");
    char *buf = NULL;
    size_t bufsz;
    ssize_t len;
    int i = 0, part1 = 0, part2 = 0;
    uint64_t badge = (uint64_t)-1;
    while ((len = getline(&buf, &bufsz, f)) > 1) {
        ssize_t half = len / 2;    // buf includes '\n' so len should be odd, but int div truncates
        uint64_t comp1 = index(buf, half);         // all item types in first compartment
        uint64_t comp2 = index(buf + half, half);  // all item types in second compartment
        part1 += firstitem(comp1 & comp2);         // item type that is in both compartments
        badge &= (comp1 | comp2);  // item types that are in either compartment of all rucksacks
        if (++i == 3) {            // badge is for every group of three rucksacks
            part2 += firstitem(badge);
            badge = (uint64_t)-1;  // re-init
            i = 0;
        }
    }
    fclose(f);
    free(buf);
    printf("%d\n%d\n", part1, part2);
    return 0;
}
