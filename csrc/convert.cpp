#include <stdio.h>
#include <string.h>
#include "softfloat.h"
#include "ncnn_conv.h"

int main(void) {
    // float num1, num2;

    // 32 to 16
    // 输入两个浮点数
    // printf("Enter the first floating-point number: ");
    // scanf("%f", &num1);
    // printf("Enter the second floating-point number: ");
    // scanf("%f", &num2);

    // // 打印第一个浮点数的十六进制表示
    // uint32_t *pNum1 = (uint32_t*)&num1;
    // uint32_t *pNum2 = (uint32_t*)&num2;

    // unsigned short n1_f16 = float32_to_float16(num1);
    // unsigned short n2_f16 = float32_to_float16(num2);

    // printf("Memory representation of %.6f in hexadecimal (float): 0x%08X, fp16 is %04x\n", num1, *pNum1, n1_f16);
    // printf("Memory representation of %.6f in hexadecimal (float): 0x%08X, fp16 is %04x\n", num2, *pNum2, n2_f16);

    // 16 to 32
    // two fp16
    unsigned short num1, num2;
    printf("Enter the first fp16: ");
    scanf("%x", &num1);
    printf("Enter the second fp16: ");
    scanf("%x", &num2);

    unsigned short n1_f16 = num1;
    unsigned short n2_f16 = num2;

    float n1_f32 = float16_to_float32(n1_f16);
    float n2_f32 = float16_to_float32(n2_f16);

    printf("Memory representation of %04x in hexadecimal (fp16): %04x, float is %.6f\n", num1, n1_f16, n1_f32);
    printf("Memory representation of %04x in hexadecimal (fp16): %04x, float is %.6f\n", num2, n2_f16, n2_f32);

    return 0;
}