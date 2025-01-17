#include<random>
#include <stdio.h>


static int rn() {
    std::random_device rd; // 使用随机设备作为种子
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 作为随机数生成器
    std::uniform_int_distribution<int> distribution(0, 1); // 生成 0 或 1

    int random_bit = distribution(gen); // 生成随机的 0 或 1

    int result = (random_bit == 0) ? -1 : 1; // 如果随机数为 0，则结果为 -1；如果随机数为 1，则结果为 1
    
    // std::cout << "Random result: " << result << std::endl;

    return result;
}

int main (){
    unsigned short a = 0x4567;
    unsigned short b = a + rn();
    printf("b = %x", b);
    return 0;
}