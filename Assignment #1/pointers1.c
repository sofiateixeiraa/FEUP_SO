#include <stdio.h>
int main() {
    int i, j, *p, *q;
    i = 5;
    p = &i;
    *p = 7;
    j = 3;
    p = &j;
    q = p;
    p = &i;
    *q = 2;
    printf("i=%d, j=%d, p=%p, q=%p\n", i, j, p, q);
    return 0;
}

//Result: i=7, j=2, p=0x7ffdcc79b0f0, q=0x7ffdcc79b0f4