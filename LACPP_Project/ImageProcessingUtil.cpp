#include "ImageProcessingUtil.h"

/*
// Compute x component of the gradient vector at a given point in an image
int ImageProcessingUtil::GetGradientX(const cv::Mat image, int x, int y)
{
	return image.at<uchar>(y - 1, x - 1) +
			2 * image.at<uchar>(y, x + 1) +
				image.at<uchar>(y + 1, x - 1) -
				image.at<uchar>(y - 1, x + 1) -
			2 * image.at<uchar>(y, x + 1) -
				image.at<uchar>(y + 1, x + 1);
}

// Compute y component of the gradient vector at a given point in an image
int ImageProcessingUtil::GetGradientY(const cv::Mat image, int x, int y)
{
	return image.at<uchar>(y - 1, x - 1) +
			2 * image.at<uchar>(y - 1, x) +
				image.at<uchar>(y - 1, x + 1) -
				image.at<uchar>(y + 1, x - 1) -
			2 * image.at<uchar>(y + 1, x) -
				image.at<uchar>(y + 1, x + 1);
}

int ImageProcessingUtil::GetSobelOperator(const cv::Mat img, int x, int y)
{
	int gx = GetGradientX(img, x, y);
	int gy = GetGradientY(img, x, y);
	int sum = abs(gx) + abs(gy);
	sum = sum > 255 ? 255 : sum;
	sum = sum < 0 ? 0 : sum;

	return sum;
}
*/

// Compute x component of the gradient vector at a given point in an image
int ImageProcessingUtil::GetGradientX(const cv::Mat* image, int x, int y)
{
	return image->at<uchar>(y - 1, x - 1) +
			2 * image->at<uchar>(y, x + 1) +
				image->at<uchar>(y + 1, x - 1) -
				image->at<uchar>(y - 1, x + 1) -
			2 * image->at<uchar>(y, x + 1) -
				image->at<uchar>(y + 1, x + 1);
}

// Compute y component of the gradient vector at a given point in an image
int ImageProcessingUtil::GetGradientY(const cv::Mat* image, int x, int y)
{
	return image->at<uchar>(y - 1, x - 1) +
			2 * image->at<uchar>(y - 1, x) +
				image->at<uchar>(y - 1, x + 1) -
				image->at<uchar>(y + 1, x - 1) -
			2 * image->at<uchar>(y + 1, x) -
				image->at<uchar>(y + 1, x + 1);
}

int ImageProcessingUtil::GetSobelOperator(const cv::Mat* img, int x, int y)
{
	int gx = GetGradientX(img, x, y);
	int gy = GetGradientY(img, x, y);
	int sum = abs(gx) + abs(gy);
	sum = sum > 255 ? 255 : sum;
	sum = sum < 0 ? 0 : sum;

	return sum;
}
