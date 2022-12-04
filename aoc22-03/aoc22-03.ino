#include "input03.h"

// Priority of item type
static char prio(const char item)
{
    if (item >= 'a' && item <= 'z')
        return item - 'a' + 1;   // [a..z] => [1..26]
    if (item >= 'A' && item <= 'Z')
        return item - 'A' + 27;  // [A..Z] => [27..58]
    return 0;  // error
}

// Set bit for each item in compartment (error=0, 'a'=1, 'b'=2, ..., 'A'=27, 'B'=28, ...)
static uint64_t index(const char* compartment, const byte size)
{
    if (compartment == NULL || size == 0)
        return 0;  // error
    uint64_t mask = 0;
    const char *item = compartment, *end = compartment + size;
    while (item != end)
        mask |= (1ull << prio(*item++));  // set bit in the correct index for this item type
    return mask;
}

// Get highest priority item type from index (0=error, 1='a', 2='b', ..., 27='A', 28='B', ...)
static byte firstitem(uint64_t mask)
{
    byte bit = 0;  // bit index where LSB=0
    while (mask >>= 1)  // does not differentiate between mask=0 and mask=1 but both are errors
        bit++;
    return bit;
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(9600);
    while (!Serial);
    unsigned long t0 = millis();

    int i = 0, part1 = 0, part2 = 0, N = strlen_P(data);
    byte j = 0;
    uint64_t badge = (uint64_t)-1;  // badge has all item types initially
    char buf[64];
    while (i < N) {
        byte len = 0;
        while (i < N && len < 64) {
            char c = pgm_read_byte_near(data + i++);
            if (c == '\n')
                break;
            buf[len++] = c;
        }
        if (len) {
            byte half = len >> 1;
            uint64_t comp1 = index(buf, half);         // all item types in first compartment
            uint64_t comp2 = index(buf + half, half);  // all item types in second compartment
            part1 += firstitem(comp1 & comp2);         // item type that is in both compartments
            badge &= (comp1 | comp2);   // item types that are in either compartment of all rucksacks
            if (++j == 3) {             // badge is for every group of three rucksacks
                part2 += firstitem(badge);
                badge = (uint64_t)-1;   // re-init
                j = 0;
            }
        }
    }

    unsigned long dt = millis() - t0;
    Serial.print(F("Part 1: "));
    Serial.println(part1);
    Serial.print(F("Part 2: "));
    Serial.println(part2);
    Serial.print(F("Time: "));
    Serial.print(dt);
    Serial.println(F(" ms"));

    digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
    delay(1000);
}
