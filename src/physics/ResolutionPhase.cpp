#include "ResolutionPhase.h"

void ResolutionPhaseConstraintBase::resolve(std::vector<ContactManifold*>& manifolds)
{
	for(ContactManifold* manifold : manifolds)
	{
		for (int i = 0; i < manifold->contactPointCount; i++)
		{
			SolveContactConstranst(manifold->contactPoints[i]);
		}
	}
}

void ResolutionPhaseConstraintBase::SolveContactConstranst(ContactPoint & contact)
{

}
