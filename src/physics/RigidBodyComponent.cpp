#include "RigidBodyComponent.h"

void RigidBodyComponent::Tick() noexcept
{
}

RigidBodyComponent::RigidBodyComponent()
{

}

RigidBodyComponent::RigidBodyComponent(float mass, const glm::mat4x4& inertia, float impulseCoeff, float frictionCoeff):
	rigidBody(mass, inertia, impulseCoeff, frictionCoeff)
{
}

RigidBody * RigidBodyComponent::GetRigidBody()
{
	return &rigidBody;
}

