#include <stdio.h>
#include <string.h>
#include "softfloat.h"

int main(void) {
    float a = -0.23566;
    float32_t ai;
    memcpy(&ai, &a, sizeof(a));
    printf("ai is: %x\n", ai.v);


    float b = -10.00588;
    float32_t bi;
    memcpy(&bi, &b, sizeof(b));
    printf("bi is: %x\n", bi.v);

    float32_t cf = f32_add(ai, bi);
    float d = a + b;

    float c;
    memcpy(&c, &cf, sizeof(cf));
    printf("c is: %f\n", c);
    printf("d is: %f\n", d);
    
    return 0;
}