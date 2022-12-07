#ifndef _GRAB_H
#define _GRAB_H

#include <opencv2\opencv.hpp>
#include <GL/freeglut.h>

class ScreenGraber
{
public:
	ScreenGraber();
	~ScreenGraber();
	void GrabScreen();
	void saveColorImg(const std::string& _str);
private:
	GLbyte* colorArr;
	GLint	viewPort[4];
	cv::Size win;
};

class RealityGrabber
{
public:
	RealityGrabber();
	~RealityGrabber();
	bool OpenReality();
	bool CloseReality();
	void GrabReality();
	void saveRealityImg(const std::string& _str);
	cv::Mat getRealityImg();
private:
	cv::Mat realityImg;
	cv::VideoCapture *m_pcap;
};

#endif