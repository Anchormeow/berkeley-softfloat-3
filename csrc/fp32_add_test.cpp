#include <stdio.h>
#include <string.h>
#include "softfloat.h"
#include <math.h>

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
    
    // int input2_float[] = {0x3e000000, 0x3e000001};
    int input2_float[] = {0x3BA3D70A, 0x3BA3D70B};
    float e,f;
    memcpy(&e, &input2_float[0], sizeof(a));
    memcpy(&f, &input2_float[1], sizeof(b));
    float diff = (float)fabs(f - e);
    bool f1 =  e == f;
    printf("diff is: %.8f\n", diff);
    printf("f1 is: %d\n", f1);
    
    return 0;
}