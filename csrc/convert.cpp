#include <stdio.h>
#include <string.h>
#include "softfloat.h"
#include "ncnn_conv.h"

int main(void) {
    float num1, num2;

    // 输入两个浮点数
    printf("Enter the first floating-point number: ");
    scanf("%f", &num1);
    printf("Enter the second floating-point number: ");
    scanf("%f", &num2);

    // 打印第一个浮点数的十六进制表示
    uint32_t *pNum1 = (uint32_t*)&num1;
    uint32_t *pNum2 = (uint32_t*)&num2;

    unsigned short n1_f16 = float32_to_float16(num1);
    unsigned short n2_f16 = float32_to_float16(num2);

    printf("Memory representation of %.6f in hexadecimal (float): 0x%08X, fp16 is %04x\n", num1, *pNum1, n1_f16);
    printf("Memory representation of %.6f in hexadecimal (float): 0x%08X, fp16 is %04x\n", num2, *pNum2, n2_f16);

    return 0;
}