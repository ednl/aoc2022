/**
 * Advent of Code 2022
 * Day 15: Beacon Exclusion Zone
 * https://adventofcode.com/2022/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * gives a shortest runtime for my input file (not the example) of 26 µs.
 * On a Raspberry Pi 4 with the CPU in performance mode: 124 µs.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>
#include <stdlib.h>    // abs
#include <stdbool.h>
#include <stdint.h>    // int64_t, INT64_C
#include <inttypes.h>  // PRId64
#include "startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "example15.txt"
#define N (14)   // number of sensors in example file
#define Y (10)   // horizontal line of interest in example
#define M (20)   // max x or y value for part 2
#else
#define NAME "input15.txt"
#define N (38)       // number of sensors in input file
#define Y (2000000)  // horizontal line of interest in input
#define M (4000000)  // max x or y value for part 2
#endif

#define TUNING (INT64_C(4000000))

typedef struct {
    int x, y;
} Vec;

typedef struct {
    Vec l, r;
} Square;

static Vec sensor[N];
static Vec beacon[N];
static int range[N], beacons;
static Vec seg[N];  // segments on line y=Y covered by a sensor from x=.x to x=.y
static Square square[N];

// Rotate left by 45 deg + dilation by sqrt(2)
// [[1,1],[-1,1]].(x,y) = (x+y,-x+y) = (x+y,y-x)
static Vec rotleft(const Vec v)
{
    return (Vec){v.x + v.y, v.y - v.x};
}

// Rotate right by 45 deg + contraction by sqrt(2)
//    [[1,-1],[1,1]].(x  ,y  )/2 = (x-y    ,x+y    )/2
// or [[1,-1],[1,1]].(x+y,y-x)/2 = (x+y+x-y,x+y-x+y)/2 = (2x,2y)/2 = (x,y)
static Vec rotright(const Vec v)
{
    return (Vec){(v.x - v.y) >> 1, (v.x + v.y) >> 1};
}

// Manhattan distance
static int manh(const Vec a, const Vec b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Sign of x: x > 0 => +1, x == 0 => 0, x < 0 => -1
static int sign(int x)
{
    return (x > 0) - (x < 0);
}

// Sign, element-wise
static Vec signdot(const Vec v)
{
    return (Vec){sign(v.x), sign(v.y)};
}

static Vec sub(const Vec v, const Vec w)
{
    return (Vec){v.x - w.x, v.y - w.y};
}

static bool equal(const Vec v, const Vec w)
{
    return v.x == w.x && v.y == w.y;
}

// Overlap for ranges vx..vy and wx..wy where (x,y) sorted in each pair
static bool overlap(const Vec v, const Vec w)
{
    return v.x <= w.y && v.y >= w.x;
}

static int cmp_int(const void* p, const void* q)
{
    return sign(*(const int*)p - *(const int*)q);
}

// Deduplicate array of ints
// Returns: number of remaining (unique) ints
static int dedup_int(int* const arr, const int len)
{
    if (arr == NULL || len <= 0)
        return 0;
    qsort(arr, (size_t)len, sizeof(*arr), cmp_int);
    int i = 0, j = 0;
    while (j < len) {
        while (++j < len && arr[i] == arr[j]);  // find first a[j] != a[i]
        if (++i != j && j < len)  // shift a[j] up if skipped (order of conditions is important because of ++)
            arr[i] = arr[j];
    }
    return i;
}

static int cmp_yx(const void* p, const void* q)
{
    const Vec s = signdot(sub(*(const Vec*)p, *(const Vec*)q));
    if (s.y)
        return s.y;  // first sort on vy < wy
    return s.x;      // then sort on vx < wx
}

// Deduplicate array of position vectors
// Returns: number of remaining (unique) vectors
static int dedup_vec(Vec* const arr, const int n)
{
    if (arr == NULL || n <= 0)
        return 0;
    qsort(arr, (size_t)n, sizeof(*arr), cmp_yx);
    int i = 0, j = 0;
    while (j < n) {
        while (++j < n && equal(arr[i], arr[j]));  // find first a[j] != a[i]
        if (++i != j && j < n)  // shift a[j] up if skipped (order of conditions is important because of ++)
            arr[i] = arr[j];
    }
    return i;
}

static int cmp_xy(const void* p, const void* q)
{
    const Vec s = signdot(sub(*(const Vec*)p, *(const Vec*)q));
    if (s.x)
        return s.x;  // first sort on vx < wx
    return -s.y;     // then sort on vy > wy
}

// Join array of segments where overlapping
// Returns: number of remaining (disjoint) segments
static int join(Vec* const arr, const int n)
{
    if (arr == NULL || n <= 0)
        return 0;
    qsort(arr, (size_t)n, sizeof(*arr), cmp_xy);
    int i = 0, j = 0;
    while (j < n) {
        while (++j < n && overlap(arr[i], arr[j])) {  // find first a[j] with no overlap on a[i]
            if (arr[i].y < arr[j].y)  // adjust upper limit of range
                arr[i].y = arr[j].y;
        }
        if (++i != j && j < n)  // shift a[j] up if skipped (order of conditions is important because of ++)
            arr[i] = arr[j];
    }
    return i;
}

// Return number of covered segments on line y=Y (possibly overlapping)
static void read(const char* const name)
{
    int i = 0;
    FILE* f = fopen(name, "r");
    while (i < N && fscanf(f,
        "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d ",
        &sensor[i].x, &sensor[i].y, &beacon[i].x, &beacon[i].y) == 4) {
        range[i] = manh(sensor[i], beacon[i]);
        ++i;
    }
    fclose(f);
}

static int part1(void)
{
    int segs = 0, w;
    for (int i = 0; i < N; ++i)
        if ((w = range[i] - abs(sensor[i].y - Y)) >= 0)
            seg[segs++] = (Vec){sensor[i].x - w, sensor[i].x + w};

    int cannot = 0;
    segs = join(seg, segs);  // also sorted
    for (int i = 0; i < segs; ++i)
        cannot += seg[i].y - seg[i].x + 1;  // disjoint segment width

    int i = 0, j = 0;
    beacons = dedup_vec(beacon, N);  // also sorted
    // Go to first beacon that is on line y=Y
    while (i < beacons && beacon[i].y != Y)
        ++i;
    // Look at all beacons on line y=Y (it's just 1 beacon in both the example and my input...)
    while (i < beacons && beacon[i].y == Y) {
        while (j < segs && seg[j].y < beacon[i].x)  // go to first segment where beacon may be in
            ++j;
        if (j < segs && seg[j].x <= beacon[i].x)  // check if beacon is in segment
            --cannot;
        ++i;
    }
    return cannot;
}

//           S                            S'
//                        |       %
//                        |     %   .   .   .
//   |                    |       %   #   #   r
// - + - - % % - -        |     %   .   #   #
//   |   % % .       =>   + - - - % - # - S - #
//   | % % . # .          |     %   .   #   #
//   % % . # # # .        |       %   l   #   #
//   % . l # S # r        |     %   .   .   .
//   |   . # # #          |       %
//   |     . #
//
// NB: y increases downwards!
// So, the ranges of sensors s1 and s2 are 1 apart if:
//   s1.l'.x - 1 == s2.r'.x + 1
//   s1.r'.x + 1 == s2.l'.x - 1
//   s1.l'.y + 1 == s2.r'.y - 1
//   s1.r'.y - 1 == s2.l'.y + 1
static int64_t part2(void)
{
    for (int i = 0; i < N; ++i)
        square[i] = (Square){
            rotleft((Vec){sensor[i].x - range[i] - 1, sensor[i].y}),   //  left point of sensor range - 1
            rotleft((Vec){sensor[i].x + range[i] + 1, sensor[i].y})};  // right point of sensor range + 1

    int nx = 0, ny = 0;
    int x[N], y[N];
    for (int i = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j) {
            // Checking overlap not necessary if all solutions checked against sensors anyway
            if (square[i].l.x == square[j].r.x)// && overlap((Vec){square[i].r.y, square[i].l.y}, (Vec){square[j].r.y, square[j].l.y}))
                x[nx++] = square[i].l.x;
            else if (square[i].r.x == square[j].l.x)// && overlap((Vec){square[i].r.y, square[i].l.y}, (Vec){square[j].r.y, square[j].l.y}))
                x[nx++] = square[i].r.x;
            if (square[i].l.y == square[j].r.y)// && overlap((Vec){square[i].l.x, square[i].r.x}, (Vec){square[j].l.x, square[j].r.x}))
                y[ny++] = square[i].l.y;
            else if (square[i].r.y == square[j].l.y)// && overlap((Vec){square[i].l.x, square[i].r.x}, (Vec){square[j].l.x, square[j].r.x}))
                y[ny++] = square[i].r.y;
        }
    nx = dedup_int(x, nx);
    ny = dedup_int(y, ny);

    for (int i = 0; i < nx; ++i)
        for (int j = 0; j < ny; ++j) {
            Vec v = rotright((Vec){x[i], y[j]});
            if (v.x >= 0 && v.x <= M && v.y >= 0 && v.y <= M) {
                // Example has 4 solutions! (more if not checking overlap)
                // (12, 1) -> 48000001
                // ( 8, 5) -> 32000005
                // (18, 7) -> 72000007
                // (14,11) -> 56000011
                // Inevitable to check against all sensors
                // But input has only one solution, so this won't take much time
                bool distress = true;
                for (int k = 0; k < N; ++k)
                    if (manh(sensor[k], v) <= range[k]) {
                        distress = false;
                        break;
                    }
                if (distress)
                    return TUNING * v.x + v.y;
            }
        }
    return 0;
}

int main(void)
{
    starttimer();
    read(NAME);
    printf("Part 1: %d\n", part1());         // example=26, input=5838453
    printf("Part 2: %"PRId64"\n", part2());  // example=56000011, input=12413999391794
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
