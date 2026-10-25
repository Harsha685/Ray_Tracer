#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

// C++ Std Usings
using std::make_shared;
using std::shared_ptr;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

// Converts an angle from degrees to radians (the unit std::sin, std::cos, etc. expect).
// A full circle is 360 degrees or 2*pi radians, so we multiply by pi / 180.
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// Returns a uniformly distributed random number in [0, 1).
//
// The generator (Mersenne Twister) and distribution are function-level statics, so they
// are created once on the first call and reused afterwards; creating a new generator on
// every call would be slow and would keep returning the same first value. The generator
// is default-seeded, so every run of the program produces the same sequence, which makes
// renders reproducible.
inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

// Returns a uniformly distributed random number in [min, max).
// Takes a value r in [0, 1) and stretches/shifts it: min + (max - min) * r.
inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}

// Common Headers (included last, because they rely on the utilities above)
#include "interval.h"
#include "color.h"
#include "ray.h"
#include "vec3.h"

#endif // COMMON_H
