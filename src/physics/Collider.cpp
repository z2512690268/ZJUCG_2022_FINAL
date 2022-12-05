#include "physics/Collider.h"


void ColliderPlane::SetPoint(glm::vec3 p)
{
	this->point = p;
}

glm::vec3 ColliderPlane::GetPoint()
{
	return this->point;
}

void ColliderPlane::SetNormal(glm::vec3 n)
{
	this->normal = n;
}

glm::vec3 ColliderPlane::GetNormal()
{
	return this->normal;
}

void ColliderBox::SetMinPoint(glm::vec3 minPoint)
{
	this->minPoint = minPoint;
}

glm::vec3 ColliderBox::GetMinPoint()
{
	return this->minPoint;
}

void ColliderBox::SetMaxPoint(glm::vec3 maxPoint)
{
	this->maxPoint = maxPoint;
}

glm::vec3 ColliderBox::GetMaxPoint()
{
	return this->maxPoint;
}

void ColliderSphere::SetCenter(glm::vec3 c)
{
	this->center = c;
}

glm::vec3 ColliderSphere::GetCenter()
{
	return this->center;
}

void ColliderSphere::SetRadius(float r)
{
	this->radius = r;
}

float ColliderSphere::GetRadius()
{
	return this->radius;
}
