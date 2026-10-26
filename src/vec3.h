#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include "common.h"

// A 3-component vector of doubles, used for points, directions, and colors.
class vector3 {
public:
    double coords[3];

    // CONSTRUCTORS

    /*
     * Creates the zero vector (0, 0, 0).
     */
    vector3() : coords{0, 0, 0} {}

    /*
     * Creates the vector (c0, c1, c2).
     */
    vector3(double c0, double c1, double c2)
        : coords{c0, c1, c2} {}

    // GETTERS

    /*
     * Return the first, second, and third component respectively.
     */
    double x() const { return coords[0]; }
    double y() const { return coords[1]; }
    double z() const { return coords[2]; }

    // UNARY MINUS

    /*
     * Returns a new vector pointing the opposite way: (-x, -y, -z).
     */
    vector3 operator-() const {
        return vector3(-coords[0], -coords[1], -coords[2]);
    }

    // INDEXING

    /*
     * Read-only access to component i.
     *
     *  - 0 = x, 1 = y, 2 = z.
     *  - No bounds checking.
     */
    double operator[](int i) const {
        return coords[i];
    }

    /*
     * Writable access to component i, so v[0] = 5 works.
     *
     *  - No bounds checking.
     */
    double& operator[](int i) {
        return coords[i];
    }

    // COMPOUND ASSIGNMENT
    // Each of these modifies this vector in place and returns a reference to it,
    // so calls can be chained like (a += b) *= 2.

    /*
     * Adds v component by component.
     */
    vector3& operator+=(const vector3& v) {
        coords[0] += v[0];
        coords[1] += v[1];
        coords[2] += v[2];
        return *this;
    }

    /*
     * Subtracts v component by component.
     */
    vector3& operator-=(const vector3& v) {
        coords[0] -= v[0];
        coords[1] -= v[1];
        coords[2] -= v[2];
        return *this;
    }

    /*
     * Multiplies every component by the scalar m.
     */
    vector3& operator*=(double m) {
        coords[0] *= m;
        coords[1] *= m;
        coords[2] *= m;
        return *this;
    }

    /*
     * Divides every component by the scalar d.
     */
    vector3& operator/=(double d) {
        coords[0] /= d;
        coords[1] /= d;
        coords[2] /= d;
        return *this;
    }

    // LENGTH

    /*
     * Euclidean length (magnitude): sqrt(x^2 + y^2 + z^2).
     */
    double length() const {
        return std::sqrt(length_squared());
    }

    /*
     * Squared length: x^2 + y^2 + z^2.
     *
     *  - Cheaper than length() because it skips the square root.
     *  - Prefer it when only comparing lengths.
     */
    double length_squared() const {
        return coords[0] * coords[0]
             + coords[1] * coords[1]
             + coords[2] * coords[2];
    }

    // RANDOM VECTORS

    /*
     * Returns a vector whose components are each random in [0, 1).
     */
    static vector3 random() {
        return vector3(random_double(), random_double(), random_double());
    }

