#include "physics/PhysicsSystem.h"

PhysicsSystem::PhysicsSystem()
{
	this->boardPhase = new BoardPhaseNquared();
	this->narrowPhase = new NarrowPhaseGJKEPA();
	this->resolutionPhase = new ResolutionPhaseConstraintBase();
	this->integratePhase = new IntegratePhase();
}

int PhysicsSystem::Initialize() noexcept
{
	gravity = glm::vec3(0.0f, -9.80665f, 0.0f);

	

	return 0;
}

void PhysicsSystem::Finalize() noexcept
{
}

void PhysicsSystem::Tick(float deltaTime) noexcept
{
	// boardPhase
	std::vector<RigidBody*> activedRigidBodies;
	CollectRigidBodies(activedRigidBodies);
	boardPhase->Update(deltaTime);

	std::vector<RigidBodyPair> candicate;
	boardPhase->GeneratePossiblePair(activedRigidBodies, candicate);

	// narrowPhase
	std::vector<ContactManifold*> manifolds;
	narrowPhase->CollideDetection(candicate, manifolds);

	// resolutionPhase
	resolutionPhase->resolve(manifolds);

	// integratePhase
	integratePhase->integrate();
	 
}

void PhysicsSystem::AddComponent(RigidBodyComponent * comp)
{
	if (nullptr == comp)
	{
		return;
	}

	components.insert(comp);
}

void PhysicsSystem::DeleteComponent(RigidBodyComponent * comp)
{
	if (nullptr == comp)
	{
		return;
	}

	components.erase(comp);
}

/*
暂时先每帧都收集一遍。后续可以加脏标记或者维护一个有效集合。
*/
void PhysicsSystem::CollectRigidBodies(std::vector<RigidBody*>& activedRigidBodies)
{
	

}



