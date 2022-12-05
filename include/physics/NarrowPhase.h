#ifndef _NARROWPHASE_H
#define _NARROWPHASE_H

#include <vector>

#include "physics/BoardPhase.h"
#include "physics/Contact.h"

class INarrowPhase
{
public:
	virtual void CollideDetection(std::vector<RigidBodyPair>& rigidBodies, std::vector<ContactManifold*>&) = 0;

};

class NarrowPhaseGJKEPA: public INarrowPhase
{
	virtual void CollideDetection(std::vector<RigidBodyPair>& rigidBodies, std::vector<ContactManifold*>&);
};


#endif