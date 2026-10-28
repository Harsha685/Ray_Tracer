#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <vector>
#include <thread>
#include <atomic>

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
 *  - The generator (Mersenne Twister) and the distribution are thread_local: each
 *    thread gets its own copy, created on that thread's first call and reused after.
 *  - A single shared generator would be a data race, because every call updates the
 *    generator's internal state and several render threads call this at once.
 *  - Creating a new generator on every call would be slow and would keep returning
 *    the same first value.
 *  - Seeds come from next_seed, a counter shared by all threads that hands out
 *    0, 1, 2, ... It is atomic so two threads starting at once never get the same seed.
 *  - The main thread always calls this first (to build the scene), so it always gets
 *    seed 0 and the scene is identical on every run.
 *  - Render threads get the following seeds, so each produces a different sequence.
 *    Which thread gets which seed and which rows depends on timing, so the noise in
 *    the image still differs slightly between runs.
 */
inline double random_double() {
    static std::atomic<unsigned> next_seed{0};   // Shared by all threads; hands out 0, 1, 2, ...
    thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);
    thread_local std::mt19937 generator(next_seed++);
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

// Common Headers (included last, because they rely on the utilities above)
#include "interval.h"
#include "color.h"
#include "ray.h"
#include "vec3.h"

#endif // COMMON_H
