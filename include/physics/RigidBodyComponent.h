#ifndef _RIGIDBODYCOMPONENT_H
#define _RIGIDBODYCOMPONENT_H

#include <vector>

#include <glm/glm.hpp>
#include "physics/RigidBody.h"
#include "physics/Collider.h"


class RigidBodyComponent
{
public:
	virtual int Initialize() noexcept;
	virtual void Finalize() noexcept;
	virtual void Tick() noexcept;
	RigidBodyComponent();
	RigidBodyComponent(float mass, const glm::mat3x3& inertia, float impulseCoeff, float frictionCoeff);

public:
	RigidBody* GetRigidBody();

private:
	RigidBody rigidBody;
	
};

#endif