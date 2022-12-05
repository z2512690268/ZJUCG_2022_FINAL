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
��ʱ��ÿ֡���ռ�һ�顣�������Լ����ǻ���ά��һ����Ч���ϡ�
*/
void PhysicsSystem::CollectRigidBodies(std::vector<RigidBody*>& activedRigidBodies)
{
	

}



