// File: ImageProcessingUtil.h
// Contains code for image processing algorithms, such as the Sobel operator

#ifndef IMAGEPROCESSINGUTIL_H_
#define IMAGEPROCESSINGUTIL_H_

#include <opencv2/imgproc/imgproc.hpp>

class ImageProcessingUtil
{
public:
	/*
	static int GetGradientX(const cv::Mat img, int x, int y);
	static int GetGradientY(const cv::Mat img, int x, int y);
	static int GetSobelOperator(const cv::Mat img, int x, int y);
	*/

	static int GetGradientX(const cv::Mat* img, int x, int y);
	static int GetGradientY(const cv::Mat* img, int x, int y);
	static int GetSobelOperator(const cv::Mat* img, int x, int y);
};

#endif
