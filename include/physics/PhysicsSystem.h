#ifndef _PHYSICSSYSTEM_H
#define _PHYSICSSYSTEM_H




#include <set>
#include <vector>
#include "physics/RigidBodyComponent.h"

#include "physics/BoardPhase.h"
#include "physics/NarrowPhase.h"
#include "physics/ResolutionPhase.h"
#include "physics/IntegratePhase.h"


class World;
class RigidBodyComponent;

class PhysicsSystem
{

public:
	PhysicsSystem();
	virtual int		Initialize() noexcept;
	virtual void	Finalize() noexcept;
	virtual void	Tick(float deltaTime) noexcept;

	virtual void AddComponent(RigidBodyComponent* comp);
	virtual void DeleteComponent(RigidBodyComponent* comp);

	void CollectRigidBodies(std::vector<RigidBody*>& activedRigidBodies);

private:
	std::set<RigidBodyComponent*> components;

private:
	glm::vec3 gravity;
	World* mWorld;

	IBoardPhase* boardPhase;
	INarrowPhase* narrowPhase;
	IResolutionPhase* resolutionPhase;
	IIntegratePhase* integratePhase;

};

#endif