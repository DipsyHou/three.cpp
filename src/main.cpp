#include "vector3.hpp"
#include "plane.hpp"
#include "space.hpp"
#include "viewpoint.hpp"
#include "utils/utils_models.hpp"
#include "utils/utils_loop.hpp"
#include <iostream>

int main()
{
    Space space;

    // Floor plane
    space.addPlane(Plane(
        Vector3(-10, -2, -10),
        Vector3(10, -2, -10),
        Vector3(10, -2, 10)
    ));
    space.addPlane(Plane(
        Vector3(-10, -2, -10),
        Vector3(10, -2, 10),
        Vector3(-10, -2, 10)
    ));
    
    // Wall 1
    space.addPlane(Plane(
        Vector3(-5, -2, 5),
        Vector3(5, -2, 5),
        Vector3(5, 3, 5)
    ));
    space.addPlane(Plane(
        Vector3(-5, -2, 5),
        Vector3(5, 3, 5),
        Vector3(-5, 3, 5)
    ));
    
    // Wall 2
    space.addPlane(Plane(
        Vector3(-5, -2, -5),
        Vector3(-5, -2, 5),
        Vector3(-5, 3, 5)
    ));
    space.addPlane(Plane(
        Vector3(-5, -2, -5),
        Vector3(-5, 3, 5),
        Vector3(-5, 3, -5)
    ));
    
    // Wall 3
    space.addPlane(Plane(
        Vector3(5, -2, -5),
        Vector3(5, 3, -5),
        Vector3(5, 3, 5)
    ));
    space.addPlane(Plane(
        Vector3(5, -2, -5),
        Vector3(5, 3, 5),
        Vector3(5, -2, 5)
    ));

    addCube(space, Vector3(-2, -1, 0), 2.0);

    // addBall(space, Vector3(2, 0, 0), 1.0, 6, 4);

    // addCylinder(space, Vector3(0, -1, -3), 0.5, 2.0, 12);

    Viewpoint viewpoint(
        Vector3(0, 0, 0),  // position
        0.0,                // yaw
        0.0,                // pitch
        90.0,               // fov
        &space,
        1920,                // screenWidth
        1080                 // screenHeight
    );

    runInteractionLoop(viewpoint);
    
    return 0;
}