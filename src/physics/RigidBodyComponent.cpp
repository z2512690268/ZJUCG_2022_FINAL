#include "RigidBodyComponent.h"

void RigidBodyComponent::Tick() noexcept
{
}

RigidBodyComponent::RigidBodyComponent()
{

}

RigidBodyComponent::RigidBodyComponent(float mass, const glm::mat3x3& inertia, float impulseCoeff, float frictionCoeff):
	rigidBody(mass, inertia, impulseCoeff, frictionCoeff)
{
}

RigidBody * RigidBodyComponent::GetRigidBody()
{
	return &rigidBody;
}

