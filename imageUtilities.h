#ifndef IMAGE_UTILITIES_H
#define IMAGE_UTILITIES_H

#include<opencv2/core/core.hpp>
#include<opencv2/ml/ml.hpp>
#include<opencv/cv.h>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<cstdlib>

#define diff(x,y)   (x-y)
#define sqr(d) (d*d)
#define bgrDiff(x, y)  (abs(diff((x)->red, (y)->red)) + abs(diff((x)->green, (y)->green)) + abs(diff((x)->blue, (y)->blue)))


struct RGB {
   uchar blue;
   uchar green;
   uchar red;
 };

/*Onw implementation for gray conversion*/
int bgr2grey (cv::Mat & src, cv::Mat & dst);
int Gaussian3_3 (cv::Mat & src, cv::Mat & dst);
int Sobel (cv::Mat & src, cv::Mat & dst);

uint32_t buildEdges(cv::Mat & image, uint32_t (* e)[3], uint32_t radius);

#endif

