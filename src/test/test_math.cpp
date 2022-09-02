/**
 * @file test_math.cpp
 * @author limil (limil.top@qq.com)
 * @brief 测试三维矩阵求逆是否正确
 * @date 2022-09-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "mathbase.h"
#include <iostream>
#include <cstdlib>
#include <random>
#include <cmath>

using namespace std;

const float eps = 1e-6;

inline void check1(float x, bool& flag) {
    flag &= (fabs(x - 1.0f) < eps);
}

inline void check0(float x, bool& flag) {
    flag &= (fabs(x) < eps);
}

int main() {
    default_random_engine e;
    uniform_real_distribution<float> rr(0.0f, 1.0f);
    while(1) {
        float data[9];
        for(int i = 0; i < 9; i++) {
            data[i] = rr(e);
        }

        mat3 m(data);
        if(fabs(m.det()) < eps) continue;

        m = m * m.inv();

        bool passed = true;
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                if(i == j) check1(m.data()[i * 3 + j], passed);
                else check0(m.data()[i * 3 + j], passed);
                cout << m.data()[i * 3 + j]  << " ";
            }
            cout << endl;
        }
        if(passed) {
            cout << "Passed!" << endl;
        } else {
            cout << "Error!" << endl;
        }
        break;
    }
}
