#pragma once

#include <glm/glm.hpp>

#include "material.h"
#include "intersect.h"
#include "ray.h"

class Object {
public:
    Object(const Material& material) : material(material) {}
    virtual Intersect rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const = 0;
    virtual Intersect rayIntersect(const Ray &ray) const = 0;
    Material material;
};