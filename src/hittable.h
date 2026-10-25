#ifndef HITTABLE_H
#define HITTABLE_H

#include "common.h"

// Everything we need to know about a single ray/object intersection
class hit_record{
    public:

        vector3 normal;     // Unit surface normal at the hit, always pointing against the ray
        point3 point;       // World position where the ray hit the surface
        double t;           // Ray parameter at the hit: point = origin + t * direction
        bool front_face;    // True if the ray hit the outside of the surface

        // Stores the surface normal so that it always faces the incoming ray, and records
        // which side of the surface was hit.
        //
        // Objects compute their normal pointing outward (e.g. away from a sphere's center).
        // If the ray direction and that outward normal point in opposite directions
        // (dot product < 0), the ray came from outside: front_face = true and the normal is
        // kept as is. Otherwise the ray is inside the object, so front_face = false and the
        // normal is flipped. Shading code can then always assume the normal faces the ray.
        void set_face_normal(const ray& r, const vector3& outward_normal) {
            // outward_normal is assumed to have unit length
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

// Abstract base class for anything a ray can hit (spheres, lists of objects, ...)
class hittable{
    public:
        // Virtual destructor so deleting a derived object through a hittable pointer
        // (e.g. a shared_ptr<hittable>) runs the derived class's destructor correctly.
        virtual ~hittable() = default;

        // Tests whether ray r hits this object at some t strictly inside ray_t.
        // On a hit, fills rec with the hit details and returns true; otherwise returns
        // false and rec should be treated as unchanged. Each shape implements its own math.
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif // HITTABLE_H
