class Vector3D
{
public:
    double x;
    double y;
    double z;
    Vector3D();
    Vector3D(double x, double y, double z);
    double dot(const Vector3D &rhs) const;
    Vector3D cross(const Vector3D &rhs) const;
    void normalize();
    Vector3D operator*(double x) const;
    Vector3D operator-(const Vector3D &x) const;
    double getX() const;
    double getY() const;
    double getZ() const;
};

Vector3D operator*(double x, const Vector3D &y);