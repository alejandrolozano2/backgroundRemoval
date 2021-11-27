#include "imageUtilities.h"
#include "mst_image.h"
#include "sorting.h"
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <chrono>

#define N_FRAMES 10
#define SEARCH_RANGE 70
using namespace cv;
using namespace std;
using namespace std::chrono;

struct RGB_u32 {
   uint32_t blue;
   uint32_t green;
   uint32_t red;
 };

int main(int argc, char * argv[]) {

        VideoCapture cam(0);
        cv::Mat colorMat, color;
        cv::Size size;
        size.height = 480;
        size.width = 640;
        cv::Mat grayMat(size, CV_8U);
        cv::Mat templateImage , outMatch;
        cv::Mat blurMat(size, CV_8U);
        cv::Mat sobelMat(size, CV_8U);
        cv::Mat sobelAngle(size, CV_64F);
        cv::Mat nThreshold(size, CV_8U);
        cv::Mat out(size, CV_8U);

        if (!cam.isOpened()) return -1;
        /*Save JPG of first capture*/
        cam >> colorMat;

        cv::Size s = colorMat.size();
        uint32_t imageSize = s.height * s.width;
        uint32_t threshold = 7;

        double minVal; 
        double maxVal; 
        Point minLoc; 
        Point maxLoc;

        uint8_t i = 0;
        templateImage  = imread("ojos.jpg", 0);
        imshow("template", templateImage);
        for(;;) {
                        
                auto start = high_resolution_clock::now();
                cam >> colorMat;
                color = colorMat.clone();
#if 1
                bgr2grey(colorMat, grayMat);
                Gaussian3_3(grayMat, blurMat);
                Sobel(blurMat, sobelMat, sobelAngle);
                mthreshold(sobelMat, nThreshold, 19, 20, (uint8_t)maxVal);
                matchTemplate(grayMat, templateImage, outMatch, TM_CCOEFF_NORMED);

		double minVal; double maxVal; 
		cv::Point minLoc, maxLoc, matchLoc;
		cv::minMaxLoc(outMatch, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
		matchLoc = maxLoc;
#if 0
		cv::rectangle(
			colorMat,
			matchLoc,
			cv::Point(matchLoc.x + templateImage.cols , matchLoc.y + templateImage.rows),
			CV_RGB(255,0,0),
			3);
#endif                             

                uint8_t * pEdges = nThreshold.ptr<uint8_t>(0);
                int32_t search = 0;
                pEdges += (matchLoc.y + templateImage.rows) * s.width + matchLoc.x;
                while (search < SEARCH_RANGE) {
                        search++; 
                        DFS(nThreshold, out,
                         (matchLoc.y ) * s.width + matchLoc.x - search,
                          s.width * s.height - 1);
                }

                fillOutside(colorMat, out);              
#endif
                imshow("Original", color);
                imshow("Color", colorMat);
                imshow("Edges", sobelMat);
                imshow("Threshold", nThreshold);
                imshow("DFS", out);
                out = 0;

                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(stop - start);
                cout << duration.count() << " us"  << endl;    
                
                char c=(char)waitKey(25);
                if(c==27)
                        break;

        }

        return 1;
}
