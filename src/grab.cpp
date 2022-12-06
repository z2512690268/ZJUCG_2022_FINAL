#include "grab.h"

ScreenGraber::ScreenGraber()
{
	colorArr = NULL;
}

ScreenGraber::~ScreenGraber()
{
	if (colorArr != NULL){
		delete[] colorArr;
		colorArr = NULL;
	}
}

void ScreenGraber::Grab() {
	glGetIntegerv(GL_VIEWPORT, viewPort);
	if (colorArr != NULL){
		delete[] colorArr; colorArr = NULL;
	}
	win.width = viewPort[2];
	win.height = viewPort[3];
	colorArr = new GLbyte[4 * win.area()];

	glReadPixels(viewPort[0], viewPort[1], viewPort[2], viewPort[3],
		GL_RGBA, GL_UNSIGNED_BYTE, colorArr);
}

void ScreenGraber::saveColorImg(const std::string & str){
	cv::Mat img;
	std::vector<cv::Mat> imgPlanes;
	img.create(win.height, win.width, CV_8UC3);
	cv::split(img, imgPlanes);

	for (int i = 0; i < win.height; i++) {
		UCHAR* plane0Ptr = imgPlanes[0].ptr<UCHAR>(i);
		UCHAR* plane1Ptr = imgPlanes[1].ptr<UCHAR>(i);
		UCHAR* plane2Ptr = imgPlanes[2].ptr<UCHAR>(i);
		for (int j = 0; j < win.width; j++) {
			int k = 4 * (i * win.width + j); // RGBA   
			plane2Ptr[j] = colorArr[k];
			plane1Ptr[j] = colorArr[k + 1];
			plane0Ptr[j] = colorArr[k + 2];
		}
	}

	cv::merge(imgPlanes, img);
	cv::flip(img, img, 0);
	if (str.length() == 0){
		return ;
	}
	cv::imwrite(str.c_str(), img);
}