#include "box.h"
#include "ray.h"
#include "intersect.h"

/*
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

glm::vec2 Box::calculateUV(const glm::vec3& hitVector) const {
    // Calculate UV coordinates based on the direction of the intersecting ray
    float u, v;

    if (std::abs(hitVector.z) > std::max(std::abs(hitVector.x), std::abs(hitVector.y))) {
        // Front or back face
        u = (hitVector.x - bounds[0].x) / (bounds[1].x - bounds[0].x);
        v = (hitVector.y - bounds[0].y) / (bounds[1].y - bounds[0].y);
    } else if (std::abs(hitVector.x) > std::abs(hitVector.y)) {
        // Left or right face
        u = (hitVector.z - bounds[0].z) / (bounds[1].z - bounds[0].z);
        v = (hitVector.y - bounds[0].y) / (bounds[1].y - bounds[0].y);
    } else {
        // Top or bottom face
        u = (hitVector.x - bounds[0].x) / (bounds[1].x - bounds[0].x);
        v = (hitVector.z - bounds[0].z) / (bounds[1].z - bounds[0].z);
    }

    // Clamp UV coordinates to [0, 1] range
    u = glm::clamp(u, 0.0f, 1.0f);
    v = glm::clamp(v, 0.0f, 1.0f);

    return glm::vec2(u, v);
}

Intersect Box::rayIntersect(const Ray &ray) const {
	float tmin, tmax, tymin, tymax, tzmin, tzmax, distance;

	tmin = (bounds[ray.sign[0]].x - ray.origin.x) * ray.invDirection.x;
	tmax = (bounds[1-ray.sign[0]].x - ray.origin.x) * ray.invDirection.x;
	tymin = (bounds[ray.sign[1]].y - ray.origin.y) * ray.invDirection.y;
	tymax = (bounds[1-ray.sign[1]].y - ray.origin.y) * ray.invDirection.y;

	if ((tmin > tymax) || (tymin > tmax))
		return Intersect{false};

	if (tymin > tmin)
	tmin = tymin;
	if (tymax < tmax)
	tmax = tymax;

	tzmin = (bounds[ray.sign[2]].z - ray.origin.z) * ray.invDirection.z;
	tzmax = (bounds[1-ray.sign[2]].z - ray.origin.z) * ray.invDirection.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return Intersect{false};

	if (tzmin > tmin)
	tmin = tzmin;
	if (tzmax < tmax)
	tmax = tzmax;

	distance = tmin;

	if (distance < 0) {
		distance = tmax;
		if (distance < 0) return Intersect{false};
	}
	// Normal calculation method from:
	// https://www.reddit.com/r/raytracing/comments/yxaabc/ray_box_intersection_normal/
	// https://www.shadertoy.com/view/wtSyRd
	
	// Intersection point on box
	glm::vec3 hitPoint = ray.origin + ray.direction * distance;
    glm::vec3 hitVector = hitPoint - bounds[0];

	// Calculate UV coordinates based on the intersection point
    float u = (hitPoint.x - bounds[0].x) / (bounds[1].x - bounds[0].x);
    float v = (hitPoint.y - bounds[0].y) / (bounds[1].y - bounds[0].y);

    // Clamp UV coordinates to [0, 1] range
    u = glm::clamp(u, 0.0f, 1.0f);
    v = glm::clamp(v, 0.0f, 1.0f);

    glm::vec2 uv = calculateUV(hitPoint);
	// glm::vec2 uv(u, v);

	// Normal Calculation
	glm::vec3 normal;
	// normal = boxVec / std::max(std::max(std::abs(boxVec.x), std::abs(boxVec.y)), std::abs(boxVec.z));
	// normal = glm::clamp(normal, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	// normal = glm::normalize(glm::floor(normal * 1.0000001f)); // Unit normal for hitPoint

	const float epsilon = 1e-6f; // Adjust the epsilon value as needed

	if (std::abs(hitPoint.x - bounds[0].x) < epsilon) {
		normal = glm::vec3(-1.0f, 0.0f, 0.0f);  // Left face
	} else if (std::abs(hitPoint.x - bounds[1].x) < epsilon) {
		normal = glm::vec3(1.0f, 0.0f, 0.0f);   // Right face
	} else if (std::abs(hitPoint.y - bounds[0].y) < epsilon) {
		normal = glm::vec3(0.0f, -1.0f, 0.0f);  // Bottom face
	} else if (std::abs(hitPoint.y - bounds[1].y) < epsilon) {
		normal = glm::vec3(0.0f, 1.0f, 0.0f);   // Top face
	} else if (std::abs(hitPoint.z - bounds[0].z) < epsilon) {
		normal = glm::vec3(0.0f, 0.0f, -1.0f);  // Back face
	} else if (std::abs(hitPoint.z - bounds[1].z) < epsilon) {
		normal = glm::vec3(0.0f, 0.0f, 1.0f);   // Front face
	}

    return Intersect{true, distance, hitPoint, normal, uv};
}

Intersect Box::rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const {
	Ray ray(rayOrigin, rayDirection);
	return Box::rayIntersect(ray);
}
