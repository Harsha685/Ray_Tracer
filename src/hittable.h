#ifndef HITTABLE_H
#define HITTABLE_H

#include "common.h"
#include "interval.h"
#include "ray.h"
#include "AABB.h"

class material;  // Defined in material.h; hit_record only needs a pointer to it

// Everything we need to know about a single ray/object intersection
class hit_record {
public:
    material* mat;              // Material of the surface that was hit (owned by the object, not the record)
    vector3 normal;             // Unit surface normal at the hit, always pointing against the ray
    point3 point;               // World position where the ray hit the surface
    double t;                   // Ray parameter at the hit: point = origin + t * direction
    double u = 0;               // Surface texture coordinate
    double v = 0;               // Surface texture coordinate
    bool front_face;            // True if the ray hit the outside of the surface

    /*
     * Stores the surface normal so that it always faces the incoming ray, and records
     * which side of the surface was hit.
     *
     *  - outward_normal must be unit length and point out of the object
     *    (e.g. away from a sphere's center).
     *  - If the ray direction and outward_normal point in opposite directions
     *    (dot product < 0), the ray came from outside: front_face = true and the
     *    normal is kept as is.
     *  - Otherwise the ray is inside the object: front_face = false and the normal
     *    is flipped.
     *  - Shading code can then always assume the normal faces the ray.
     */
    void set_face_normal(const ray& r, const vector3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Abstract base class for anything a ray can hit (spheres, lists of objects, ...)
class hittable {
public:
    /*
     * Virtual destructor.
     *
     *  - Ensures deleting a derived object through a hittable pointer
     *    (e.g. a shared_ptr<hittable>) runs the derived class's destructor.
     */
    virtual ~hittable() = default;

    /*
     * Tests whether ray r hits this object at some t strictly inside ray_t.
     *
     *  - On a hit, fills rec with the hit details and returns true.
     *  - On a miss, returns false and rec should be treated as unchanged.
     *  - Each shape implements its own intersection math.
     */
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

    /*
     * Returns a box that fully encloses the object (over the whole shutter interval
     * for moving objects).
     */
    virtual aabb bounding_box() const = 0;
};

// Wraps an object and moves it by offset.
class translate : public hittable {
    public:
        translate(shared_ptr<hittable> object, const vector3& offset)
            : object(object), offset(offset)
        {
            bbox = object->bounding_box() + offset;
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            ray offset_r(r.origin() - offset, r.direction(), r.time());

            if (!object->hit(offset_r, ray_t, rec))
                return false;

            rec.point += offset;

            return true;
        }

        aabb bounding_box() const override { return bbox; }

    private:
        shared_ptr<hittable> object;
        vector3 offset;
        aabb bbox;
};

// Wraps an object and rotates it by angle degrees around the y-axis (through the world origin).
class rotate_y : public hittable {
    public:
        rotate_y(shared_ptr<hittable> object, double angle) : object(object) {
            auto radians = degrees_to_radians(angle);
            sin_theta = std::sin(radians);
            cos_theta = std::cos(radians);
            bbox = object->bounding_box();

            // Rotate all 8 corners of the object's box and take the box enclosing them.
            point3 min( infinity,  infinity,  infinity);
            point3 max(-infinity, -infinity, -infinity);

            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                        auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                        auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                        auto newx =  cos_theta * x + sin_theta * z;
                        auto newz = -sin_theta * x + cos_theta * z;

                        vector3 tester(newx, y, newz);

                        for (int c = 0; c < 3; c++) {
                            min[c] = std::fmin(min[c], tester[c]);
                            max[c] = std::fmax(max[c], tester[c]);
                        }
                    }
                }
            }

            bbox = aabb(min, max);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Transform the ray from world space to object space (rotate by -angle).
            auto origin = point3(
                (cos_theta * r.origin().x()) - (sin_theta * r.origin().z()),
                r.origin().y(),
                (sin_theta * r.origin().x()) + (cos_theta * r.origin().z())
            );

            auto direction = vector3(
                (cos_theta * r.direction().x()) - (sin_theta * r.direction().z()),
                r.direction().y(),
                (sin_theta * r.direction().x()) + (cos_theta * r.direction().z())
            );

            ray rotated_r(origin, direction, r.time());

            // Determine whether an intersection exists in object space (and if so, where).
            if (!object->hit(rotated_r, ray_t, rec))
                return false;

            // Transform the intersection from object space back to world space (rotate by +angle).
            rec.point = point3(
                (cos_theta * rec.point.x()) + (sin_theta * rec.point.z()),
                rec.point.y(),
                (-sin_theta * rec.point.x()) + (cos_theta * rec.point.z())
            );

            rec.normal = vector3(
                (cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
                rec.normal.y(),
                (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
            );

            return true;
        }

        aabb bounding_box() const override { return bbox; }

    private:
        shared_ptr<hittable> object;
        double sin_theta;
        double cos_theta;
        aabb bbox;
};

#endif // HITTABLE_H
