#ifndef _BOARDPHASE_H
#define _BOARDPHASE_H

#include <vector>

#include "physics/Collider.h"
#include "physics/RigidBody.h"


typedef std::pair<RigidBody *, RigidBody *> RigidBodyPair;

class IBoardPhase
{
public:
	virtual void Update(float dt) = 0;
	virtual std::vector<RigidBodyPair>& GeneratePossiblePair(std::vector<RigidBody*>& rigidbodies, std::vector<RigidBodyPair>& candicate) = 0;
	
};


class BoardPhaseNquared : public IBoardPhase
{
public:
	virtual void Update(float dt);
	virtual std::vector<RigidBodyPair>& GeneratePossiblePair(std::vector<RigidBody*>& rigidbodies, std::vector<RigidBodyPair>& candicate);


};

#endif