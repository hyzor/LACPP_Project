// File: main.cpp

#include <iostream>
#include <thread>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <sys/time.h>

#include "EdgeDetection.h"
#include "ImageProcessingUtil.h"

#define USE_COMMANDLINE 0

void test()
{

}

int main(int argc, char* argv[])
{
	// Input file set to some default test image
	// The test image can be fetched from Wikipedia at:
	// http://commons.wikimedia.org/wiki/File:Valve_original_(1).PNG
	std::string inputFile = "Valve_original_(1).PNG";

	// Timer
	timeval t1_seq, t2_seq;
	timeval t1_threads, t2_threads;
	timeval t1_tasks, t2_tasks;
	double elapsedTime;

	EdgeDetection edgeDetection;

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
	cv::Mat* img_src;
	cv::Mat img_dest_seq, img_dest_threads, img_dest_tasks;
	img_src = new cv::Mat(cv::imread(inputFile, CV_LOAD_IMAGE_GRAYSCALE));
	//img_src = cv::imread(inputFile, CV_LOAD_IMAGE_GRAYSCALE);
	//img_dest = img_src->clone(); // Clone the source image into our destination image
								// so that the dimensions are identical

	// Failed to allocate memory for image
	if (!img_src)
	{
		std::cout << "Failed to allocate memory for image " << inputFile << "\n";
		return 1;
	}

	// Img has failed to load
	if (!img_src->data)
	{
		std::cout << "Could not load image " << inputFile << "\n";
		return 1;
	}

	std::thread testThread(test);

	// Run and measure sequential version
	gettimeofday(&t1_seq, NULL);
	img_dest_seq = edgeDetection.ProcessImg(img_src, EdgeDetection::NONE, 1);
	gettimeofday(&t2_seq, NULL);

	// Run and measure threads and locks version
	gettimeofday(&t1_threads, NULL);
	img_dest_threads = edgeDetection.ProcessImg(img_src, EdgeDetection::THREADS_AND_LOCKS, 10);
	gettimeofday(&t2_threads, NULL);

	// Run and measure tasks version
	gettimeofday(&t1_tasks, NULL);
	img_dest_tasks = edgeDetection.ProcessImg(img_src, EdgeDetection::TASKS, 10);
	gettimeofday(&t2_tasks, NULL);

	std::cout << "------------------\nElapsed\n------------------\n";

	// Elapsed time in ms
	elapsedTime = (t2_seq.tv_sec - t1_seq.tv_sec) * 1000.0;		// (Total s -> ms)
	elapsedTime += (t2_seq.tv_usec - t1_seq.tv_usec) * 0.001;	// (+ Total us -> ms)
	std::cout << "Sequential: " << elapsedTime << "ms\n";

	elapsedTime = (t2_threads.tv_sec - t1_threads.tv_sec) * 1000.0;		// (Total s -> ms)
	elapsedTime += (t2_threads.tv_usec - t1_threads.tv_usec) * 0.001;	// (+ Total us -> ms)
	std::cout << "Threads and locks: " << elapsedTime << "ms\n";

	elapsedTime = (t2_tasks.tv_sec - t1_tasks.tv_sec) * 1000.0;		// (Total s -> ms)
	elapsedTime += (t2_tasks.tv_usec - t1_tasks.tv_usec) * 0.001;	// (+ Total us -> ms)
	std::cout << "Tasks: " << elapsedTime << "ms\n";

	cv::namedWindow("Original");
	cv::imshow("Original", *img_src);

	cv::namedWindow("Result (sequential)");
	cv::imshow("Result (sequential)", img_dest_seq);

	cv::namedWindow("Result (threads and locks)");
	cv::imshow("Result (threads and locks)", img_dest_threads);

	cv::namedWindow("Result (tasks)");
	cv::imshow("Result (tasks)", img_dest_tasks);

	cv::waitKey(0);

	if (img_src)
	{
		delete img_src;
		img_src = nullptr;
	}

	return 0;
}
