#include "sphere.h"

Sphere::Sphere(const glm::vec3& center, float radius, const Material& material) 
    : center(center), radius(radius), Object(material) {}

Intersect Sphere::rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const {
    glm::vec3 originToCenter = rayOrigin - center;
    
    float a = glm::dot(rayDirection, rayDirection);
    float b = glm::dot(originToCenter, rayDirection) * 2.0f;
    float c = glm::dot(originToCenter, originToCenter) - radius * radius;

    float discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0)
        return Intersect{false};

    float distance = (-b - sqrt(discriminant)) / (2.0f * a);

    if (distance < 0)
        return Intersect{false};

    glm::vec3 point = rayOrigin + distance * rayDirection;
    glm::vec3 normal = glm::normalize(point - center);
    return Intersect{true, distance, point, normal};
}