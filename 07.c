/**
 * Advent of Code 2022
 * Day 7: No Space Left On Device
 * https://adventofcode.com/2022/day/7
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX1 (100000ul)                 // part 1: max dir size to include in sum
#define MAX2 (70000000ul - 30000000ul)  // part 2: max used space = disk size - required free space

typedef struct _FSNode {
    size_t size, nodes;          // size=0 for dirs, nodes=0 for files
    struct _FSNode *up, **node;  // link to parent node, array of pointers to dir entries
} FSNode;

static size_t used;          // total used space
static size_t part1, part2;  // global for access in recursive function

static FSNode* append(FSNode * const parent, const size_t size)
{
    FSNode *child = calloc(1, sizeof(FSNode));
    if (child != NULL) {
        child->size = size;
        if (parent != NULL) {
            FSNode **p = realloc(parent->node, (parent->nodes + 1ul) * sizeof(FSNode**));
            if (p == NULL) {
                free(child);
                return NULL;
            }
            parent->node = p;
            parent->node[parent->nodes++] = child;
            child->up = parent;
        }
    }
    return child;
}

static void delete(FSNode * node)
{
    if (node == NULL)
        return;
    for (size_t i = 0; i < node->nodes; ++i)
        delete(node->node[i]);
    free(node->node);
    free(node);
}

static size_t calc(FSNode * node)
{
    if (node == NULL)
        return 0;
    if (node->size != 0)
        return node->size;  // file size
    size_t sum = 0;  // dir size
    for (size_t i = 0; i < node->nodes; ++i)
        sum += calc(node->node[i]);
    if (used == 0 && sum <= MAX1)
        part1 += sum;  // sum all dir sizes of at most MAX1
    if (used != 0 && sum >= used - MAX2 && (sum < part2 || part2 == 0))
        part2 = sum;  // find minimum dir size of at least required size
    return sum;
}

int main(void)
{
    FILE *f = fopen("aocinput/2022-07-input.txt", "r");
    if (!f)
        return 1;

    char *buf = NULL;
    size_t bufsz;
    ssize_t len;
    FSNode *root = NULL, *cur = NULL;

    while ((len = getline(&buf, &bufsz, f)) > 1)
        switch (buf[0]) {
        case '$':  // command
            if (buf[2] == 'c')  // no action for command "ls", only for "cd"
                switch (buf[5]) {
                    case '/': cur = root = append(NULL, 0); break;  // $ cd /
                    case '.': cur = cur->up;                break;  // $ cd ..
                    default : cur = append(cur, 0);         break;  // $ cd dirname
                }
        case 'd': break;  // no action for listing "dir dirname"
        default : append(cur, strtoul(buf, NULL, 10)); break;  // listing "123 filename"
        }
    fclose(f);
    free(buf);

    used = calc(root);
    printf("Part 1: %zu\n", part1);  // 1845346
    calc(root);
    printf("Part 2: %zu\n", part2);  // 3636703
    delete(root);
    return 0;
}
