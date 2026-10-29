#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "vec3.h"
#include "ray.h"

// Axis-aligned bounding box: one interval per axis. Used to cheaply reject rays before testing objects.
class aabb {
public:
    interval x, y, z;

    /*
     * Creates an empty box (every axis interval is empty).
     */
    aabb() {}

    aabb(const interval& x, const interval& y, const interval& z) : x(x), y(y), z(z) {}

    aabb(const point3& a, const point3& b) {
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

    /*
     * Creates the smallest box enclosing both box0 and box1.
     */
    aabb(const aabb& box0, const aabb& box1) {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
    }

    const interval& axis_interval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit_interval(const ray& r, interval i) const{
        const point3& ray_orig = r.origin();
        const vector3& ray_dir  = r.direction();
        
        for (int axis = 0; axis<3 ;axis++){
            const interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];            
            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > i.min) i.min = t0;
                if (t1 < i.max) i.max = t1;
            } else {
                if (t1 > i.min) i.min = t1;
                if (t0 < i.max) i.max = t0;
            }

            if (i.max <= i.min)
                return false;            
        }
        return true;
    }
};

#endif // AABB_H
