#include "Vector3D.h"
#include "math.h"

Vector3D::Vector3D() : Vector3D(0, 0, 0) {}

Vector3D::Vector3D(double x, double y, double z) : x{x}, y{y}, z{z} {}

double Vector3D::dot(const Vector3D &rhs) const
{
    double out = 0;

    out += x * rhs.x;
    out += y * rhs.y;
    out += z * rhs.z;

    return out;
}

Vector3D Vector3D::cross(const Vector3D &rhs) const
{
    return Vector3D((y * rhs.z) - (z * rhs.y), (z * rhs.x) - (x * rhs.z), (x * rhs.y) - (y * rhs.x));
}

void Vector3D::normalize()
{
    float mag = sqrt(dot(*this));
    x /= mag;
    y /= mag;
    z /= mag;
}

Vector3D Vector3D::operator*(double num) const
{
    return Vector3D(x * num, y * num, z * num);
}

Vector3D Vector3D::operator-(const Vector3D &other) const
{
    return Vector3D(x - other.x, y - other.y, z - other.z);
}

Vector3D operator*(double num, const Vector3D &vector)
{
    return vector * num;
}
