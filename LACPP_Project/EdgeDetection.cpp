/*
 * EdgeDetection.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: hyzor
 */

#include "EdgeDetection.h"

EdgeDetection::EdgeDetection()
{}

EdgeDetection::~EdgeDetection()
{}

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
		//std::cout << "Running sequential version...\n";

		// Now process our destination image with edge detection using the Sobel operator
		for (int y = 1; y < img_src->rows - 1; ++y)
		{
			for (int x = 1; x < img_src->cols - 1; ++x)
			{
				img_dest.at<uchar>(y, x) = ImageProcessingUtil::GetSobelOperator(img_src, x, y);
			}
		}
		//std::cout << "...Done!\n";
	}

	//-------------------------------------------
	// Process image using threads and locks
	//-------------------------------------------
	else if (parallelMethod == ParallelMethod::THREADS_AND_LOCKS)
	{
		//std::cout << "Running threads and locks version...\n";
		std::mutex mtx;

		// Number of rows are less than the amount of threads requested,
		// spawn threads equal to the number of rows
		if ((unsigned int)img_src->rows < num_threads)
		{
			num_threads = (unsigned int)img_src->rows;
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

			// There is an equal amount of rows per thread

			//std::cout << "There is an equal amount of rows per threads!\n";

			for (unsigned int i = 0; i < num_threads; ++i)
			{
				unsigned int curRowsPerThread = rowsPerThread
								+ (i < rows_rest ? 1 : 0);	// Add rest row, if any
				unsigned int rowStart = (i * curRowsPerThread) + 1;
				myThreads[i] = std::thread(&EdgeDetection::ThreadFunc, this, img_src,
									&img_dest, rowStart, (rowStart + curRowsPerThread),
									std::ref(mtx));
			}

			for (unsigned int i = 0; i < num_threads; ++i)
			{
				myThreads[i].join();
			}
		}

		//std::cout << "...Done!\n";
	}

	//-------------------------------------------
	// Process image using tasks
	//-------------------------------------------
	else if (parallelMethod == ParallelMethod::TASKS)
	{
		//std::cout << "Running tasks version...\n";

		unsigned int tasksPerThread = 2;

		// Number of rows are less than the amount of threads requested,
		// spawn threads equal to the number of rows
		if ((unsigned int)img_src->rows < num_threads)
		{
			num_threads = (unsigned int)img_src->rows;
			tasksPerThread = 1;
		}

		ThreadPool* pool = new ThreadPool(num_threads);
		std::vector<std::future<unsigned int>> results;

		unsigned int numTasks = num_threads * tasksPerThread;

		unsigned int rowsPerTask = (unsigned int)img_src->rows / numTasks;
		unsigned int rows_rest = (unsigned int)img_src->rows % numTasks;

		std::cout << "Tasks: " << numTasks << "\n" <<
				"Rows/task: " << rowsPerTask << "\n";
		std::cout << "Rows rest: " << rows_rest << "\n";

		Task(img_src, 0, (0 + 0), 0, &img_dest);

		// Add all the tasks, and store their results
		for (unsigned int i = 0; i < numTasks; ++i)
		{
			unsigned int curRowsPerTask = rowsPerTask
							+ (i < rows_rest ? 1 : 0);	// Add rest row, if any

			unsigned int rowStart = (i * curRowsPerTask) + 1;

			results.emplace_back(
					// Wrap task function into the lamba function
		            pool->AddTask([](EdgeDetection* edgeObj, const cv::Mat* img_src, unsigned int rowStart,
		            		unsigned int rowEnd, unsigned int id, cv::Mat* img_dest)
					{
						return edgeObj->Task(img_src, rowStart, rowEnd, id, img_dest);
		            }, this, img_src, rowStart, (rowStart + curRowsPerTask), i, &img_dest)
					);
		}

		// Collect results when all tasks have finished
		//for (unsigned int i = 0; i < results.size(); ++i)
		//{
			//std::cout << results[i].get() << "\n";
		//}

		// Shutdown thread pool
		pool->Shutdown();
		delete pool;

		//std::cout << "...Done!\n";
	}

	return img_dest;
}

unsigned int EdgeDetection::Task(const cv::Mat* img_src, unsigned int rowStart,
		unsigned int rowEnd, unsigned int id, cv::Mat* img_dest)
{
	//unsigned int numRows = rowEnd - rowStart;
	//std::map<int, int**> map;

	// Cache local pixel data for this task
	for (unsigned int y = rowStart; y < rowEnd; ++y)
	{
		for (int x = 1; x < img_src->cols - 1; ++x)
		{
			//pixelData[y][x] = ImageProcessingUtil::GetSobelOperator(img_src, x, (y + rowStart));
			img_dest->at<uchar>(y, x) = ImageProcessingUtil::GetSobelOperator(img_src, x, y);
		}
	}

	//map[id] = pixelData;

	return id;
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
