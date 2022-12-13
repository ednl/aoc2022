#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>   // isdigit

typedef struct _Ell {
    struct _Ell *next;  // next element on same level, next=NULL: last element
    struct _Ell *list;  // list=NULL: this is a value or empty list, list!=NULL: this is a list
    int value;  // -1: empty list, >=0: value
} Ell;

static Ell* new(const Ell *first, Ell *const prev)
{
    Ell *e = calloc(1, sizeof(Ell));
    if (!first)
        first = e;       // this is the first element
    if (prev)            // there is another
        prev->next = e;  // continue linked list
    return e;
}

static Ell* parse(const char *const line)
{
    Ell *first = NULL, *cur = NULL, *prev = NULL;
    const char *c = line;
    while (1) {
        switch (*c) {
        case '[':                             // new element is a list
            cur = new(first, prev);           // create element, set first and prev->next
            if (!(cur->list = parse(c + 1)))  // parse sub list, can be empty
                cur->value = -1;              // element is empty list
            break;
        case ']': return first;               // return link to first element, can be NULL
        case ',': prev = cur; break;          // next element
        default:
            if (isdigit(*c)) {                // new element is a number
                cur = new(first, prev);       // create element, set first and prev->next
                int x = *c - '0';             // convert to value
                while (isdigit(*(c + 1)))     // next char is a number
                    x = x * 10 + *++c - '0';  // step to next char, add to value
                cur->value = x;
            } else
                return first;                 // error, but save the allocated memory
            break;
        }
        ++c;                                  // next char on line
    }
}

static void show(const Ell *const e)
{
    for (int i = 0; i < p->len; ++i)
        printf("%3d", p->sig[i].level);
    printf("\n");
    for (int i = 0; i < p->len; ++i)
        printf("%3d", p->sig[i].value);
    printf("\n\n");
}

int main(void)
{
    int i = 0;
    ssize_t len;
    size_t bufsz;
    char *line;
    FILE *f = fopen("example13.txt", "r");
    while ((len = getline(&line, &bufsz, f)) > 0) {
        if (len == 1) {
            i = 0;
        } else {
            pair[i++] = parse(buf);
            show(pair + i);
            ++i;
        }
    }
    fclose(f);
    free(buf);
    return 0;
}
