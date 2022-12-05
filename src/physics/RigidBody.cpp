#include "RigidBody.h"


void RigidBody::SetMaster(RigidBodyComponent * master)
{
	this->master = master;
}

RigidBodyComponent * RigidBody::GetMaster()
{
	return this->master;
}

void RigidBody::InitializeCoeff(float mass, const glm::mat3x3& inertia, float impulseCoeff, float frictionCoeff)
{
	SetMass(mass);
	SetInertia(inertia);
	SetImpulseCoefficient(impulseCoeff);
	SetFrictionCoefficient(frictionCoeff);
}



bool RigidBody::IsStatic()
{
	return this->isStatic;
}

void RigidBody::SetStatic(bool isStatic)
{
	this->isStatic = isStatic;
}

void RigidBody::SetMass(float mass)
{
	this->mass = mass;
	this->inverseMass = 1.0f / mass;
}

float RigidBody::GetMass()
{
	return this->mass;
}

float RigidBody::GetInverseMass()
{
	return this->inverseMass;
}

void RigidBody::SetInertia(const glm::mat3x3 & inertia)
{
	this->inertia = inertia;
	this->inverseInertia = glm::inverse(inertia);
}

glm::mat3x3 RigidBody::GetInertia()
{
	return this->inertia;
}

glm::mat3x3 RigidBody::GetInverseInertia()
{
	return this->inverseInertia;
}

float RigidBody::GetImpulseCoefficient()
{
	return this->impulseCoefficient;
}

void RigidBody::SetImpulseCoefficient(float coeff)
{
	if (coeff > 1.0f)
	{
		coeff = 1.0f;
	}
	else if (coeff < 0.0f)
	{
		coeff = 0.0f;
	}
	this->impulseCoefficient = coeff;
}

float RigidBody::GetFrictionCoefficient()
{
	return this->frictionCoefficient;
}

void RigidBody::SetFrictionCoefficient(float coeff)
{
	this->frictionCoefficient = coeff;
}

glm::vec3 RigidBody::GetForce()
{
	return this->force;
}

void RigidBody::SetForce(const glm::vec3 & force)
{
	this->force = force;
}

glm::vec3 RigidBody::GetVelocity()
{
	return this->velocity;
}

void RigidBody::SetVelocity(const glm::vec3 & velocity)
{
	this->velocity = velocity;
}

glm::vec3 RigidBody::GetAngularVelocity()
{
	return this->angularVel;
}

void RigidBody::SetAngularVelocity(const glm::vec3 & angularVelocity)
{
	this->angularVel = angularVelocity;
}

/*
检测是否能进入休眠状态
1. 速度足够小
2. 与其他物体的所有接触的穿透距离足够小
*/
bool RigidBody::CheckIfSleep()
{
	return false;
}

