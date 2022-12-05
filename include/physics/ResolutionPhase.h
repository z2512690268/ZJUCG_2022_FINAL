#ifndef _RESOLUTIONPHASE_H
#define _RESOLUTIONPHASE_H

#include <vector>

#include "physics/Contact.h"

class IResolutionPhase
{
public:
	virtual void resolve(std::vector<ContactManifold*>& manifolds) = 0;
};

class ResolutionPhaseConstraintBase : public IResolutionPhase
{
public:
	virtual void resolve(std::vector<ContactManifold*>& manifolds);
	void SolveContactConstranst(ContactPoint& contact);
};

#endif