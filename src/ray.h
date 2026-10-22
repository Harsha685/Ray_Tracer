
#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
private:
    point3 orig;
    vector3 dir;

public:
    ray() = default;

    ray(const point3& origin, const vector3& direction)
        : orig(origin), dir(direction) {}

    const point3& origin() const {
        return orig;
    }

    const vector3& direction() const {
        return dir;
    }

    point3 at(double t) const {
        return orig + t * dir;
    }
};

#endif // RAY_H
