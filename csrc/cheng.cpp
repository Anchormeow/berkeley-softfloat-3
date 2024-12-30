#include <stdio.h>
#include <string.h>
#include "softfloat.h"

void mul_float(float a, float b) {

    float mul_f32;
    printf("mul_float FP32 Expression: ");
    printf("%.8f x %.8f = ", a, b );
    mul_f32 = a * b;
    printf("%.8f\n", mul_f32);


    float16_t mul_f16;

    printf("mul_float FP16 Expression: ");
    float32_t a_f32, b_f32;
    memcpy(&a_f32, &a, sizeof(float)); // 将输入转换为float32_t
    memcpy(&b_f32, &b, sizeof(float)); // 将输入转换为float32_t
    float16_t a_f16 = f32_to_f16(a_f32);   // 转换为float16_t
    float16_t b_f16 = f32_to_f16(b_f32);   // 转换为float16_t
    mul_f16 = f16_mul(a_f16, b_f16);
    // 打印算式
    printf("%.8f (cast to fp16: %x) x %.8f (cast to fp16: %x) =", a, a_f16.v, b, b_f16.v);

    // 转换结果为float32并打印
    float result;
    float32_t result_f32 = f16_to_f32(mul_f16);
    printf(" %x in fp16", mul_f16.v);
    memcpy(&result, &result_f32, sizeof(float));
    printf(" = %.8f in fp32\n", result);
}

void mul_hex(int a, int b) {
    // FP16 sum init
    float16_t mul_f16;

    printf("mul_hex FP16 Expression: ");

    float16_t a_f16, b_f16;
    a_f16.v = a;
    b_f16.v = b;
    mul_f16 = f16_mul(a_f16, b_f16);
    // 打印算式
    printf("(fp16) %x x (fp16) %x =", a_f16.v, b_f16.v);

    // 转换结果为float32并打印
    float result;
    float32_t result_f32 = f16_to_f32(mul_f16);
    printf(" %x in fp16", mul_f16.v);
    memcpy(&result, &result_f32, sizeof(float));
    printf(" = %.8f in fp32\n", result);
}

int main(void) {
    // float a = 0.1;
    // float32_t ai;
    // memcpy(&ai, &a, sizeof(a));
    // printf("ai is: %x\n", ai.v);
    // float16_t af = f32_to_f16(ai);
    // printf("af is: %u %x\n", af.v, af.v);

    // float b = 0.2;
    // float32_t bi;
    // memcpy(&bi, &b, sizeof(b));
    // float16_t bf = f32_to_f16(bi);
    // printf("bf is: %u %x\n", bf.v, bf.v);

    // float16_t cf = f16_mul(af, bf);
    // float16_t df = f16_add(af, bf);

    // float c, d;
    // float32_t ci = f16_to_f32(cf);
    // memcpy(&c, &ci, sizeof(c));
    // printf("c is: %.8f\n",c);
    // float32_t di = f16_to_f32(df);
    // memcpy(&d, &di, sizeof(d));
    // printf("d is: %.8f\n",d);


    // unsigned short ei = 0x2e66;
    // float16_t ef;
    // // ef.v = ei;
    // ef = {ei};
    // unsigned short fi = 0x3266;
    // unsigned short gi = 0x34cc;
    // float16_t hf = f16_add(ef, {fi});
    // float16_t jf = f16_add(ef, {gi});
    // printf("hf is: %u %x\n", hf.v, hf.v);
    // printf("jf is: %u %x %hu\n", jf.v, jf.v, jf.v);
    // expB  = expF16UI( hf );
    // sigB  = fracF16UI( jf );
    // uint_fast16_t sig = 0x0FFFF;

    // uint_fast16_t sig = 0x05472;
    // printf("sig is: %x\n", sig);
    // uint_fast8_t roundBits = 0x08;
    // sig &= ~(uint_fast16_t) (! (roundBits ^ 8));
    // printf("sig is: %x\n", sig);
    // // int_fast16_t a = 0x0001;
    // int_fast16_t a = 0x8001;
    // printf("a is: %x\n", a);


    float a1 = 0.1f;
    float b1 = 0.2f;
    mul_float(a1, b1);

    int a2 = 0x2e66;
    int b2 = 0x3266;

    mul_hex(a2, b2);
    return 0;
}