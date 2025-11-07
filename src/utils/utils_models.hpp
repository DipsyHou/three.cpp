#ifndef UTILS_MODELS_HPP
#define UTILS_MODELS_HPP

#include "space.hpp"

void addCube(Space& space, const Vector3& center, double size)
{
    double half = size / 2.0;

    Vector3 v0(center.x - half, center.y - half, center.z - half);
    Vector3 v1(center.x + half, center.y - half, center.z - half);
    Vector3 v2(center.x + half, center.y + half, center.z - half);
    Vector3 v3(center.x - half, center.y + half, center.z - half);
    Vector3 v4(center.x - half, center.y - half, center.z + half);
    Vector3 v5(center.x + half, center.y - half, center.z + half);
    Vector3 v6(center.x + half, center.y + half, center.z + half);
    Vector3 v7(center.x - half, center.y + half, center.z + half);

    // Front face
    space.addPlane(Plane(v0, v1, v2));
    space.addPlane(Plane(v0, v2, v3));

    // Back face
    space.addPlane(Plane(v5, v4, v7));
    space.addPlane(Plane(v5, v7, v6));

    // Left face
    space.addPlane(Plane(v4, v0, v3));
    space.addPlane(Plane(v4, v3, v7));

    // Right face
    space.addPlane(Plane(v1, v5, v6));
    space.addPlane(Plane(v1, v6, v2));

    // Top face
    space.addPlane(Plane(v3, v2, v6));
    space.addPlane(Plane(v3, v6, v7));

    // Bottom face
    space.addPlane(Plane(v4, v5, v1));
    space.addPlane(Plane(v4, v1, v0));
}

void addBall(Space& space, const Vector3& center, double radius, int segments = 12, int rings = 12)
{
    for (int i = 0; i < rings; ++i) {
        double theta1 = M_PI * i / rings;
        double theta2 = M_PI * (i + 1) / rings;

        for (int j = 0; j < segments; ++j) {
            double phi1 = 2 * M_PI * j / segments;
            double phi2 = 2 * M_PI * (j + 1) / segments;

            Vector3 v0(
                center.x + radius * std::sin(theta1) * std::cos(phi1),
                center.y + radius * std::cos(theta1),
                center.z + radius * std::sin(theta1) * std::sin(phi1)
            );
            Vector3 v1(
                center.x + radius * std::sin(theta2) * std::cos(phi1),
                center.y + radius * std::cos(theta2),
                center.z + radius * std::sin(theta2) * std::sin(phi1)
            );
            Vector3 v2(
                center.x + radius * std::sin(theta2) * std::cos(phi2),
                center.y + radius * std::cos(theta2),
                center.z + radius * std::sin(theta2) * std::sin(phi2)
            );
            Vector3 v3(
                center.x + radius * std::sin(theta1) * std::cos(phi2),
                center.y + radius * std::cos(theta1),
                center.z + radius * std::sin(theta1) * std::sin(phi2)
            );

            space.addPlane(Plane(v0, v1, v2));
            space.addPlane(Plane(v0, v2, v3));
        }
    }
}

#endif