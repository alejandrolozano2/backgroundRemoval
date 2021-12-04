#include "imageUtilities.h"
#include "mst_image.h"
#include "sorting.h"
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <chrono>

#define N_FRAMES        10
#define SEARCH_RANGE    70
#define PIXEL_COUNT     10000
using namespace cv;
using namespace std;
using namespace std::chrono;

struct RGB_u32 {
   uint32_t blue;
   uint32_t green;
   uint32_t red;
 };


 typedef enum {
         eNotInitialized  = 0,
         eInitialized,
 }eState;

int main(int argc, char * argv[]) {

        eState state = eNotInitialized;
        VideoCapture cam(0);
        cv::Mat colorMat, color, colorMask;
        cv::Size size;
        size.height = 720;
        size.width = 1280;
        cv::Mat grayMat(size, CV_8U);
        cv::Mat grayMat2(size, CV_8U);
        cv::Mat templateImage , outMatch;
        cv::Mat blurMat(size, CV_8U);
        cv::Mat blurMat2(size, CV_8U);
        cv::Mat sobelMat(size, CV_8U);
        cv::Mat sobelAngle(size, CV_64F);
        cv::Mat nThreshold(size, CV_8U);
        cv::Mat mask(size, CV_8U);
        cv::Mat filledMask(size, CV_8U);
        cv::Mat out(size, CV_8U);

        if (!cam.isOpened()) return -1;
        cam.set(CV_CAP_PROP_FRAME_WIDTH,1280);
        cam.set(CV_CAP_PROP_FRAME_HEIGHT,720);  
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
        templateImage  = imread("ojos2.png", 0);
        imshow("template", templateImage);
        for(;;) {
                        
                auto start = high_resolution_clock::now();
                cam >> colorMat;
                color = colorMat.clone();
#if 1
                bgr2grey(colorMat, grayMat);
                Gaussian3_3(grayMat, blurMat);
                //Gaussian3_3(grayMat2, blurMat);
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
                int32_t search = 0, pixelCount = 0;
                pEdges += (matchLoc.y + templateImage.rows) * s.width + matchLoc.x;
                while (search < SEARCH_RANGE) {
                        search++; 
                        pixelCount += DFS(nThreshold, out,
                         (matchLoc.y ) * s.width + matchLoc.x - search,
                          s.width * s.height - 1);

                        if (pixelCount > PIXEL_COUNT && state == eNotInitialized) {
                                state = eInitialized;
                                /*Get mask*/
                                fillOutside(colorMat, out);
                                getMask(nThreshold, out, mask);
                                fillMask(mask);
                                /*Color mask using first frame*/
                                colorMask = color.clone();
                        } 
                }

                fillOutside(colorMat, out);   
                removeMask(colorMat, out, mask, colorMask);  

#if 1
                if (state == eInitialized) {

                        for (int i = 0; i < 0; i++) {
                                bgr2grey(colorMat, grayMat2);
                                Gaussian3_3(grayMat2, blurMat);
                                /*
                                Gaussian3_3(blurMat, blurMat2);
                                Gaussian3_3(blurMat2, blurMat);
                                Gaussian3_3(blurMat, blurMat2);
                                Gaussian3_3(blurMat2, blurMat);*/
                                Sobel(blurMat, sobelMat, sobelAngle);
                                mthreshold(sobelMat, nThreshold, 49, 50, (uint8_t)maxVal);  
                                fillOutside(colorMat, nThreshold);
                        }
                }
#endif
#endif

                imshow("Original", color);
                imshow("Grey", grayMat2);
                imshow("Edges", nThreshold);
                imshow("Mask", mask);
                imshow("DFS", out);
                imshow("Out", colorMat);
                
                out = 0;

                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(stop - start);
                cout << duration.count() << " us " << "pixel count " << pixelCount << endl;    
                
                char c=(char)waitKey(25);
                if(c==27)
                        break;

        }

        return 1;
}
