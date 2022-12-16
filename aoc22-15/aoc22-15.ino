#define N (38)       // number of sensors in input file
#define Y (INT32_C(2000000))  // horizontal line of interest in input
#define M (INT32_C(4000000))  // max x or y value for part 2

#define TUNING (INT64_C(4000000))

typedef struct {
    int32_t x, y;
} Vec;

typedef struct {
    Vec l, r;
} Sq;

Vec sensor[N] = {
    {2300471, 2016823},
    {1315114,   37295},
    {1039523, 3061589},
    { 214540, 3768792},
    {1641345, 3524291},
    {1016825, 1450262},
    {2768110, 3703050},
    {2213658, 3522463},
    {3842967, 3381135},
    {3952516, 2683159},
    { 172892,  369117},
    {3999720, 3498306},
    {1596187,  307084},
    {3863253, 3406760},
    {3927553, 3450758},
    {2774120, 3228484},
    {3897140, 3418751},
    {1880329, 2843697},
    {  33790, 3243415},
    { 438583, 2647769},
    {1540347, 3177380},
    {3120086, 3997791},
    {3428967, 3105227},
    {2898335, 1037911},
    {3456260, 3578627},
    {1859971, 3999725},
    {3147730, 3999322},
    {3920847,   71575},
    { 956723, 3999438},
    {1193760, 3758205},
    {3999446, 1929369},
    {1434466, 2254087},
    { 200365, 1856636},
    {1859710,   31159},
    {3712613, 3930105},
    {1660185,    2900},
    {1497065,   93501},
    {3832823, 3346266}};

Vec beacon[N] = {
    {2687171, 2822745},
    {1671413,   43557},
    {1570410, 3710085},
    {-355567, 3900317},
    {1570410, 3710085},
    { 745731, 2000000},
    {3133588, 3984216},
    {1570410, 3710085},
    {3839159, 3421933},
    {3213800, 2708360},
    {-228964, 1438805},
    {3839159, 3421933},
    {1671413,   43557},
    {3839159, 3421933},
    {3839159, 3421933},
    {2687171, 2822745},
    {3839159, 3421933},
    {2687171, 2822745},
    {-355567, 3900317},
    { 745731, 2000000},
    {1570410, 3710085},
    {3133588, 3984216},
    {3213800, 2708360},
    {3213800, 2708360},
    {3839159, 3421933},
    {1570410, 3710085},
    {3133588, 3984216},
    {3826138, -255533},
    {1570410, 3710085},
    {1570410, 3710085},
    {3213800, 2708360},
    { 745731, 2000000},
    { 745731, 2000000},
    {1671413,   43557},
    {3133588, 3984216},
    {1671413,   43557},
    {1671413,   43557},
    {3839159, 3421933}};

