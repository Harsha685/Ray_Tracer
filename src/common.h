#ifndef COMMON_H
#define COMMON_H

// Shared constants, aliases and random-number helpers. Includes no project headers,
// so every other header can include it without creating an include cycle.

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

#endif // COMMON_H
