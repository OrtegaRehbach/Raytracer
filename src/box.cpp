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
	glm::vec3 hitPoint = ray.origin + (ray.direction * distance);
	// Center of the box
	glm::vec3 boxCenter = (bounds[1] + bounds[0]) / 2.0f;
	glm::vec3 boxVec = hitPoint - boxCenter; // Vector from hitPoint to center

	// Calculate UV coordinates based on the intersection point
    float u = (hitPoint.x - bounds[0].x) / (bounds[1].x - bounds[0].x);
    float v = (hitPoint.y - bounds[0].y) / (bounds[1].y - bounds[0].y);

    // Clamp UV coordinates to [0, 1] range
    u = glm::clamp(u, 0.0f, 1.0f);
    v = glm::clamp(v, 0.0f, 1.0f);

	// Normal Calculation
	glm::vec3 normal;
	normal = boxVec / std::max(std::max(std::abs(boxVec.x), std::abs(boxVec.y)), std::abs(boxVec.z));
	normal = glm::clamp(normal, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	normal = glm::normalize(glm::floor(normal * 1.0000001f)); // Unit normal for hitPoint

    return Intersect{true, distance, hitPoint, normal, glm::vec2(u, v)};
}

Intersect Box::rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const {
	Ray ray(rayOrigin, rayDirection);
	return Box::rayIntersect(ray);
}
