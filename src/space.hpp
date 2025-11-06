#ifndef SPACE_HPP
#define SPACE_HPP

#include "plane.hpp"
#include <vector>

class Space
{
public:
    Space() {}
    
    void addPlane(const Plane& plane)
    {
        planes.push_back(plane);
    }

    const std::vector<Plane>& getPlanes() const { return planes; }

private:
    std::vector<Plane> planes;
};

#endif