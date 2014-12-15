/*
 * EdgeDetection.h
 *
 *  Created on: Dec 15, 2014
 *      Author: hyzor
 */

#ifndef EDGEDETECTION_H_
#define EDGEDETECTION_H_

#include <iostream>
#include <thread>
#include <mutex>

#include <opencv2/imgproc/imgproc.hpp>

#include "ImageProcessingUtil.h"

class EdgeDetection
{
public:
	enum ParallelMethod
	{
		NONE = 0,
		THREADS_AND_LOCKS,
		TASKS
	};

	EdgeDetection();
	virtual ~EdgeDetection();

	cv::Mat ProcessImg(const cv::Mat* img, unsigned int parallelMethod, unsigned int num_threads);

private:
	void ThreadFunc(const cv::Mat* img_src, cv::Mat* img_dest, unsigned int rowStart, unsigned int rowEnd, std::mutex& mtx);
};

#endif /* EDGEDETECTION_H_ */
