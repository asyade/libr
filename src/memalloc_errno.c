#include "libr.h"

int errcode(int val, int set)
{
    static int err = 0;

    if (set)
        err = val;
    return (err);
}

int memalloc_geterr()
{
    return (errcode(0, 0));
}

int memalloc_seterr(int code)
{
    return (errcode(code, 1));
}

void memalloc_panic(int code)
{
    printf("Memory arena corupted exit, (error: %d)", code);
    exit(1);
}