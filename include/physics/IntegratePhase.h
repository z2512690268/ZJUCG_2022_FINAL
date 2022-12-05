#ifndef _INTEGRATEPHASE_H
#define _INTEGRATEPHASE_H

class IIntegratePhase
{
public:
	virtual void integrate() = 0;
};

class IntegratePhase : public IIntegratePhase
{
public:
	virtual void integrate();
};

#endif