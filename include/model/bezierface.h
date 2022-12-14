#pragma once
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
//贝塞尔旋转曲面类
class BezierFace
{
	//顶点数
	int numVertices;
	//索引数(每个四边形片6个顶点)
	int numIndices;

	float texrange_l,texrange_r;

	//uv 分段数
	
	//attributes
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<float> us;
	std::vector<float> rs;
	std::vector<float> ys;
	
	std::vector<int> indices;
	//生成
	void generate(int prec);
	//控制点
	float* controlPoints;
	std::vector<glm::vec2> controlPointsVector;

	float toRadians(float degrees);
	float Bernstein(float u, int index);
public:
	
	BezierFace();
	//BezierFace(int i); 
	BezierFace(std::vector<glm::vec2> controlPointsVector,float l=0,float r=1); 
	
	float getLength();
	int getNumVertices();
	int getNumIndices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec2> getTexCoords();
	std::vector<int> getIndices();

	bool getRadiance(float pos,float &y,float &r);


};
