#pragma once

#include <glm/glm.hpp>

#include "object.h"
#include "material.h"

class Sphere : public Object {
public:
    Sphere(const glm::vec3& center, float radius, const Material& material);
    Intersect rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const override;

private:
    glm::vec3 center;
    float radius;
}; 