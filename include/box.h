#pragma once

#include <glm/glm.hpp>
// #include <assert.h>

#include "object.h"
#include "material.h"

/*
 * Axis-aligned bounding box class, for use with the optimized ray-box
 * intersection test described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

class Box : public Object {
public:
	Box(const glm::vec3 &b0, const glm::vec3 &b1, const Material &mat) : Object(mat) { 
		bounds[0] = b0;
		bounds[1] = b1;
	}
	Intersect rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const override;
    Intersect rayIntersect(const Ray &ray) const override;

	// corners
	glm::vec3 bounds[2];
};