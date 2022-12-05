#ifndef _CONTACT_H
#define _CONTACT_H

#include <glm/glm.hpp>
#include "physics/Collider.h"

#define CONTACT_POINT_COUNT 5
#define CONTACT_DRIFTING_THRESHOLD  0.004f

class ContactPoint
{
public:
	ContactPoint(void)
		: normalImpulseSum(0.0f)
		, tangentImpulseSum1(0.0f)
		, tangentImpulseSum2(0.0f)
	{ }

public:
	// contact point data
	glm::vec3 globalPositionA;
	glm::vec3 globalPositionB;
	glm::vec3 localPositionA;
	glm::vec3 localPositionB;

	// these 3 vectors form an orthonormal basis
	glm::vec3 normal; // points from colliderA to colliderB
	glm::vec3 tangent1, tangent2;

	// penetration depth
	float penetrationDistance;

	// for clamping (more on this later)
	float normalImpulseSum;
	float tangentImpulseSum1;
	float tangentImpulseSum2;

};

struct ContactManifold
{
	Collider* colliderA;
	Collider* colliderB;

	// AB间的所有碰撞点
	int contactPointCount = 0;
	ContactPoint contactPoints[CONTACT_POINT_COUNT];
};

#endif