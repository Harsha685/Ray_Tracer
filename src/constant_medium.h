#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "material.h"
#include "texture.h"

// A volume of constant density (smoke, fog, mist) filling the inside of a boundary shape.
//
//  - A ray passing through the volume may scatter at any point along the way. The chance
//    of scattering over a small distance dL is density * dL, so the distance travelled
//    before scattering follows an exponential distribution.
//  - If that random distance is longer than the ray's path through the boundary, the ray
//    passes straight through without hitting anything.
//  - The boundary must be convex (sphere, box, ...): the code assumes that once a ray
//    leaves the boundary it never re-enters it.
class constant_medium : public hittable {
public:
    /*
     * Fills boundary with a medium of the given density whose color comes from tex.
     * Higher density means rays scatter sooner, so the volume looks thicker.
     */
    constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<Texture> tex)
        : boundary(boundary), neg_inv_density(-1 / density),
          phase_function(make_shared<isotropic>(tex))
    {}

    /*
     * Same, with a single color.
     */
    constant_medium(shared_ptr<hittable> boundary, double density, const color& albedo)
        : boundary(boundary), neg_inv_density(-1 / density),
          phase_function(make_shared<isotropic>(albedo))
    {}

    /*
     * Finds where (if anywhere) the ray scatters inside the medium.
     *
     *  - Finds the entry point t1 and exit point t2 of the ray through the boundary,
     *    searching the whole line so rays starting inside the volume also work.
     *  - Clamps [t1, t2] to ray_t, and to t >= 0 (no scattering behind the ray origin).
     *  - Samples hit_distance = -(1/density) * ln(random). If it lies beyond the exit
     *    point the ray misses; otherwise the hit is at t1 + hit_distance along the ray.
     *  - Normal, front_face and (u, v) are placeholders: the isotropic material
     *    ignores the first two, and a volume has no surface coordinates.
     */
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record rec1, rec2;

        if (!boundary->hit(r, interval::universe, rec1))
            return false;

        if (!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2))
            return false;

        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto ray_length = r.direction().length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density * std::log(random_double());

        if (hit_distance > distance_inside_boundary)
            return false;

        rec.t = rec1.t + hit_distance / ray_length;
        rec.point = r.at(rec.t);

        rec.normal = vector3(1, 0, 0);  // Arbitrary: the isotropic material ignores it
        rec.front_face = true;          // Also arbitrary
        rec.u = 0;                      // A volume has no surface coordinates; set them so
        rec.v = 0;                      // stale values from an earlier hit are not used
        rec.mat = phase_function;

        return true;
    }

    aabb bounding_box() const override { return boundary->bounding_box(); }

private:
    shared_ptr<hittable> boundary;        // Convex shape enclosing the volume
    double neg_inv_density;               // -1 / density, used to sample the scatter distance
    shared_ptr<material> phase_function;  // Isotropic material applied at scatter points
};

#endif // CONSTANT_MEDIUM_H
