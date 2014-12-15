// File: main.cpp
//*****************************************************************************************
// This file contains an edge detection algorithm using Sobel, the original author is
// Bibek Subedi and his code can be found at:
// http://www.programming-techniques.com/2013/03/sobel-and-prewitt-edge-detector-in-c.html
//
// The purpose of this program is simply to parallelize the original algorithm.
//*****************************************************************************************

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <sys/time.h>

#define USE_COMMANDLINE 0

// Compute x component of the gradient vector at a given point in an image
int xGradient(cv::Mat image, int x, int y)
{
	return image.at<uchar>(y - 1, x - 1) +
			2 * image.at<uchar>(y, x + 1) +
				image.at<uchar>(y + 1, x - 1) -
				image.at<uchar>(y - 1, x + 1) -
			2 * image.at<uchar>(y, x + 1) -
				image.at<uchar>(y + 1, x + 1);
}

// Compute y component of the gradient vector at a given point in an image
int yGradient(cv::Mat image, int x, int y)
{
	return image.at<uchar>(y - 1, x - 1) +
			2 * image.at<uchar>(y - 1, x) +
				image.at<uchar>(y - 1, x + 1) -
				image.at<uchar>(y + 1, x - 1) -
			2 * image.at<uchar>(y + 1, x) -
				image.at<uchar>(y + 1, x + 1);
}

int main(int argc, char* argv[])
{
	// Input file set to some default test image
	// The test image can be fetched from Wikipedia at:
	// http://commons.wikimedia.org/wiki/File:Valve_original_(1).PNG
	std::string inputFile = "Valve_original_(1).PNG";

	// Timer
	timeval t1, t2;
	double elapsedTime;

	// Use command line argument
	if (USE_COMMANDLINE)
	{
		// Use did not specify image path and name
		if (argc < 2)
		{
			std::cout << "Usage: ./LACPP_Project IMAGE_PATH\n";

			return 1;
		}

		inputFile = std::string(argv[1]);
	}

	// Do not use command line argument
	else
	{}

	// Load image
	cv::Mat img_src, img_dest;
	img_src = cv::imread(inputFile, CV_LOAD_IMAGE_GRAYSCALE);
	img_dest = img_src.clone(); // Clone the source image into our destination image
								// so that the dimensions are identical

	// Img has failed to load
	if (!img_src.data)
	{
		std::cout << "Could not load image " << inputFile << "\n";
		return 1;
	}

	// Clear the data in our destination image - we don't want the source image data
	for (int y = 0; y < img_src.rows; ++y)
	{
		for (int x = 0; x < img_src.cols; ++x)
		{
			img_dest.at<uchar>(y, x) = 0.0;
		}
	}

	gettimeofday(&t1, NULL);

	// Now process our destination image with edge detection using the Sobel operator
	// TODO: Parallelize using threads and locks
	// TODO: Parallelize using tasks
	int gx, gy, sum;
	gx = gy = sum = 0;
	for (int y = 1; y < img_src.rows - 1; ++y)
	{
		for (int x = 1; x < img_src.cols - 1; x++)
		{
			gx = xGradient(img_src, x, y);
			gy = yGradient(img_src, x, y);
			sum = abs(gx) + abs(gy);
			sum = sum > 255 ? 255:sum;
			sum = sum < 0 ? 0 : sum;
			img_dest.at<uchar>(y, x) = sum;
		}
	}

	gettimeofday(&t2, NULL);

	// Elapsed time in ms
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;		// (Total s -> ms)
	elapsedTime += (t2.tv_usec - t1.tv_usec) * 0.001;	// (+ Total us -> ms)
	std::cout << "Elapsed: " << elapsedTime << "ms\n";

	cv::namedWindow("Original");
	cv::imshow("Original", img_src);

	cv::namedWindow("Result");
	cv::imshow("Result", img_dest);

	cv::waitKey(0);
	return 0;
}
