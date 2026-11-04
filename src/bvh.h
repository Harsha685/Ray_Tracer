#ifndef BVH_H
#define BVH_H

#include "AABB.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>

// Bounding volume hierarchy: a binary tree of boxes over a set of objects.
//
//  - Each node holds two children (objects or smaller nodes) and a box enclosing both.
//  - A ray that misses a node's box skips everything below it, so a hit test costs
//    roughly O(log n) box tests instead of testing all n objects.
class bvh_node : public hittable {
public:
    /*
     * Builds a BVH over every object in list.
     *
     *  - list is taken by value, so the sort below reorders a copy, not the caller's list.
     */
    bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {}

    /*
     * Builds a BVH over objects[start, end).
     *
     *  - Picks a random axis and sorts the span by each object's box minimum on it.
     *  - 1 object: both children point to it. 2 objects: one child each.
     *  - More: splits the sorted span in half and recurses on each half.
     */
    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
        int axis = random_int(0, 2);

        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2) {
            left = objects[start];
            right = objects[start + 1];
        } else {
            std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

            auto mid = start + object_span / 2;
            left = make_shared<bvh_node>(objects, start, mid);
            right = make_shared<bvh_node>(objects, mid, end);
        }

        bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    /*
     * Returns the closest hit among the objects under this node.
     *
     *  - If the ray misses this node's box, it misses everything inside it.
     *  - Otherwise both children are tested. The right child's range is cut off at
     *    the left child's hit, so it only reports a hit that is closer.
     */
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox.hit_interval(r, ray_t))
            return false;

        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<hittable> left;   // First child (an object or a smaller node)
    shared_ptr<hittable> right;  // Second child (may be the same object as left)
    aabb bbox;                   // Box enclosing both children

    /*
     * Orders two objects by the lower edge of their bounding boxes on axis_index.
     */
    static bool box_compare(
        const shared_ptr<hittable>& a, const shared_ptr<hittable>& b, int axis_index
    ) {
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }

    // box_compare fixed to the x, y and z axis, in the two-argument form std::sort needs
    static bool box_x_compare(const shared_ptr<hittable>& a, const shared_ptr<hittable>& b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const shared_ptr<hittable>& a, const shared_ptr<hittable>& b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const shared_ptr<hittable>& a, const shared_ptr<hittable>& b) {
        return box_compare(a, b, 2);
    }
};

#endif // BVH_H
