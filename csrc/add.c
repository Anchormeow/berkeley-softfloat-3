#include <stdio.h>
#include <string.h>
#include "softfloat.h"

int main(void) {
    float a = 0.1;
    float32_t ai;
    memcpy(&ai, &a, sizeof(a));
    printf("ai is: %x\n", ai.v);
    float16_t af = f32_to_f16(ai);
    printf("af is: %u %x\n", af.v, af.v);

    float b = 0.2;
    float32_t bi;
    memcpy(&bi, &b, sizeof(b));
    float16_t bf = f32_to_f16(bi);
    printf("bf is: %u %x\n", bf.v, bf.v);

    float16_t cf = f16_mul(af, bf);
    float16_t df = f16_add(af, bf);

    float c, d;
    float32_t ci = f16_to_f32(cf);
    memcpy(&c, &ci, sizeof(c));
    printf("c is: %.8f\n",c);
    float32_t di = f16_to_f32(df);
    memcpy(&d, &di, sizeof(d));
    printf("d is: %.8f\n",d);
    
    return 0;
}