#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

using color = vector3;

inline void write_color(std::ostream& op, const color& c){
    auto red = c.x();
    auto green = c.y();
    auto blue = c.z();

    int rbyte = int(255.999 * red);
    int gbyte = int(255.999 * green);
    int bbyte = int(255.999 * blue);

    op << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
#endif // COLOR_H