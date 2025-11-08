#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <cmath>

class Vector3
{
public:
    double x;
    double y;
    double z;

    inline Vector3() : x(0), y(0), z(0) {}
    inline Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    inline Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    inline Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    inline Vector3 operator*(double scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    inline double dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    inline Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    inline double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    inline Vector3 normalize() const {
        double mag = magnitude();
        if (mag == 0) return Vector3(0, 0, 0);
        double invMag = 1.0 / mag;  // 优化：用乘法代替除法
        return Vector3(x * invMag, y * invMag, z * invMag);
    }
};

#endif  // VECTOR3_HPP