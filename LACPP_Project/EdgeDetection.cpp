/*
 * EdgeDetection.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: hyzor
 */

#include "EdgeDetection.h"

EdgeDetection::EdgeDetection()
{
	// TODO Auto-generated constructor stub

}

EdgeDetection::~EdgeDetection()
{
	// TODO Auto-generated destructor stub
}

cv::Mat EdgeDetection::ProcessImg(const cv::Mat* img, unsigned int parallelMethod, unsigned int num_threads)
{
	if (!img->data)
	{
		std::cout << __FUNCTION__ << ": Could not load image " << "\n";
		return cv::Mat::zeros(0, 0, 0);
	}

	const cv::Mat* img_src = img;
	cv::Mat img_dest = img->clone();

	// Clear the data in our destination image - we don't want the source image data
	for (int y = 0; y < img_src->rows; ++y)
	{
		for (int x = 0; x < img_src->cols; ++x)
		{
			img_dest.at<uchar>(y, x) = 0.0;
		}
	}

	//-------------------------------------------
	// Process image using no parallelism
	//-------------------------------------------
	if (parallelMethod == ParallelMethod::NONE)
	{
		std::cout << "Running sequential version...\n";

		// Now process our destination image with edge detection using the Sobel operator
		for (int y = 1; y < img_src->rows - 1; ++y)
		{
			for (int x = 1; x < img_src->cols - 1; ++x)
			{
				img_dest.at<uchar>(y, x) = ImageProcessingUtil::GetSobelOperator(img_src, x, y);
			}
		}
		std::cout << "...Done!\n";
	}

	//-------------------------------------------
	// Process image using threads and locks
	//-------------------------------------------
	else if (parallelMethod == ParallelMethod::THREADS_AND_LOCKS)
	{
		std::cout << "Running threads and locks version...\n";
		std::mutex mtx;

		// Number of rows are less than the amount of threads requested,
		// spawn threads equal to the number of rows
		if ((unsigned int)img_src->rows < num_threads)
		{

		}

		// Spawn the maximum number of threads
		else
		{
			// Calculate the number of rows per thread, also calculate
			// if there is an uneven number of rows per thread
			unsigned int rowsPerThread = (unsigned int)img_src->rows / num_threads;
			unsigned int rows_rest = (unsigned int)img_src->rows % num_threads;

			std::cout << "Rows/thread: " << rowsPerThread << "\n";
			std::cout << "Rest rows: " << rows_rest << "\n";

			std::thread myThreads[num_threads];

			if (rows_rest == 0)
			{
				std::cout << "There are an equal amount of rows per threads!\n";

				for (unsigned int i = 0; i < num_threads; ++i)
				{
					unsigned int rowStart = (i * rowsPerThread) + 1;
					myThreads[i] = std::thread(&EdgeDetection::ThreadFunc, this, img_src,
										&img_dest, rowStart, (rowStart + rowsPerThread),
										std::ref(mtx));
				}

				for (unsigned int i = 0; i < num_threads; ++i)
				{
					myThreads[i].join();
				}
			}
		}

		std::cout << "...Done!\n";
	}

	return img_dest;
}

void EdgeDetection::ThreadFunc(const cv::Mat* img_src,
							cv::Mat* img_dest,
							unsigned int rowStart,
							unsigned int rowEnd,
							std::mutex& mtx)
{
	unsigned int numRows = rowEnd - rowStart;
	int pixelData[numRows][img_src->cols];

	/*
	// Write directly to the destination image data
	for (unsigned int y = rowStart; y < rowEnd; ++y)
	{
		for (int x = 1; x < img_src->cols - 1; ++x)
		{
			img_dest->at<uchar>(y, x) = ImageProcessingUtil::GetSobelOperator(img_src, x, y);
		}
	}
	*/

	// Cache local pixel data for this thread
	for (unsigned int y = 0; y < numRows; ++y)
	{
		for (int x = 1; x < img_src->cols - 1; ++x)
		{
			pixelData[y][x] = ImageProcessingUtil::GetSobelOperator(img_src, x, (y + rowStart));
		}
	}

	bool hasFinished = false;

	// Now try to lock the shared data (our destination image data)
	// and write our local cache to it
	while (!hasFinished)
	{
		if (mtx.try_lock())
		{
			for (unsigned int y = 0; y < numRows; ++y)
			{
				for (int x = 1; x < img_src->cols - 1; ++x)
				{
					img_dest->at<uchar>((y + rowStart), x) = pixelData[y][x];
				}
			}

			hasFinished = true;

			mtx.unlock();
		}
	}
}
