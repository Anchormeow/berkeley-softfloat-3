#include <stdio.h>
#include <string.h>
#include "softfloat.h"

int main(void) {
    float a = -3.99003;
    float32_t ai;
    memcpy(&ai, &a, sizeof(a));
    printf("ai is: %x\n", ai.v);


    float b = -4.00588;
    float32_t bi;
    memcpy(&bi, &b, sizeof(b));
    printf("bi is: %x\n", bi.v);

    float32_t cf = f32_mul(ai, bi);
    float d = a * b;

    float c;
    memcpy(&c, &cf, sizeof(cf));
    printf("c is: %f\n", c);
    printf("d is: %f\n", d);
    
    unsigned int a_u = 0xc07f5ca7;
    unsigned int b_u = 0xc080302b;
    float32_t c_u = f32_mul({a_u}, {b_u});
    printf("c_u is: %x\n", c_u.v);
    float e;
    memcpy(&e, &c_u, sizeof(cf));
    printf("e is: %f\n", e);
    return 0;
}