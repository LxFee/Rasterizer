#include "mathbase.h"
#include <iostream>
#include <cstdlib>

using namespace std;



int main() {
    float data[9];
    for(int i = 0; i < 9; i++) {
        data[i] = rand();
    }
    mat3 m(data);

    m = m * m.inv();
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            cout << m.data()[i * 3 + j]  << " ";
        }
        cout << endl;
    }
}
