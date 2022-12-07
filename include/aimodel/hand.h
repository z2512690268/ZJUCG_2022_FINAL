#ifndef _HAND_H
#define _HAND_H

#include <opencv2/opencv.hpp>
#include <vector>

class HandDetector
{
public:
    HandDetector() {}
    int Handpose_Recognition(cv::Mat src);
private:
    bool HandKeypoints_Detect(cv::Mat src, std::vector<cv::Point>&HandKeypoints);
    int Handpose_Internal(std::vector<cv::Point>&HandKeypoints);
};
#endif