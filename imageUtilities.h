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
#define bgrDiff2(x, y)  ((diff((x)->red, (y)->red)) + (diff((x)->green, (y)->green)) + (diff((x)->blue, (y)->blue)))


struct RGB {
   uchar blue;
   uchar green;
   uchar red;
 };

/*Onw implementation for gray conversion*/
int bgr2grey (cv::Mat & src, cv::Mat & dst);
int Gaussian3_3 (cv::Mat & src, cv::Mat & dst);
int Sobel (cv::Mat & src, cv::Mat & dst, cv::Mat  & angles);
int nonMaxSuppresion (cv::Mat & G, cv::Mat & T, cv::Mat  & dst);
int mthreshold (cv::Mat & src, cv::Mat & dst, float lowRatio, float highRatio, uint8_t max);
int mhysteresis(cv::Mat & src, cv::Mat & dst);
bool DFS(cv::Mat & image, cv::Mat &  out, uint32_t start, uint32_t end, uint32_t * lM, uint32_t * rM, uint32_t * tM);
uint32_t correlation(cv::Mat & src, cv::Mat & shape, uint32_t * row, uint32_t * col);
uint32_t buildEdges(cv::Mat & image, uint32_t (* e)[3], uint32_t in, uint32_t out , uint32_t step);
bool isPixelInside(uint32_t row, uint32_t col, uint32_t * lM, uint32_t * rM, uint32_t * tM);
void fillOutside(cv::Mat & colorOut, cv::Mat edges);

#endif

