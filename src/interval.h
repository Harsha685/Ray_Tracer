#ifndef INTERVAL_H
#define INTERVAL_H

#include "common.h"

// A range of real numbers [min, max]. Used for the allowed t range of a ray and for clamping.
class interval {
public:
    double min, max;

    /*
     * Creates an empty interval.
     *
     *  - min = +infinity and max = -infinity, so no number can be inside it.
     */
    interval() : min(+infinity), max(-infinity) {}

    /*
     * Creates the interval [min, max].
     */
    interval(double min, double max) : min(min), max(max) {}

    /*
     * Creates the tightest interval enclosing both a and b.
     */
    interval(const interval& a, const interval& b) {
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }

    /*
     * Returns the length of the interval (negative if the interval is empty).
     */
    double size() const {
        return max - min;
    }

    /*
     * Returns true if x lies in the closed interval [min, max] (endpoints included).
     */
    bool contains(double x) const {
        return min <= x && x <= max;
    }

    /*
     * Returns true if x lies in the open interval (min, max) (endpoints excluded).
     *
     *  - Used for ray hits, so a hit exactly at t = min or t = max is rejected.
     */
    bool surrounds(double x) const {
        return min < x && x < max;
    }

    /*
     * Returns x forced into [min, max].
     *
     *  - Values below min become min.
     *  - Values above max become max.
     *  - Values already inside are returned unchanged.
     */
    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    /*
     * Returns a copy of the interval widened by delta (delta/2 on each side).
     */
    interval extend(double delta) const {
        auto padding = delta * 0.5;
        return interval(min - padding, max + padding);
    }

    static const interval empty, universe;
};

inline const interval interval::empty    = interval(+infinity, -infinity);  // Contains nothing
inline const interval interval::universe = interval(-infinity, +infinity);  // Contains every number

#endif // INTERVAL_H
