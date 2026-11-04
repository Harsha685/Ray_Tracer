#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "hittable_list.h"

// A flat parallelogram: corner Q plus edge vectors u and v. Its four corners are
// Q, Q + u, Q + v and Q + u + v.
class quad : public hittable {
public:
    /*
     * Creates the quad with corner Q and edges u and v.
     *
     *  - n = cross(u, v) is perpendicular to the quad. Its direction (right-hand rule
     *    from u to v) is the quad's front side.
     *  - The quad lies in the plane dot(normal, P) = D, so D = dot(normal, Q).
     *  - w = n / dot(n, n) is cached for hit(), which uses it to turn a hit point into
     *    the quad's own (alpha, beta) coordinates.
     */
    quad(const point3& Q, const vector3& u, const vector3& v, shared_ptr<material> mat)
        : Q(Q), u(u), v(v), mat(mat)
    {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        set_bounding_box();
    }

    /*
     * Sets bbox to the box enclosing all four corners: the union of the boxes spanned
     * by the two diagonals.
     */
    void set_bounding_box() {
        auto bbox_diagonal1 = aabb(Q, Q + u + v);
        auto bbox_diagonal2 = aabb(Q + u, Q + v);
        bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override { return bbox; }

    /*
     * Tests whether ray r hits the quad at some t inside ray_t.
     *
     *  - Ray-plane intersection: t = (D - dot(normal, origin)) / dot(normal, direction).
     *  - A ray (almost) parallel to the plane never hits it.
     *  - The hit point P is then written as Q + alpha * u + beta * v. With p = P - Q,
     *    alpha = dot(w, cross(p, v)) and beta = dot(w, cross(u, p)).
     *  - The point is inside the quad when both alpha and beta are in [0, 1]; those
     *    two values double as the texture coordinates (u, v).
     */
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto denominator = dot(normal, r.direction());

        // No hit if the ray is parallel to the plane
        if (std::fabs(denominator) < 1e-8)
            return false;

        // No hit if the plane is outside the allowed t range
        auto t = (D - dot(normal, r.origin())) / denominator;
        if (!ray_t.contains(t))
            return false;

        // Find the hit point's (alpha, beta) coordinates and check it lies inside the quad
        auto intersection = r.at(t);
        vector3 planar_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
            return false;

        rec.t = t;
        rec.point = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    /*
     * Returns true if the plane point (a, b) lies inside the quad, i.e. a and b are
     * both in [0, 1]. On success it also stores (a, b) as the texture coordinates.
     */
    bool is_interior(double a, double b, hit_record& rec) const {
        const interval unit_interval(0, 1);

        if (!unit_interval.contains(a) || !unit_interval.contains(b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    point3 Q;                   // Starting corner
    vector3 u, v;               // Edge vectors from Q
    vector3 w;                  // n / dot(n, n), used to find a hit point's (alpha, beta)
    shared_ptr<material> mat;   // Surface material
    aabb bbox;                  // Box enclosing the four corners
    vector3 normal;             // Unit normal, cross(u, v) normalized
    double D;                   // Plane constant: dot(normal, P) = D for every P on the plane
};

/*
 * Returns the axis-aligned box with opposite corners a and b as a list of six quads.
 *
 *  - min and max are the corners with the smallest and largest coordinates, so a and b
 *    can be given in any order.
 *  - Each face's edges are ordered so cross(u, v) points out of the box, which makes
 *    front_face true for rays hitting it from outside.
 *  - Rotate or move the result with rotate_y / translate.
 */
inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat) {
    auto sides = make_shared<hittable_list>();

    auto min = point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
    auto max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

    auto dx = vector3(max.x() - min.x(), 0, 0);
    auto dy = vector3(0, max.y() - min.y(), 0);
    auto dz = vector3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}

#endif // QUAD_H
