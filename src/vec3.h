
#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

// Vector class
class vector3 {
public:
    double coords[3];

    // CONSTRUCTORS
    vector3() : coords{0, 0, 0} {}

    vector3(double c0, double c1, double c2)
        : coords{c0, c1, c2} {}

    // GETTERS X, Y, Z
    double x() const { return coords[0]; }
    double y() const { return coords[1]; }
    double z() const { return coords[2]; }

    // UNARY MINUS
    vector3 operator-() const {
        return vector3(-coords[0], -coords[1], -coords[2]);
    }

    // INDEXING
    double operator[](int i) const {
        return coords[i];
    }

    double& operator[](int i) {
        return coords[i];
    }

    // COMPOUND ASSIGNMENT
    vector3& operator+=(const vector3& v) {
        coords[0] += v[0];
        coords[1] += v[1];
        coords[2] += v[2];
        return *this;
    }

    vector3& operator-=(const vector3& v) {
        coords[0] -= v[0];
        coords[1] -= v[1];
        coords[2] -= v[2];
        return *this;
    }

    vector3& operator*=(double m) {
        coords[0] *= m;
        coords[1] *= m;
        coords[2] *= m;
        return *this;
    }

    vector3& operator/=(double d) {
        coords[0] /= d;
        coords[1] /= d;
        coords[2] /= d;
        return *this;
    }

    // LENGTH
    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return coords[0] * coords[0]
             + coords[1] * coords[1]
             + coords[2] * coords[2];
    }
};

// Alias of vector3
using point3 = vector3;

// OUTPUT
inline std::ostream& operator<<(std::ostream& out, const vector3& v) {
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

// VECTOR ADDITION
inline vector3 operator+(const vector3& u, const vector3& v) {
    return vector3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

// VECTOR SUBTRACTION
inline vector3 operator-(const vector3& u, const vector3& v) {
    return vector3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

// COMPONENT-WISE MULTIPLICATION
inline vector3 operator*(const vector3& u, const vector3& v) {
    return vector3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

// COMPONENT-WISE DIVISION
inline vector3 operator/(const vector3& u, const vector3& v) {
    return vector3(u[0] / v[0], u[1] / v[1], u[2] / v[2]);
}

// DOT PRODUCT
inline double dot(const vector3& u, const vector3& v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

// CROSS PRODUCT
inline vector3 cross(const vector3& u, const vector3& v) {
    return vector3(
        u[1] * v[2] - u[2] * v[1],
        u[2] * v[0] - u[0] * v[2],
        u[0] * v[1] - u[1] * v[0]
    );
}


// SCALAR MULTIPLICATION
inline vector3 operator*(double m, const vector3& v) {
    return vector3(m * v[0], m * v[1], m * v[2]);
}

inline vector3 operator*(const vector3& v, double m) {
    return m * v;
}

// SCALAR DIVISION
inline vector3 operator/(const vector3& v, double d) {
    return (1.0 / d) * v;
}

// UNIT VECTOR
inline vector3 unit_vector(const vector3& v) {
    return v / v.length();
}

#endif // VEC3_H
