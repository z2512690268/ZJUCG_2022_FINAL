#ifndef _GRAB_H
#define _GRAB_H

#include <opencv2\opencv.hpp>
#include <GL/freeglut.h>

class ScreenGraber
{
public:
	ScreenGraber();
	~ScreenGraber();
	void Grab();
	void saveColorImg(const std::string& _str);
private:
	GLbyte* colorArr;
	GLint	viewPort[4];
	cv::Size win;
};

#endif