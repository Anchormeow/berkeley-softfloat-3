#include <stdio.h>
#include <string.h>
#include "softfloat.h"
#include "ncnn_conv.h"

void calculate_sum_float(float input[], int size) {
    // FP32 sum init
    float sum_f32 = 0.0f;

    printf("calculate_sum_float FP32 Expression: ");
    for (int i = 0; i < size; ++i) {
        sum_f32 = sum_f32 + input[i];       // 累加

        // 打印算式
        if (i > 0) {
            printf(" + ");
        }
        printf("%.8f", input[i]);
    }
    printf(" = %.8f\n", sum_f32);


    // FP16 sum init
    float16_t sum_f16 = {0};

    printf("calculate_sum_float FP16 Expression: ");
    for (int i = 0; i < size; ++i) {
        // float32_t num_f32;
        // memcpy(&num_f32, &input[i], sizeof(float)); // 将输入转换为float32_t
        // float16_t num_f16 = f32_to_f16(num_f32);   // 转换为float16_t
        float16_t num_f16;
        num_f16.v = float32_to_float16(input[i]);
        sum_f16 = f16_add(sum_f16, num_f16);       // 累加

        // 打印算式
        if (i > 0) {
            printf(" + ");
        }
        printf("%.8f (cast to fp16: %x)", input[i], num_f16.v);

    }

    // 转换累加结果为float32并打印
    float result;
    // float32_t result_f32 = f16_to_f32(sum_f16);
    printf(" = %x in fp16", sum_f16.v);
    // memcpy(&result, &result_f32, sizeof(float));
    result = float16_to_float32(sum_f16.v);
    printf(" = %.8f in fp32\n", result);
}

void calculate_sum_hex(int input[], int size) {
    // FP16 sum init
    float16_t sum_f16 = {0};

    printf("calculate_sum_hex FP16 Expression: ");
    for (int i = 0; i < size; ++i) {
        float16_t num_f16;
        num_f16.v = input[i];
        sum_f16 = f16_add(sum_f16, num_f16);       // 累加

        // 打印算式
        if (i > 0) {
            printf(" + ");
        }
        printf("(fp16) %x ", num_f16.v);

    }

    // 转换累加结果为float32并打印
    float result;
    // float32_t result_f32 = f16_to_f32(sum_f16);
    printf(" = %x in fp16", sum_f16.v);
    // memcpy(&result, &result_f32, sizeof(float));
    result = float16_to_float32(sum_f16.v);
    printf(" = %.8f in fp32\n", result);
}

int main(void) {
    float input1[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    int size1 = sizeof(input1) / sizeof(input1[0]);

    calculate_sum_float(input1, size1);

    int input2[] = {0x2e66, 0x3266, 0x34cd, 0x3666, 0x0400};
    // int input2[] = {0x0000, 0x0400};
    int size2 = sizeof(input2) / sizeof(input2[0]);
    calculate_sum_hex(input2, size2);

    // float a = 1;
    // float32_t ai;
    // memcpy(&ai, &a, sizeof(a));
    // printf("ai is: %x\n", ai.v);
    // float16_t af = f32_to_f16(ai);
    // printf("af is: %x\n", af.v);
    // float b = -1;
    // float32_t bi;
    // memcpy(&bi, &b, sizeof(b));
    // printf("bi is: %x\n", bi.v);
    // float16_t bf = f32_to_f16(bi);
    // printf("bf is: %x\n", bf.v);

    // float16_t c;
    // c.v = 0x4567;
    // printf("c is: %x\n", c);
    return 0;
}