int32_t range[N], beacons;
Vec seg[N];  // segments on line y=Y covered by a sensor from x=.x to x=.y
Sq sq[N];

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
static int32_t manh(const Vec a, const Vec b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Sign of x: x > 0 => +1, x == 0 => 0, x < 0 => -1
static int32_t sign(int32_t x)
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

static int32_t cmp_int(const void* p, const void* q)
{
    return sign(*(const int32_t*)p - *(const int32_t*)q);
}

// Deduplicate array of ints
// Returns: number of remaining (unique) ints
static int32_t dedup_int(int32_t* const arr, const int32_t len)
{
    if (arr == NULL || len <= 0)
        return 0;
    qsort(arr, (size_t)len, sizeof(*arr), cmp_int);
    int32_t i = 0, j = 0;
    while (j < len) {
        while (++j < len && arr[i] == arr[j]);  // find first a[j] != a[i]
        if (++i != j && j < len)  // shift a[j] up if skipped (order of conditions is important because of ++)
            arr[i] = arr[j];
    }
    return i;
}

static int32_t cmp_yx(const void* p, const void* q)
{
    const Vec s = signdot(sub(*(const Vec*)p, *(const Vec*)q));
    if (s.y)
        return s.y;  // first sort on vy < wy
    return s.x;      // then sort on vx < wx
}

// Deduplicate array of position vectors
// Returns: number of remaining (unique) vectors
static int32_t dedup_vec(Vec* const arr, const int32_t n)
{
    if (arr == NULL || n <= 0)
        return 0;
    qsort(arr, (size_t)n, sizeof(*arr), cmp_yx);
    int32_t i = 0, j = 0;
    while (j < n) {
        while (++j < n && equal(arr[i], arr[j]));  // find first a[j] != a[i]
        if (++i != j && j < n)  // shift a[j] up if skipped (order of conditions is important because of ++)
            arr[i] = arr[j];
    }
    return i;
}

static int32_t cmp_xy(const void* p, const void* q)
{
    const Vec s = signdot(sub(*(const Vec*)p, *(const Vec*)q));
    if (s.x)
        return s.x;  // first sort on vx < wx
    return -s.y;     // then sort on vy > wy
}

// Join array of segments where overlapping
// Returns: number of remaining (disjoint) segments
static int32_t join(Vec* const arr, const int32_t n)
{
    if (arr == NULL || n <= 0)
        return 0;
    qsort(arr, (size_t)n, sizeof(*arr), cmp_xy);
    int32_t i = 0, j = 0;
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
void read(void)
{
    for (int32_t i = 0; i < N; ++i)
        range[i] = manh(sensor[i], beacon[i]);
}

int32_t part1(void)
{
    int32_t segs = 0, w;
    for (int32_t i = 0; i < N; ++i)
        if ((w = range[i] - abs(sensor[i].y - Y)) >= 0)
            seg[segs++] = (Vec){sensor[i].x - w, sensor[i].x + w};

    int32_t cannot = 0;
    segs = join(seg, segs);  // also sorted
    for (int32_t i = 0; i < segs; ++i)
        cannot += seg[i].y - seg[i].x + 1;  // disjoint segment width

    int32_t i = 0, j = 0;
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
int64_t part2(void)
{
    for (int32_t i = 0; i < N; ++i)
        sq[i] = (Sq){
            rotleft((Vec){sensor[i].x - range[i] - 1, sensor[i].y}),   //  left point of sensor range - 1
            rotleft((Vec){sensor[i].x + range[i] + 1, sensor[i].y})};  // right point of sensor range + 1

    int32_t nx = 0, ny = 0;
    int32_t x[8], y[8];
    for (int32_t i = 0; i < N - 1; ++i)
        for (int32_t j = i + 1; j < N; ++j) {
            if (sq[i].l.x == sq[j].r.x)// && overlap((Vec){square[i].r.y, square[i].l.y}, (Vec){square[j].r.y, square[j].l.y}))
                x[nx++] = sq[i].l.x;
            else if (sq[i].r.x == sq[j].l.x)// && overlap((Vec){square[i].r.y, square[i].l.y}, (Vec){square[j].r.y, square[j].l.y}))
                x[nx++] = sq[i].r.x;
            if (sq[i].l.y == sq[j].r.y)// && overlap((Vec){square[i].l.x, square[i].r.x}, (Vec){square[j].l.x, square[j].r.x}))
                y[ny++] = sq[i].l.y;
            else if (sq[i].r.y == sq[j].l.y)// && overlap((Vec){square[i].l.x, square[i].r.x}, (Vec){square[j].l.x, square[j].r.x}))
                y[ny++] = sq[i].r.y;
        }
    nx = dedup_int(x, nx);
    ny = dedup_int(y, ny);

    for (int32_t i = 0; i < nx; ++i)
        for (int32_t j = 0; j < ny; ++j) {
            Vec v = rotright((Vec){x[i], y[j]});
            if (v.x >= 0 && v.x <= M && v.y >= 0 && v.y <= M) {
                // Example has 4 solutions! (more if not checking overlap)
                // (12, 1) -> 48000001
                // ( 8, 5) -> 32000005
                // (18, 7) -> 72000007
                // (14,11) -> 56000011
                // Inevitable to check against all sensors
                // But input has only one, so this won't take much time
                // bool distress = true;
                // for (int32_t k = 0; k < N; ++k)
                //     if (manh(sensor[k], v) <= range[k]) {
                //         distress = false;
                //         break;
                //     }
                // if (distress)
                    return TUNING * v.x + v.y;
            }
        }
    return 0;
}

size_t print64(Print* pr, uint64_t n) {
  char buf[21];
  char *str = &buf[sizeof(buf) - 1];
  *str = '\0';
  do {
    uint64_t m = n;
    n /= 10;
    *--str = m - 10*n + '0';
  } while (n);
  pr->print(str);
}
size_t print64(Print* pr, int64_t n) {
  size_t s = 0;
  if (n < 0) {
    n = -n;
    s = pr->print('-');
  }  return s + print64(pr, (uint64_t)n);
}
size_t println64(Print* pr, int64_t n) {
  return print64(pr, n) + pr->println();
}
size_t println64(Print* pr, uint64_t n) {
  return print64(pr, n) + pr->println();
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(9600);
    while (!Serial);
    unsigned long t0 = millis();

    read();
    int32_t p1 = part1();
    int64_t p2 = part2();

    unsigned long dt = millis() - t0;
    Serial.print(F("Part 1: "));
    Serial.println(p1);
    Serial.print(F("Part 2: "));
    println64(&Serial, p2);
    Serial.print(F("Time: "));
    Serial.print(dt);
    Serial.println(F(" ms"));

    digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
    delay(1000);
}
