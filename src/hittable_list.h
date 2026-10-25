
#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "common.h"
#include <vector>

class hittable_list : public hittable {
    public:
        std::vector<std::shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(shared_ptr<hittable> obj) {add(obj);}

        void clear(){
            objects.clear();
        }

        void add(shared_ptr<hittable> obj){
            objects.push_back(obj);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
            hit_record temp_rec;
            bool hit_any = false;
            double closest = ray_t.max;
            for (const auto& object : objects) {
                if(object->hit(r, interval(ray_t.min, closest), temp_rec)){
                    hit_any = true;
                    closest = temp_rec.t;
                    rec = temp_rec;
                }
            }
            return hit_any;
        }
};

#endif // HITTABLE_LIST_H
