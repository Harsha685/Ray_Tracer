#ifndef COMMON_H
#define COMMON_H

// Shared constants, aliases and random-number helpers, plus the headers nearly every
// file needs (see "Common Headers" at the bottom).
//
//  - Every .cpp file must include this header first.
//  - vec3.h and interval.h use the helpers below but must not include this header:
//    this header includes them, and the cycle would break the build.

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

/*
 * Converts an angle from degrees to radians.
 *
 *  - std::sin, std::cos and std::tan all expect radians.
 *  - A full circle is 360 degrees or 2 * pi radians, so the factor is pi / 180.
 */
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

/*
 * Returns a uniformly distributed random number in [0, 1).
 *
 *  - The generator (Mersenne Twister) and the distribution are function-level statics:
 *    they are created once on the first call and reused afterwards.
 *  - Creating a new generator on every call would be slow and would keep returning
 *    the same first value.
 *  - The generator is default-seeded, so every run produces the same sequence and
 *    renders are reproducible.
 */
inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

/*
 * Returns a uniformly distributed random number in [min, max).
 *
 *  - Takes r in [0, 1) from random_double() and stretches/shifts it:
 *    min + (max - min) * r.
 */
inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

/*
 * Returns a uniformly distributed random integer in [min, max] (both ends included).
 *
 *  - Draws a real number in [min, max + 1) and rounds it down, so each integer gets
 *    an equal-width slice of that range.
 *  - std::floor instead of a plain int() cast: int() rounds toward zero, which would
 *    fold the slices for -0.x and 0.x together when min is negative.
 */
inline int random_int(int min, int max) {
    return int(std::floor(random_double(min, max + 1)));
}

// Common Headers
// These stay below the helpers above because vec3.h and interval.h depend on them.
#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif // COMMON_H
