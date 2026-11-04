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

    /*
     * Creates a box from one interval per axis, padded so no side is thinner than 0.0001.
     */
    aabb(const interval& x, const interval& y, const interval& z) : x(x), y(y), z(z) {
        pad_to_minimums();
    }

    /*
     * Creates the box with opposite corners a and b (in any order), padded so no side is
     * thinner than 0.0001.
     */
    aabb(const point3& a, const point3& b) {
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
        pad_to_minimums();
    }

    /*
     * Creates the smallest box enclosing both box0 and box1.
     */
    aabb(const aabb& box0, const aabb& box1) {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
    }

    /*
     * Returns the box's interval on axis n (0 = x, 1 = y, 2 = z).
     */
    const interval& axis_interval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    /*
     * Returns true if ray r passes through the box at some t inside the interval i
     * (the slab method).
     *
     *  - Per axis, the box is the slab between two parallel planes. The ray crosses
     *    them at t0 = (min - origin) / direction and t1 = (max - origin) / direction.
     *  - A negative direction swaps which plane is crossed first, hence the t0 < t1 check.
     *  - Each axis narrows i to the t range spent inside that slab. The ray is inside the
     *    box only where all three ranges overlap, so an empty i means a miss.
     *  - Dividing by a zero direction gives +-infinity, which the comparisons handle.
     */
    bool hit_interval(const ray& r, interval i) const {
        const point3& ray_orig = r.origin();
        const vector3& ray_dir = r.direction();

        for (int axis = 0; axis < 3; axis++) {
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

private:
    /*
     * Widens any side thinner than delta.
     *
     *  - A flat object (e.g. an axis-aligned quad) would otherwise get a box of zero
     *    thickness, which the slab test in hit_interval() can miss.
     */
    void pad_to_minimums() {
        double delta = 0.0001;
        if (x.size() < delta) x = x.extend(delta);
        if (y.size() < delta) y = y.extend(delta);
        if (z.size() < delta) z = z.extend(delta);
    }
};

/*
 * Returns the box translated by offset (each axis interval is shifted by that axis's component).
 */
inline aabb operator+(const aabb& bbox, const vector3& offset) {
    return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

inline aabb operator+(const vector3& offset, const aabb& bbox) {
    return bbox + offset;
}

#endif // AABB_H
