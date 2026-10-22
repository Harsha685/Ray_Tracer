
#include <iostream>
#include "vec3.h"

int main() {
    vector3 v1(1, 2, 3);
    vector3 v2(4, 5, 6);

    std::cout << "v1: " << v1 << "\n";
    std::cout << "v2: " << v2 << "\n";
    std::cout << "v1 + v2: " << v1 + v2 << "\n";
    std::cout << "v1 - v2: " << v1 - v2 << "\n";
    std::cout << "-v1: " << -v1 << "\n";
    std::cout << "v1 * v2: " << v1 * v2 << "\n";
    std::cout << "v1 * 2: " << v1 * 2 << "\n";
    std::cout << "v1 / 2: " << v1 / 2 << "\n";
    std::cout << "dot(v1, v2): " << dot(v1, v2) << "\n";
    std::cout << "cross(v1, v2): " << cross(v1, v2) << "\n";
    std::cout << "length(v1): " << v1.length() << "\n";
    std::cout << "unit(v1): " << unit_vector(v1) << "\n";

    return 0;
}
