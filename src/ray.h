#ifndef RAY_H
#define RAY_H

#include "vec3.h"

// A half-line in 3D: P(t) = origin + t * direction, for t >= 0
class ray {
private:
    point3 orig;   // Starting point of the ray
    vector3 dir;   // Direction of the ray (not necessarily unit length)
    double tm = 0; // Moment in time the ray exists at, used for motion blur

public:
    /*
     * Creates a ray with origin and direction both (0, 0, 0).
     *
     *  - Needed so a ray can be declared before it is assigned.
     */
    ray() = default;

    /*
     * Creates a ray starting at origin and travelling along direction at the given time.
     */
    ray(const point3& origin, const vector3& direction, double time)
        : orig(origin), dir(direction), tm(time) {}

    /*
     * Creates a ray starting at origin and travelling along direction at time 0.
     */
    ray(const point3& origin, const vector3& direction)
        : ray(origin, direction, 0) {}

    /*
     * Returns the ray's starting point.
     */
    const point3& origin() const {
        return orig;
    }

    /*
     * Returns the ray's direction vector (as given, not normalized).
     */
    const vector3& direction() const {
        return dir;
    }

    /*
     * Returns the moment in time the ray exists at.
     */
    double time() const {
        return tm;
    }

    /*
     * Returns the point reached after travelling t times the direction from the origin.
     *
     *  - t = 0 is the origin.
     *  - t = 1 is origin + direction.
     *  - Negative t is behind the origin.
     */
    point3 at(double t) const {
        return orig + t * dir;
    }
};

#endif // RAY_H