    /*
     * Returns a vector whose components are each random in [min, max).
     */
    static vector3 random(double min, double max) {
        return vector3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

    /*
     * Returns true if the vector is very close to zero in all three dimensions.
     *
     *  - Each component is compared against a tolerance of 1e-8.
     *  - Used to catch degenerate scatter directions that would produce NaNs or
     *    infinities later on.
     */
    bool near_zero() const {
        auto s = 1e-8;
        return (std::fabs(coords[0]) < s)
            && (std::fabs(coords[1]) < s)
            && (std::fabs(coords[2]) < s);
    }
};

// Alias of vector3, used where the value is a position rather than a direction
using point3 = vector3;

// OUTPUT

/*
 * Prints the vector as "x y z" so it can be streamed with std::cout << v.
 */
inline std::ostream& operator<<(std::ostream& out, const vector3& v) {
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

// ARITHMETIC

/*
 * Vector addition: returns (u.x + v.x, u.y + v.y, u.z + v.z).
 */
inline vector3 operator+(const vector3& u, const vector3& v) {
    return vector3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

/*
 * Vector subtraction: returns (u.x - v.x, u.y - v.y, u.z - v.z).
 *
 *  - For points, b - a is the vector from a to b.
 */
inline vector3 operator-(const vector3& u, const vector3& v) {
    return vector3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

/*
 * Component-wise multiplication: returns (u.x * v.x, u.y * v.y, u.z * v.z).
 *
 *  - Mainly used for tinting one color by another (e.g. attenuation).
 */
inline vector3 operator*(const vector3& u, const vector3& v) {
    return vector3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

/*
 * Component-wise division: returns (u.x / v.x, u.y / v.y, u.z / v.z).
 */
inline vector3 operator/(const vector3& u, const vector3& v) {
    return vector3(u[0] / v[0], u[1] / v[1], u[2] / v[2]);
}

/*
 * Dot product: returns u.x*v.x + u.y*v.y + u.z*v.z.
 *
 *  - Equals |u| |v| cos(angle between them).
 *  - Positive: they point roughly the same way.
 *  - Zero: they are perpendicular.
 *  - Negative: they point in roughly opposite directions.
 */
inline double dot(const vector3& u, const vector3& v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

/*
 * Cross product: returns a vector perpendicular to both u and v.
 *
 *  - Direction follows the right-hand rule.
 *  - Length is |u| |v| sin(angle between them).
 */
inline vector3 cross(const vector3& u, const vector3& v) {
    return vector3(
        u[1] * v[2] - u[2] * v[1],
        u[2] * v[0] - u[0] * v[2],
        u[0] * v[1] - u[1] * v[0]
    );
}

/*
 * Scalar multiplication written m * v: every component multiplied by m.
 */
inline vector3 operator*(double m, const vector3& v) {
    return vector3(m * v[0], m * v[1], m * v[2]);
}

/*
 * Scalar multiplication written v * m. Forwards to m * v.
 */
inline vector3 operator*(const vector3& v, double m) {
    return m * v;
}

/*
 * Scalar division: every component divided by d.
 *
 *  - Implemented as multiplication by 1 / d (one division instead of three).
 */
inline vector3 operator/(const vector3& v, double d) {
    return (1.0 / d) * v;
}

// GEOMETRY HELPERS

/*
 * Returns a vector pointing the same way as v but with length 1.
 */
inline vector3 unit_vector(const vector3& v) {
    return v / v.length();
}

/*
 * Returns a random direction of length 1, uniformly distributed over the unit sphere.
 *
 *  - Rejection sampling: pick random points in the cube [-1, 1]^3 until one lands
 *    inside the unit sphere, then normalize it.
 *  - Points outside the sphere are rejected because keeping them would bias
 *    directions toward the cube's corners.
 *  - Points extremely close to the center (length^2 <= 1e-160) are also rejected,
 *    since normalizing them could underflow to zero and produce infinities/NaNs.
 */
inline vector3 random_unit_vector() {
    while (true) {
        auto p = vector3::random(-1, 1);
        auto lensq = p.length_squared();
        if (1e-160 < lensq && lensq <= 1)
            return p / std::sqrt(lensq);
    }
}

/*
 * Returns a random unit direction in the hemisphere that normal points into.
 *
 *  - A random unit vector is generated; if its dot product with the normal is
 *    positive it already points away from the surface and is returned as is.
 *  - Otherwise it points into the surface, so it is flipped.
 *  - This was the first diffuse model. lambertian now uses normal + random_unit_vector()
 *    instead, so this function is kept for reference and experiments.
 */
inline vector3 random_on_hemisphere(const vector3& normal) {
    vector3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

/*
 * Mirror reflection of v about the surface normal n.
 *
 *  - n must be unit length.
 *  - dot(v, n) * n is the part of v along the normal; subtracting it twice flips
 *    that part while keeping the part parallel to the surface.
 *  - Result: v - 2 * dot(v, n) * n.
 */
inline vector3 reflect(const vector3& v, const vector3& n) {
    return v - 2 * dot(v, n) * n;
}

/*
 * Refracts the unit direction v through a surface with unit normal n (Snell's law).
 *
 *  - n1_over_n2 is the ratio of refractive indices (incoming medium / outgoing medium).
 *  - The outgoing ray is split into a part perpendicular to n and a part parallel to n.
 *  - Perpendicular part: n1_over_n2 * (v + cos_theta * n).
 *  - Parallel part: -sqrt(|1 - |perp|^2|) * n, which makes the result unit length.
 *  - Does not check for total internal reflection; the caller (dielectric) does that.
 */
inline vector3 refract(const vector3& v, const vector3& n, double n1_over_n2) {
    auto cos_theta = std::fmin(dot(-v, n), 1.0);
    vector3 r_out_perp = n1_over_n2 * (v + cos_theta * n);
    vector3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

/*
 * Returns a random point inside the unit disk on the z = 0 plane.
 *
 *  - Rejection sampling: pick (x, y) in the square [-1, 1]^2 until one lands inside
 *    the circle of radius 1.
 *  - Used by the camera to pick a ray origin on the lens for defocus blur.
 */
inline vector3 random_in_unit_disk() {
    while (true) {
        auto p = vector3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

#endif // VEC3_H
