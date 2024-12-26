#include <stdio.h>
#include <string.h>
#include "softfloat.h"

void calculate_sum(float input[], int size) {
    // FP16 sum init
    float16_t sum_f16 = {0};

    printf("FP16 Expression: ");
    for (int i = 0; i < size; ++i) {
        float32_t num_f32;
        memcpy(&num_f32, &input[i], sizeof(float)); // 将输入转换为float32_t
        float16_t num_f16 = f32_to_f16(num_f32);   // 转换为float16_t
        sum_f16 = f16_add(sum_f16, num_f16);       // 累加

        // 打印算式
        if (i > 0) {
            printf(" + ");
        }
        printf("%.8f (%x)", input[i], num_f16.v);

    }

    // 转换累加结果为float32并打印
    float result;
    float32_t result_f32 = f16_to_f32(sum_f16);
    printf(" = %x in fp16", sum_f16.v);
    memcpy(&result, &result_f32, sizeof(float));
    printf(" = %.8f in fp32\n", result);

    // FP32 sum init
    float sum_f32 = 0.0f;

    printf("FP32 Expression: ");
    for (int i = 0; i < size; ++i) {
        sum_f32 = sum_f32 + input[i];       // 累加

        // 打印算式
        if (i > 0) {
            printf(" + ");
        }
        printf("%.8f", input[i]);
    }
    printf(" = %.8f\n", sum_f32);
}

int main(void) {
    float input[] = {0.1f, 0.2f, 0.3f, 0.4f};
    int size = sizeof(input) / sizeof(input[0]);

    calculate_sum(input, size);

    return 0;
}