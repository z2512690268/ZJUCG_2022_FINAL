#include "pipeline.h"
#include "debug.h"
#include "camera.h"
#include "gtest/gtest.h"

TEST(TestCaseName, TestName)
{
	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}

TEST(TransformTest, Translate)
{
	Transform t;
	t.m_pos = glm::vec3(1.0f, 2.0f, 3.0f);
	glm::mat4x4 m = t.GetTranslationMatrix();
/*
1.000000 0.000000 0.000000 1.000000
0.000000 1.000000 0.000000 2.000000
0.000000 0.000000 1.000000 3.000000
0.000000 0.000000 0.000000 1.000000
*/
	EXPECT_NEAR(m[0][0], 1.0f, 0.01f);
	EXPECT_NEAR(m[0][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[0][2], 0.0f, 0.01f);
	EXPECT_NEAR(m[0][3], 1.0f, 0.01f);
	EXPECT_NEAR(m[1][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[1][1], 1.0f, 0.01f);
	EXPECT_NEAR(m[1][2], 0.0f, 0.01f);
	EXPECT_NEAR(m[1][3], 2.0f, 0.01f);
	EXPECT_NEAR(m[2][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[2][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[2][2], 1.0f, 0.01f);
	EXPECT_NEAR(m[2][3], 3.0f, 0.01f);
	EXPECT_NEAR(m[3][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][2], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][3], 1.0f, 0.01f);
}

TEST(TransformTest, Scale)
{
	Transform t;
	t.m_scale = glm::vec3(1.0f, 2.0f, 3.0f);
	glm::mat4x4 m = t.GetScaleMatrix();
/*
1.000000 0.000000 0.000000 0.000000
0.000000 2.000000 0.000000 0.000000
0.000000 0.000000 3.000000 0.000000
0.000000 0.000000 0.000000 1.000000
*/
	EXPECT_NEAR(m[0][0], 1.0f, 0.01f);
	EXPECT_NEAR(m[0][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[0][2], 0.0f, 0.01f);
	EXPECT_NEAR(m[0][3], 0.0f, 0.01f);
	EXPECT_NEAR(m[1][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[1][1], 2.0f, 0.01f);
	EXPECT_NEAR(m[1][2], 0.0f, 0.01f);
	EXPECT_NEAR(m[1][3], 0.0f, 0.01f);
	EXPECT_NEAR(m[2][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[2][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[2][2], 3.0f, 0.01f);
	EXPECT_NEAR(m[2][3], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][2], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][3], 1.0f, 0.01f);
}

TEST(TransformTest, Rotate)
{
	Transform t;
	t.m_rotation = glm::vec3(1.0f, 2.0f, 3.0f);
	glm::mat4x4 m = t.GetRotationMatrix();
/*
0.998021 0.052936 -0.033933 0.000000
-0.052304 0.998446 0.019255 0.000000
0.034899 -0.017442 0.999239 0.000000
0.000000 0.000000 0.000000 1.000000
*/
	EXPECT_NEAR(m[0][0], 0.998021f, 0.01f);
	EXPECT_NEAR(m[0][1], 0.052936f, 0.01f);
	EXPECT_NEAR(m[0][2], -0.033933f, 0.01f);
	EXPECT_NEAR(m[0][3], 0.0f, 0.01f);
	EXPECT_NEAR(m[1][0], -0.052304f, 0.01f);
	EXPECT_NEAR(m[1][1], 0.998446f, 0.01f);
	EXPECT_NEAR(m[1][2], 0.019255f, 0.01f);
	EXPECT_NEAR(m[1][3], 0.0f, 0.01f);
	EXPECT_NEAR(m[2][0], 0.034899f, 0.01f);
	EXPECT_NEAR(m[2][1], -0.017442f, 0.01f);
	EXPECT_NEAR(m[2][2], 0.999239f, 0.01f);
	EXPECT_NEAR(m[2][3], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][2], 0.0f, 0.01f);
}

TEST(TranformTest, FullTrans)
{
	Transform t;
	t.m_pos = glm::vec3(1.0f, 2.0f, 3.0f);
	t.m_scale = glm::vec3(1.0f, 2.0f, 3.0f);
	t.m_rotation = glm::vec3(1.0f, 2.0f, 3.0f);
	glm::mat4x4 m = t.GetMatrix();
/*
0.998021 0.105872 -0.101799 1.000000
-0.052304 1.996891 0.057764 2.000000
0.034899 -0.034884 2.997716 3.000000
0.000000 0.000000 0.000000 1.000000
*/
	EXPECT_NEAR(m[0][0], 0.998021f, 0.01f);
	EXPECT_NEAR(m[0][1], 0.105872f, 0.01f);
	EXPECT_NEAR(m[0][2], -0.101799f, 0.01f);
	EXPECT_NEAR(m[0][3], 1.0f, 0.01f);
	EXPECT_NEAR(m[1][0], -0.052304f, 0.01f);
	EXPECT_NEAR(m[1][1], 1.996891f, 0.01f);
	EXPECT_NEAR(m[1][2], 0.057764f, 0.01f);
	EXPECT_NEAR(m[1][3], 2.0f, 0.01f);
	EXPECT_NEAR(m[2][0], 0.034899f, 0.01f);
	EXPECT_NEAR(m[2][1], -0.034884f, 0.01f);
	EXPECT_NEAR(m[2][2], 2.997716f, 0.01f);
	EXPECT_NEAR(m[2][3], 3.0f, 0.01f);
	EXPECT_NEAR(m[3][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][2], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][3], 1.0f, 0.01f);
}

TEST(ViewTest, ViewTran)
{
	Camera camera(1024, 768, glm::vec3(-1.0f, -2.0f, -3.0f), glm::vec3(1.0f, 2.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Pipeline p;
/*
0.980581 0.000000 -0.196116 0.392232
-0.071611 0.930949 -0.358057 0.716115
0.182574 0.365148 0.912871 3.651484
0.000000 0.000000 0.000000 1.000000
*/
	p.SetCamera(camera);
	glm::mat4x4 m = p.GetViewTrans();

	PrintGLMMat4x4(m);

	EXPECT_NEAR(m[0][0], 0.980581f, 0.01f);
	EXPECT_NEAR(m[0][1], 0.000000f, 0.01f);
	EXPECT_NEAR(m[0][2], -0.196116f, 0.01f);
	EXPECT_NEAR(m[0][3], 0.392232f, 0.01f);
	EXPECT_NEAR(m[1][0], -0.071611f, 0.01f);
	EXPECT_NEAR(m[1][1], 0.930949f, 0.01f);
	EXPECT_NEAR(m[1][2], -0.358057f, 0.01f);
	EXPECT_NEAR(m[1][3], 0.716115f, 0.01f);
	EXPECT_NEAR(m[2][0], 0.182574f, 0.01f);
	EXPECT_NEAR(m[2][1], 0.365148f, 0.01f);
	EXPECT_NEAR(m[2][2], 0.912871f, 0.01f);
	EXPECT_NEAR(m[2][3], 3.651484f, 0.01f);
	EXPECT_NEAR(m[3][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][2], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][3], 1.0f, 0.01f);
}

TEST(ProjTest, Projection)
{
	PersParam p;
	p.FOV = 45.0f;
	p.Height = 768;
	p.Width = 1024;
	p.zNear = 0.1f;
	p.zFar = 100.0f;
	
	Pipeline p1;
	p1.SetPerspectiveProj(p);
	glm::mat4x4 m = p1.GetProjTrans();
/*
3.218951 0.000000 0.000000 0.000000
0.000000 2.414213 0.000000 0.000000
0.000000 0.000000 1.002002 -0.200200
0.000000 0.000000 1.000000 0.000000
*/
	// PrintGLMMat4x4(m);

	EXPECT_NEAR(m[0][0], 3.218951f, 0.01f);
	EXPECT_NEAR(m[0][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[0][2], 0.0f, 0.01f);
	EXPECT_NEAR(m[0][3], 0.0f, 0.01f);
	EXPECT_NEAR(m[1][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[1][1], 2.414213f, 0.01f);
	EXPECT_NEAR(m[1][2], 0.0f, 0.01f);
	EXPECT_NEAR(m[1][3], 0.0f, 0.01f);
	EXPECT_NEAR(m[2][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[2][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[2][2], 1.002002f, 0.01f);
	EXPECT_NEAR(m[2][3], -0.200200f, 0.01f);
	EXPECT_NEAR(m[3][0], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][1], 0.0f, 0.01f);
	EXPECT_NEAR(m[3][2], 1.0f, 0.01f);
	EXPECT_NEAR(m[3][3], 0.0f, 0.01f);
}

TEST(CameraTest, Mouse)
{
	Camera camera(1024, 768);
	camera.OnMouse(500, 388);
	Pipeline p;
	p.SetCamera(camera);
	glm::mat4x4 m = p.GetViewTrans();
/*
0.999945 -0.000000 0.010472 0.000000
-0.000037 0.999994 0.003490 0.000000
-0.010472 -0.003491 0.999939 0.000000
0.000000 0.000000 0.000000 1.000000
*/
	// PrintGLMMat4x4(m);

	EXPECT_NEAR(m[0][0], 0.999945f, 0.01f);
	EXPECT_NEAR(m[0][1], -0.000000f, 0.01f);
	EXPECT_NEAR(m[0][2], 0.010472f, 0.01f);
	EXPECT_NEAR(m[0][3], 0.000000f, 0.01f);
	EXPECT_NEAR(m[1][0], -0.000037f, 0.01f);
	EXPECT_NEAR(m[1][1], 0.999994f, 0.01f);
	EXPECT_NEAR(m[1][2], 0.003490f, 0.01f);
	EXPECT_NEAR(m[1][3], 0.000000f, 0.01f);
	EXPECT_NEAR(m[2][0], -0.010472f, 0.01f);
	EXPECT_NEAR(m[2][1], -0.003491f, 0.01f);
	EXPECT_NEAR(m[2][2], 0.999939f, 0.01f);
	EXPECT_NEAR(m[2][3], 0.000000f, 0.01f);
	EXPECT_NEAR(m[3][0], 0.000000f, 0.01f);
	EXPECT_NEAR(m[3][1], 0.000000f, 0.01f);
	EXPECT_NEAR(m[3][2], 0.000000f, 0.01f);
	EXPECT_NEAR(m[3][3], 1.000000f, 0.01f);

	camera.OnMouse(200, 78);
	p.SetCamera(camera);
	m = p.GetViewTrans();

/*
0.963162 0.000000 0.268920 0.000000
0.070961 0.964557 -0.254153 0.000000
-0.259389 0.263873 0.929026 0.000000
0.000000 0.000000 0.000000 1.000000
*/
	// PrintGLMMat4x4(m);

	EXPECT_NEAR(m[0][0], 0.963162f, 0.01f);
	EXPECT_NEAR(m[0][1], 0.000000f, 0.01f);
	EXPECT_NEAR(m[0][2], 0.268920f, 0.01f);
	EXPECT_NEAR(m[0][3], 0.000000f, 0.01f);
	EXPECT_NEAR(m[1][0], 0.070961f, 0.01f);
	EXPECT_NEAR(m[1][1], 0.964557f, 0.01f);
	EXPECT_NEAR(m[1][2], -0.254153f, 0.01f);
	EXPECT_NEAR(m[1][3], 0.000000f, 0.01f);
	EXPECT_NEAR(m[2][0], -0.259389f, 0.01f);
	EXPECT_NEAR(m[2][1], 0.263873f, 0.01f);
	EXPECT_NEAR(m[2][2], 0.929026f, 0.01f);
	EXPECT_NEAR(m[2][3], 0.000000f, 0.01f);
	EXPECT_NEAR(m[3][0], 0.000000f, 0.01f);
	EXPECT_NEAR(m[3][1], 0.000000f, 0.01f);
	EXPECT_NEAR(m[3][2], 0.000000f, 0.01f);
	EXPECT_NEAR(m[3][3], 1.000000f, 0.01f);
}

TEST(MathTest, QRotate) 
{
	glm::vec3 v(1.0f, 2.0f, 3.0f);
	glm::vec3 v1 = RotateVectorByQuaternion(v, 45.0f, glm::vec3(0.0f, 1.0f, 1.0f));
// (1.267767, 3.292893, 2.439339)
	// PrintGLMVec3(v1);

	EXPECT_NEAR(v1.x, 1.267767f, 0.01f);
	EXPECT_NEAR(v1.y, 3.292893f, 0.01f);
	EXPECT_NEAR(v1.z, 2.439339f, 0.01f);
	
}