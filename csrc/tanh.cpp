#include <stdio.h>
#include <vector>
#include <stdexcept>
#include <cmath>
#include "ncnn_conv.h"
#include "softfloat.h"
#include <string.h>

// 函数：根据输入 x 和分段表，计算 ax^2 + bx + c
#ifndef MY_F32
float calculatePiecewise(float x) {
    struct Segment {
        float lower_bound; // 区间下限
        float upper_bound; // 区间上限
        float a;           // 参数 a
        float b;           // 参数 b
        float c;           // 参数 c
    };
    int flag = 0;
    if(x<0.0f) {
        x = -x;
        flag = 1;
    }
    // 定义分段表，包含所有区间的 [lower_bound, upper_bound) 和对应的 a, b, c
    std::vector<Segment> table = {
        {0, 0.0025, -0.0012496391032164361, 1.0000012476639613, -2.581424790887899e-10},
        {0.0025, 0.005, -0.0037498654906415996, 1.0000137478899722, -1.6400164257708666e-08},
        {0.005, 0.0075, -0.006249691201402562, 1.0000387459555784, -7.941258521448579e-08},
        {0.0075, 0.01, -0.008749053698263025, 1.0000762361920632, -2.205177389054639e-07},
        {0.01, 0.015, -0.012497329168622912, 1.0001549452869423, -6.351094584263195e-07},
        {0.015, 0.025, -0.019989258462053693, 1.000394665090151, -2.5638378768480118e-06},
        {0.025, 0.04, -0.03245321539972505, 1.0010427051300212, -1.1047089627670822e-05},
        {0.04, 0.05, -0.044878049114368844, 1.0020117756042783, -3.0002270980464313e-05},
        {0.05, 0.065, -0.05724560576362616, 1.0032731224410454, -6.222235982455108e-05},
        {0.065, 0.08, -0.07199184795917417, 1.0051898728817892, -0.0001246181823511101},
        {0.08, 0.1, -0.08902961503139593, 1.007949434453407, -0.00023652452234136217},
        {0.1, 0.125, -0.11061207367157079, 1.0123076792484207, -0.0004568989216648727},
        {0.125, 0.18, -0.1478012772003518, 1.0220408561073864, -0.0010952297229548402},
        {0.18, 0.22, -0.1896451855748255, 1.036832893802592, -0.0024054527585000525},
        {0.22, 0.3, -0.23765418008706143, 1.058671823770094, -0.004893750621296228},
        {0.3, 0.4, -0.2979602280268921, 1.0951296525532936, -0.010419795182050823},
        {0.4, 0.5, -0.3464371665701938, 1.133602356174123, -0.01806862022275095},
        {0.5, 0.7, -0.380910708747892, 1.1684758991330813, -0.02690936340330646},
        {0.7, 0.9, -0.37051165849559603, 1.1522335031384323, -0.020623295351563223},
        {0.9, 1.1, -0.3196101635611692, 1.0598167838960573, 0.021387294367081727},
        {1.1, 1.3, -0.2543420721827252, 0.9161044376545794, 0.10058192634441641},
        {1.3, 1.5, -0.19156318652164936, 0.7531156598315074, 0.20645376278971372},
        {1.5, 1.75, -0.13334241116843767, 0.5778048282020446, 0.33852121439049765},
        {1.75, 1.9, -0.09386635606709746, 0.4415661444584914, 0.4561100721883112},
        {1.9, 2.2, -0.062378973909782115, 0.32042559153470757, 0.5726709075006972},
        {2.2, 2.6, -0.03218598179811281, 0.18737371542119902, 0.719370525799581},
        {2.6, 3.2, -0.012264846125775383, 0.08360568744890952, 0.8546567507768315},
        {3.2, 4, -0.0031152222252557762, 0.025606583572377804, 0.9466997474412018},
        {4, 5, -0.0005297208458861204, 0.0053123206048557225, 0.9865755353966844},
        {5, 7, -3.246137946784751e-05, 0.0004255082107099103, 0.9986040925290274},
        {7, 9, -5.945338468370988e-07, 1.0171404203850754e-05, 0.9999564688762415},
    };
    
    for (const auto& segment : table) {
        if (x > segment.lower_bound && x <= segment.upper_bound) { // 查找区间
            return (flag == 1)? -(segment.a * x * x + segment.b * x + segment.c):segment.a * x * x + segment.b * x + segment.c; // 计算结果
        }
    }
    if(x == 0.0f) {
        return 0.0f;
    } else {
        return (flag == 1)?-1.0:1.0; // 如果 x 超出范围，返回默认值 1.0
    }
}
#else
float calculatePiecewise(float x) {
    struct Segment {
        unsigned int lower_bound; // 区间下限
        unsigned int upper_bound; // 区间上限
        unsigned int a;           // 参数 a
        unsigned int b;           // 参数 b
        unsigned int c;           // 参数 c
    };
    int flag = 0;
    if(x<0.0f) {
        x = -x;
        flag = 1;
    }
    float32_t xu;
    memcpy(&xu, &x, sizeof(x));
    // 定义分段表，包含所有区间的 [lower_bound, upper_bound) 和对应的 a, b, c
    std::vector<Segment> table = {
        {0x00000000, 0x3b23d70a, 0xbaa3caee, 0x3f80000a, 0xaf8dea53}, // 1
        {0x3b23d70a, 0x3ba3d70a, 0xbb75c04e, 0x3f800073, 0xb28ce058},
        {0x3ba3d70a, 0x3bf5c28f, 0xbbccca36, 0x3f800145, 0xb3aa8988},
        {0x3bf5c28f, 0x3c23d70a, 0xbc0f5831, 0x3f800280, 0xb46cc774},
        {0x3c23d70a, 0x3c75c28f, 0xbc4cc199, 0x3f800514, 0xb52a7c64},
        {0x3c75c28f, 0x3ccccccd, 0xbca3c083, 0x3f800cef, 0xb62c0e66}, // 6
        {0x3ccccccd, 0x3d23d70a, 0xbd04edaa, 0x3f80222b, 0xb73956e3},
        {0x3d23d70a, 0x3d4ccccc, 0xbd37d20c, 0x3f8041ec, 0xb7fbad63},
        {0x3d4ccccc, 0x3d851eb8, 0xbd6a7a5e, 0x3f806b41, 0xb8827d60},
        {0x3d851eb8, 0x3da3d70a, 0xbd937076, 0x3f80aa10, 0xb902abf0},
        {0x3da3d70a, 0x3dcccccd, 0xbdb65529, 0x3f81047d, 0xb9780391}, // 11
        {0x3dcccccd, 0x3e000000, 0xbde28895, 0x3f81934c, 0xb9ef8bef},
        {0x3e000000, 0x3e3851ec, 0xbe175938, 0x3f82d23c, 0xba8f8dd0},
        {0x3e3851ec, 0x3e6147ae, 0xbe423259, 0x3f84b6f1, 0xbb1da4cd},
        {0x3e6147ae, 0x3e99999a, 0xbe735b9e, 0x3f87828f, 0xbba05bc1},
        {0x3e99999a, 0x3ecccccd, 0xbe988e3e, 0x3f8c2d35, 0xbc2ab7ca}, // 16
        {0x3ecccccd, 0x3f000000, 0xbeb16036, 0x3f9119e2, 0xbc9404a5},
        {0x3f000000, 0x3f333333, 0xbec306ba, 0x3f95909e, 0xbcdc7106},
        {0x3f333333, 0x3f666666, 0xbebdb3b4, 0x3f937c63, 0xbca8f22f},
        {0x3f666666, 0x3f8ccccd, 0xbea3a3f1, 0x3f87a814, 0x3caf3468},
        {0x3f8ccccd, 0x3fa66666, 0xbe823920, 0x3f6a85d2, 0x3dcdfde6}, // 21
        {0x3fa66666, 0x3fc00000, 0xbe442924, 0x3f40cc30, 0x3e53689d},
        {0x3fc00000, 0x3fe00000, 0xbe088aea, 0x3f13eb04, 0x3ead52a7},
        {0x3fe00000, 0x3ff33333, 0xbdc03d01, 0x3ee214f5, 0x3ee98742},
        {0x3ff33333, 0x400ccccd, 0xbd7f8118, 0x3ea40ed3, 0x3f129a90},
        {0x400ccccd, 0x40266666, 0xbd03d573, 0x3e3fdee5, 0x3f3828ab}, // 26
        {0x40266666, 0x404ccccd, 0xbc48f27e, 0x3dab3975, 0x3f5acac9},
        {0x404ccccd, 0x40800000, 0xbb4c28c2, 0x3cd1c4e6, 0x3f725aea},
        {0x40800000, 0x40a00000, 0xba0adcf7, 0x3bae12fa, 0x3f7c9037},
        {0x40a00000, 0x40d80000, 0xb82405dd, 0x3a0390df, 0x3f7f963a}, // 5-6.75
        {0x40d80000, 0x41080000, 0xb59e81d0, 0x37a1d096, 0x3f7ffad5}, // 31
    };
    
    for (const auto& segment : table) {
        if (xu.v >= segment.lower_bound && xu.v < segment.upper_bound) { // 查找区间
            float32_t ax2 = f32_mul(f32_mul({segment.a}, xu), xu);
            float32_t bx = f32_mul({segment.b}, xu);
            float32_t result = f32_add(f32_add(ax2, bx), {segment.c});
            float t;
            memcpy(&t, &result, sizeof(result));
            return (flag == 1)? -t : t; // 计算结果
        }
    }
    if(x == 0.0f) {
        return 0.0f;
    } else {
        return (flag == 1)?-1.0f:1.0f; // 如果 x 超出范围，返回默认值 1.0
    }
}
#endif


int main() {
    float x;
    int err = 0;
    int total = 0;
    // for(x=-10.0f;x<10.0f;x=x+1.0f) {
    for(x=-9.0f;x<9.0f;x=x+0.002f) {
        float result = calculatePiecewise(x);
        float result_e = tanhf(x);
    
        unsigned short result_f16 = float32_to_float16(result);
        unsigned short result_e_f16 = float32_to_float16(result_e);
        int diff = result_f16 - result_e_f16;
        printf("%.8f %.8f %x %x %d\n", result, result_e, result_f16, result_e_f16, diff);
        if (diff != 0) {
            err++;
        }
        total++;
    }
    printf("err/total: %d/%d\n", err, total);
    return 0;
}