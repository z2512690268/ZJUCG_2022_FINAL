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
	// ��һ֡ʩ�ӵ���
	glm::vec3 force;

	// ��ǰ�����ٶ�
	glm::vec3 velocity;

	// ��ǰ���Լ��ٶ�
	glm::vec3 acceleration;

	// ��ǰ���ٶ�
	glm::vec3 angularVel;

	// �Ƿ�������״̬
	bool isSleep;

	// �Ƿ�Ϊ��̬����
	bool isStatic;

	// ����
	float mass;

	// ��������
	float inverseMass;

	// ����ϵ��
	// 0 - ��ȫ�ǵ�����ײ
	// 1 - ��ȫ������ײ
	float impulseCoefficient;

	// ת������
	glm::mat3x3 inertia;

	// ת�����������
	glm::mat3x3 inverseInertia;

	// Ħ��ϵ��
	float frictionCoefficient;

	RigidBodyComponent* master;

};
#endif