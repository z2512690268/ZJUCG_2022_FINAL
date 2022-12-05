#ifndef _RIGIDBODY_H
#define _RIGIDBODY_H

#include <glm/glm.hpp>

class RigidBodyComponent;
class RigidBody
{
public:
	RigidBody();

	void SetMaster(RigidBodyComponent* master);
	RigidBodyComponent* GetMaster();

	void InitializeCoeff(float mass, const glm::mat3x3& inertia, float impulseCoeff, float frictionCoeff);

	bool IsStatic();
	void SetStatic(bool isStatic);

	void SetMass(float m);
	float GetMass();
	float GetInverseMass();

	void SetInertia(const glm::mat3x3& inertia);
	glm::mat3x3 GetInertia();
	glm::mat3x3 GetInverseInertia();

	float GetImpulseCoefficient();
	void SetImpulseCoefficient(float coeff);

	float GetFrictionCoefficient();
	void SetFrictionCoefficient(float coeff);

	glm::vec3 GetForce();
	void SetForce(const glm::vec3& force);

	glm::vec3 GetVelocity();
	void SetVelocity(const glm::vec3& velocity);

	glm::vec3 GetAngularVelocity();
	void SetAngularVelocity(const glm::vec3& angularVelocity);

	bool CheckIfSleep();

protected:


private:
	// 这一帧施加的力
	glm::vec3 force;

	// 当前线性速度
	glm::vec3 velocity;

	// 当前线性加速度
	glm::vec3 acceleration;

	// 当前角速度
	glm::vec3 angularVel;

	// 是否处于休眠状态
	bool isSleep;

	// 是否为静态物体
	bool isStatic;

	// 质量
	float mass;

	// 质量倒数
	float inverseMass;

	// 弹性系数
	// 0 - 完全非弹性碰撞
	// 1 - 完全弹性碰撞
	float impulseCoefficient;

	// 转动惯量
	glm::mat3x3 inertia;

	// 转动惯量逆矩阵
	glm::mat3x3 inverseInertia;

	// 摩擦系数
	float frictionCoefficient;

	RigidBodyComponent* master;

};
#endif