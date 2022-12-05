#ifndef _COLLIDER_H
#define _COLLIDER_H

#include <glm/glm.hpp>

enum ColliderType
{
	Plane,
	Box,
	Sphere,
};

class RigidBodyComponent;

class Collider
{
public:
	ColliderType GetColliderType()
	{
		return ct;
	}

	RigidBodyComponent* GetComponent()
	{
		return master;
	}

protected:
	ColliderType ct;
	RigidBodyComponent* master;
};


class ColliderPlane : public Collider
{
public:
	ColliderPlane() : 
		point(0, 0, 0),
		normal(0, 1, 0)
	{
		ct = ColliderType::Plane;
	}
private:
	glm::vec3 point;
	glm::vec3 normal;

public:
	void SetPoint(glm::vec3 p);
	glm::vec3 GetPoint();
	void SetNormal(glm::vec3 n);
	glm::vec3 GetNormal();
};


class ColliderBox : public Collider
{
public:
	ColliderBox() : 
		minPoint(-0.5f, -0.5f, -0.5f),
		maxPoint(0.5f, 0.5f, 0.5f)
	{
		ct = ColliderType::Box;
	}

private:
	glm::vec3 minPoint;
	glm::vec3 maxPoint;

public:
	void SetMinPoint(glm::vec3 minPoint);
	glm::vec3 GetMinPoint();
	void SetMaxPoint(glm::vec3 maxPoint);
	glm::vec3 GetMaxPoint();
};

class ColliderSphere : public Collider
{
public:
	ColliderSphere() :
		center(0, 0, 0),
		radius(1.0f)
	{
		ct = ColliderType::Sphere;
	}
private:
	glm::vec3 center;
	float radius;

public:
	void SetCenter(glm::vec3 c);
	glm::vec3 GetCenter();
	void SetRadius(float r);
	float GetRadius();
};

#endif