#ifndef PLANE_HPP
#define PLANE_HPP

#include "vector3.hpp"

class Plane
{
public:

    Plane(const Vector3& a, const Vector3& b, const Vector3& c)
        : a(a), b(b), c(c) {}

    // getters
    Vector3 getA() const { return a; }
    Vector3 getB() const { return b; }
    Vector3 getC() const { return c; }

    //setters
    void setA(const Vector3& newA) { a = newA; }
    void setB(const Vector3& newB) { b = newB; }
    void setC(const Vector3& newC) { c = newC; }

    // calculate triangle normal (normalized)
    Vector3 normal() const 
    {
        Vector3 edge1 = b - a;
        Vector3 edge2 = c - a;
        return edge1.cross(edge2).normalize();
    }

private:

    Vector3 a;
    Vector3 b;
    Vector3 c;



};

#endif  // PLANE_HPP