#pragma once

#include <glm/glm.hpp>

class Ray
{
public:
    Ray() {}
    Ray(const glm::vec3 &o, const glm::vec3 &d) {
        origin = o;
        direction = d;
        invDirection = glm::vec3(1 / d.x, 1 / d.y, 1 / d.z);
        sign[0] = (invDirection.x < 0);
        sign[1] = (invDirection.y < 0);
        sign[2] = (invDirection.z < 0);
    }
    Ray(const Ray &r) {
        origin = r.origin;
        direction = r.direction;
        invDirection = r.invDirection;
        sign[0] = r.sign[0];
        sign[1] = r.sign[1];
        sign[2] = r.sign[2];
    }

    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 invDirection;
    int sign[3];
};