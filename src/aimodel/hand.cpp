#include "hand.h"
#include<opencv2/dnn.hpp>

//手部关键点数目
const int nPoints = 21; 
//手指索引, 分别为：大拇指、食指、中指、无名指、小拇指
const int tipIds[] = { 4,8,12,16,20 };

bool HandDetector::HandKeypoints_Detect(cv::Mat src, std::vector<cv::Point>&HandKeypoints)
{
	//模型尺寸大小
	int width = src.cols;
	int height = src.rows;
	float ratio = width / (float)height;
	int modelHeight = 368;  //由模型输入维度决定
	int modelWidth = int(ratio*modelHeight);

	//模型文件
	std::string model_file = "aimodel/pose_deploy.prototxt";  //网络模型
	std::string model_weight = "aimodel/pose_iter_102000.caffemodel";//网络训练权重

	//加载caffe模型
	cv::dnn::Net net = cv::dnn::readNetFromCaffe(model_file, model_weight);

	//将输入图像转成blob形式
	cv::Mat blob = cv::dnn::blobFromImage(src, 1.0 / 255, cv::Size(modelWidth, modelHeight), cv::Scalar(0, 0, 0));

	//将图像转换的blob数据输入到网络的第一层“image”层，见deploy.protxt文件
	net.setInput(blob, "image");

	//结果输出
	cv::Mat output = net.forward();
	int H = output.size[2];
	int W = output.size[3];

	for (int i = 0; i < nPoints; i++)
	{
		//结果预测
		cv::Mat probMap(H, W, CV_32F, output.ptr(0, i)); 

		resize(probMap, probMap, cv::Size(width, height));

		cv::Point keypoint; //最大可能性手部关键点位置
		double classProb;  //最大可能性概率值
		minMaxLoc(probMap, NULL, &classProb, NULL, &keypoint);

		HandKeypoints[i] = keypoint; //结果输出，即手部关键点所在坐标
	}

	return true;
}
int HandDetector::Handpose_Recognition(cv::Mat src){
    std::vector<cv::Point> HandKeypoints;
    HandKeypoints.resize(nPoints);
    int ret = -1;
    HandKeypoints_Detect(src, HandKeypoints);
    ret = Handpose_Internal(HandKeypoints);
    printf("Handpose ret:%d\n", ret);
    return ret;
}

int HandDetector::Handpose_Internal(std::vector<cv::Point> &HandKeypoints){
	int fingers = 0;
	//拇指
    //如果关键点'4'的x坐标大于关键点'3'的x坐标，则说明大拇指是张开的。计数1
    fingers += HandKeypoints[tipIds[0]].x > HandKeypoints[tipIds[0] - 1].x ? 1 : 0;
	//其余的4个手指
	for (int i = 1; i < 5; i++)
	{
        //例：如果关键点'8'的y坐标小于关键点'6'的y坐标，则说明食指是张开的。计数1
        fingers += HandKeypoints[tipIds[i]].y < HandKeypoints[tipIds[i] - 2].y ? 1: 0;
    }

	return fingers;
}