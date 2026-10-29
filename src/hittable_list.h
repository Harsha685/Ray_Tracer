#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "common.h"
#include <vector>

// A collection of hittable objects that itself behaves like a single hittable.
// The renderer can treat the whole scene ("world") as one object.
class hittable_list : public hittable {
public:
    std::vector<shared_ptr<hittable>> objects;

    /*
     * Creates an empty list.
     */
    hittable_list() {}

    /*
     * Creates a list that starts with a single object.
     */
    hittable_list(shared_ptr<hittable> obj) { add(obj); }

    /*
     * Removes every object from the list.
     */
    void clear() {
        objects.clear();
        bbox = aabb();
    }

    /*
     * Appends an object to the list.
     *
     *  - shared_ptr lets the same object be shared by several lists.
     *  - The object is freed automatically once nothing references it.
     */
    void add(shared_ptr<hittable> obj) {
        objects.push_back(obj);
        bbox = aabb(bbox, obj->bounding_box());
    }

    /*
     * Finds the closest object the ray hits within ray_t.
     *
     *  - Every object is tested, but the upper bound of the allowed range shrinks to
     *    the t of the nearest hit found so far.
     *  - A later object therefore only counts if it is closer than everything before it.
     *  - rec ends up describing the visible (frontmost) surface.
     *  - Returns true if at least one object was hit.
     */
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_any = false;
        double closest = ray_t.max;  // t of the nearest hit so far; later hits must be closer than this
        for (const auto& object : objects) {
            if (object->hit(r, interval(ray_t.min, closest), temp_rec)) {
                hit_any = true;
                closest = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_any;
    }

    aabb bounding_box() const override { return bbox; }

private:
    aabb bbox;  // Box enclosing every object in the list
};

#endif // HITTABLE_LIST_H
