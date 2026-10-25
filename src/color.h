#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"
#include <iostream>

using color = vector3;

inline void write_color(std::ostream& op, const color& c){
    auto red = c.x();
    auto green = c.y();
    auto blue = c.z();

    static const interval intensity(0.000,0.999); 
    int rbyte = int(256 * intensity.clamp(red));
    int gbyte = int(256 * intensity.clamp(green));
    int bbyte = int(256 * intensity.clamp(blue));

    op << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
#endif // COLOR_H