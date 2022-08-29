#include "mathbase.h"
#include <iostream>
#include <cstdlib>

using namespace std;



int main() {
    mat3 m;
    for(int i = 0; i < 9; i++) {
        m.e[i] = rand();
    }

    auto mm = m * m.inv();
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            cout << mm.e[i * 3 + j]  << " ";
        }
        cout << endl;
    }
}
