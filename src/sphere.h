#ifndef SPHERE_H
#define SPHERE_H

#include "common.h"
#include "hittable.h"

class sphere : public hittable {
public:
    sphere(const point3& center, double radius)
        : center(center), radius(std::fmax(0, radius)) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // 1. Calculate oc, a, h, and c
        vector3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        // 2. Calculate the discriminant
        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        // 3. Calculate the square root of the discriminant
        auto sqrt_d = std::sqrt(discriminant);

        // 4. Find the nearest root inside the allowed range
        auto root = (h - sqrt_d) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrt_d) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        // 5. Fill the hit record
        rec.t = root;
        rec.point = r.at(rec.t);
        vector3 outward_normal = (rec.point - center) / radius;
        rec.set_face_normal(r, outward_normal);

        // 6. Report a successful hit
        return true;
    }

private:
    point3 center;
    double radius;
};

#endif // SPHERE_H