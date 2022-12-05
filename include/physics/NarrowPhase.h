#ifndef _NARROWPHASE_H
#define _NARROWPHASE_H

#include <vector>

#include "physics/BoardPhase.h"
#include "physics/Contact.h"

class RigidBodyComponent;

class INarrowPhase
{
public:
	virtual void CollideDetection(std::vector<RigidBodyPair>& rigidBodies, std::vector<ContactManifold*>& collisions) = 0;

};

struct sResults
{
	enum eStatus
	{
		Separated,   /* Shapes doesnt penetrate												*/
		Penetrating, /* Shapes are penetrating												*/
		GJK_Failed,  /* GJK phase fail, no big issue, shapes are probably just 'touching'	*/
		EPA_Failed   /* EPA phase fail, bigger problem, need to save parameters, and debug	*/
	} status;

	glm::vec3 witnesses[2];
	glm::vec3 normal;
	float distance;
};

struct MinkowskiDiff
{
	RigidBodyComponent* box1;
	RigidBodyComponent* box2;

	inline glm::vec3 Support1(glm::vec3& dir);
	inline glm::vec3 Support2(glm::vec3& dir);
	glm::vec3 Support(glm::vec3& dir);
};

class NarrowPhaseGJKEPA: public INarrowPhase
{

	virtual void CollideDetection(std::vector<RigidBodyPair>& rigidBodies, std::vector<ContactManifold*>& collisions);

	void InitializeMinkowskiDiff(RigidBodyPair& pair, sResults& result, MinkowskiDiff& diff);
	bool Penetration(RigidBodyPair& pair, glm::vec3& guess, sResults& result);

};


#endif