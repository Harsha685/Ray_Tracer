#ifndef HITTABLE_H
#define HITTABLE_H

#include "common.h"

class hit_record{
    public:

        vector3 normal;
        point3 point;
        double t;
        bool front_face;

        void set_face_normal(const ray& r, const vector3& outward_normal) {
            // outward_normal is assumed to have unit length
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable{
    public:
        virtual ~hittable() = default;    
        virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif // HITTABLE_H