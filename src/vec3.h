#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include "common.h"

// Vector class
// A 3-component vector of doubles, used for points, directions, and colors.
class vector3 {
public:
    double coords[3];

    // CONSTRUCTORS
    // Creates the zero vector (0, 0, 0).
    vector3() : coords{0, 0, 0} {}

    // Creates the vector (c0, c1, c2).
    vector3(double c0, double c1, double c2)
        : coords{c0, c1, c2} {}

    // GETTERS X, Y, Z
    // Return the first, second, and third component respectively.
    double x() const { return coords[0]; }
    double y() const { return coords[1]; }
    double z() const { return coords[2]; }

    // UNARY MINUS
    // Returns a new vector pointing the opposite way: (-x, -y, -z).
    vector3 operator-() const {
        return vector3(-coords[0], -coords[1], -coords[2]);
    }

    // INDEXING
    // Read-only access to component i (0 = x, 1 = y, 2 = z). No bounds checking.
    double operator[](int i) const {
        return coords[i];
    }

    // Writable access to component i, so v[0] = 5 works. No bounds checking.
    double& operator[](int i) {
        return coords[i];
    }

    // COMPOUND ASSIGNMENT
    // Each of these modifies this vector in place and returns a reference to it,
    // so calls can be chained like (a += b) *= 2.

    // Adds v component by component.
    vector3& operator+=(const vector3& v) {
        coords[0] += v[0];
        coords[1] += v[1];
        coords[2] += v[2];
        return *this;
    }

    // Subtracts v component by component.
    vector3& operator-=(const vector3& v) {
        coords[0] -= v[0];
        coords[1] -= v[1];
        coords[2] -= v[2];
        return *this;
    }

    // Multiplies every component by the scalar m.
    vector3& operator*=(double m) {
        coords[0] *= m;
        coords[1] *= m;
        coords[2] *= m;
        return *this;
    }

    // Divides every component by the scalar d.
    vector3& operator/=(double d) {
        coords[0] /= d;
        coords[1] /= d;
        coords[2] /= d;
        return *this;
    }

    // LENGTH
    // Euclidean length (magnitude): sqrt(x^2 + y^2 + z^2).
    double length() const {
        return std::sqrt(length_squared());
    }

    // Squared length: x^2 + y^2 + z^2. Cheaper than length() because it skips the
    // square root, so prefer it when only comparing lengths.
    double length_squared() const {
        return coords[0] * coords[0]
             + coords[1] * coords[1]
             + coords[2] * coords[2];
    }

    // RANDOM VECTOR
    // Returns a vector whose components are each random in [0, 1).
    static vector3 random() {
        return vector3(random_double(), random_double(), random_double());
    }

    // Returns a vector whose components are each random in [min, max).
    static vector3 random(double min, double max) {
        return vector3(random_double(min,max), random_double(min,max), random_double(min,max));
    }

    bool near_zero() const {
        // True if the vector is very close to zero in all three directions
        auto s = 1e-8;
        return (std::fabs(coords[0]) < s)
            && (std::fabs(coords[1]) < s)
            && (std::fabs(coords[2]) < s);
    }
};

// Alias of vector3
using point3 = vector3;

// OUTPUT
// Prints the vector as "x y z" so it can be streamed with std::cout << v.
inline std::ostream& operator<<(std::ostream& out, const vector3& v) {
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

// VECTOR ADDITION
// Returns (u.x + v.x, u.y + v.y, u.z + v.z).
inline vector3 operator+(const vector3& u, const vector3& v) {
    return vector3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

// VECTOR SUBTRACTION
// Returns (u.x - v.x, u.y - v.y, u.z - v.z). For points, b - a is the vector from a to b.
inline vector3 operator-(const vector3& u, const vector3& v) {
    return vector3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

// COMPONENT-WISE MULTIPLICATION
// Returns (u.x * v.x, u.y * v.y, u.z * v.z). Mainly useful for tinting one color by another.
inline vector3 operator*(const vector3& u, const vector3& v) {
    return vector3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

// COMPONENT-WISE DIVISION
// Returns (u.x / v.x, u.y / v.y, u.z / v.z).
inline vector3 operator/(const vector3& u, const vector3& v) {
    return vector3(u[0] / v[0], u[1] / v[1], u[2] / v[2]);
}

// DOT PRODUCT
// Returns u.x*v.x + u.y*v.y + u.z*v.z, which equals |u| |v| cos(angle between them).
// Positive = they point roughly the same way, zero = perpendicular, negative = opposite.
inline double dot(const vector3& u, const vector3& v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

// CROSS PRODUCT
// Returns a vector perpendicular to both u and v (right-hand rule), with length
// |u| |v| sin(angle between them).
inline vector3 cross(const vector3& u, const vector3& v) {
    return vector3(
        u[1] * v[2] - u[2] * v[1],
        u[2] * v[0] - u[0] * v[2],
        u[0] * v[1] - u[1] * v[0]
    );
}

// SCALAR MULTIPLICATION
// Returns v with every component multiplied by m (written m * v).
inline vector3 operator*(double m, const vector3& v) {
    return vector3(m * v[0], m * v[1], m * v[2]);
}

// Same as above but written v * m.
inline vector3 operator*(const vector3& v, double m) {
    return m * v;
}

// SCALAR DIVISION
// Returns v with every component divided by d (implemented as multiplying by 1/d).
inline vector3 operator/(const vector3& v, double d) {
    return (1.0 / d) * v;
}

// UNIT VECTOR
// Returns a vector pointing the same way as v but with length 1.
inline vector3 unit_vector(const vector3& v) {
    return v / v.length();
}

// Returns a random direction of length 1, uniformly distributed over the unit sphere.
//
// Uses rejection sampling: pick random points in the cube [-1, 1]^3 until one lands
// inside the unit sphere, then normalize it. Points outside the sphere are rejected
// because keeping them would bias directions toward the cube's corners. Points
// extremely close to the center (length^2 <= 1e-160) are also rejected, since
// normalizing them could underflow to zero and produce infinities/NaNs.
inline vector3 random_unit_vector() {
    while (true) {
        auto p = vector3::random(-1,1);
        auto lensq = p.length_squared();
        if (1e-160 < lensq && lensq <= 1)
            return p / std::sqrt(lensq);
    }
}

// Returns a random unit direction in the hemisphere that normal points into.
//
// A random unit vector is generated; if its dot product with the normal is positive it
// already points away from the surface and is returned. Otherwise it points into the
// surface, so it is flipped. Used for diffuse bounces, where light must leave the
// surface on the same side it arrived.
inline vector3 random_on_hemisphere(const vector3& normal){
    vector3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

#endif // VEC3_H
