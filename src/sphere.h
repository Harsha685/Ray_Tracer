#ifndef SPHERE_H
#define SPHERE_H

#include "common.h"
#include "hittable.h"
#include "material.h"
// A sphere defined by a center, a radius, and the material of its surface.
class sphere : public hittable {
public:
    /*
     * Creates a stationary sphere with the given center, radius and material.
     *
     *  - A negative radius makes no sense, so it is clamped to 0 with std::fmax.
     */
    sphere(const point3& static_center, double radius, shared_ptr<material> mat)
        : center(static_center, vector3(0, 0, 0)), radius(std::fmax(0, radius)), mat(mat) {
            auto rvec = vector3(radius, radius, radius);
            bound_box = aabb(static_center - rvec, static_center + rvec);
        }

    /*
     * Creates a moving sphere whose center travels from center1 at time 0 to center2
     * at time 1.
     */
    sphere(const point3& center1, const point3& center2, double radius,
           shared_ptr<material> mat)
        : center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat)
    {
        // Box must enclose the sphere at both ends of its motion
        auto rvec = vector3(radius, radius, radius);
        aabb box1(center.at(0) - rvec, center.at(0) + rvec);
        aabb box2(center.at(1) - rvec, center.at(1) + rvec);
        bound_box = aabb(box1, box2);
    }

    /*
     * Tests whether ray r hits this sphere at some t strictly inside ray_t.
     *
     *  - A point P is on the sphere when |P - center|^2 = radius^2.
     *  - Substituting the ray P(t) = origin + t * direction gives a quadratic in t.
     *  - Discriminant < 0: the ray misses. = 0: it grazes the surface. > 0: it enters
     *    and exits.
     *  - The smaller valid root (nearer hit) is taken first; the larger one is used only
     *    if the smaller is outside ray_t (e.g. the ray starts inside the sphere).
     *  - On a hit, rec gets t, the hit point, the face-corrected normal and the material.
     */
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // 1. Calculate oc, a, h, and c
        // Solves the quadratic a*t^2 - 2h*t + c = 0 for where the ray meets the sphere
        point3 current_center = center.at(r.time());     // Where the sphere is at the ray's time
        vector3 oc = current_center - r.origin();        // Vector from ray origin to sphere center
        auto a = r.direction().length_squared();         // Quadratic 'a' coefficient
        auto h = dot(r.direction(), oc);                 // Half of the quadratic 'b' coefficient (b = -2h)
        auto c = oc.length_squared() - radius * radius;  // Quadratic 'c' coefficient

        // 2. Calculate the discriminant
        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        // 3. Calculate the square root of the discriminant
        auto sqrt_d = std::sqrt(discriminant);

        // 4. Find the nearest root inside the allowed range
        auto root = (h - sqrt_d) / a;  // Candidate t value; try the nearer intersection first
        if (!ray_t.surrounds(root)) {
            root = (h + sqrt_d) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        // 5. Fill the hit record
        rec.t = root;
        rec.point = r.at(rec.t);
        vector3 outward_normal = (rec.point - current_center) / radius;  // Dividing by radius makes it unit length
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        // 6. Report a successful hit
        return true;
    }

    aabb bounding_box() const override { return bound_box; }

private:
    ray center;                 // Center path: center.at(time) is the center at that time
    double radius;              // Radius, never negative
    shared_ptr<material> mat;   // Surface material, shared with any other object using it
    aabb bound_box;             // Box enclosing the sphere over its whole motion
};

#endif // SPHERE_H
