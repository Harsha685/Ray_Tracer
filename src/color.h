#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"
#include <iostream>

// A color is stored as a vector3 of (red, green, blue), each nominally in [0, 1]
using color = vector3;


inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}
// Writes one pixel's color to the output stream as three integers "R G B" on one line,
// which is the per-pixel format a plain-text PPM (P3) file expects.
//
// Each channel is a double that should be in [0, 1]. Values outside that range (for
// example from rounding or very bright light) are clamped to [0, 0.999], then scaled by
// 256 and truncated to an int, giving a byte in [0, 255]. Clamping to 0.999 instead of
// 1.0 guarantees 256 * x never rounds up to 256, which is not a valid byte value.
inline void write_color(std::ostream& op, const color& c){
    auto red = c.x();
    auto green = c.y();
    auto blue = c.z();

    red = linear_to_gamma(red);
    green = linear_to_gamma(green);
    blue = linear_to_gamma(blue);
    // Clamp to just under 1 so 256 * x never reaches 256 (the max byte is 255)
    static const interval intensity(0.000,0.999);
    // Scale [0, 1) to the integer byte range [0, 255]
    int rbyte = int(256 * intensity.clamp(red));
    int gbyte = int(256 * intensity.clamp(green));
    int bbyte = int(256 * intensity.clamp(blue));

    op << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif // COLOR_H
