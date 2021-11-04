#include "imageUtilities.h"
#include "mst_image.h"
#include "sorting.h"
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <chrono>


using namespace cv;
using namespace std;
using namespace std::chrono;


int main(int argc, char * argv[]) {

        VideoCapture cam(0);
        cv::Mat colorMat;
        cv::Size size;
        size.height = 480;
        size.width = 640;
        cv::Mat grayMat(size, CV_8U);

        if (!cam.isOpened()) return -1;
        /*Save JPG of first capture*/
        //cam >> colorMat;
        //imwrite("Prueba.jpg", colorMat);
        
        cv::Size s = colorMat.size();
        uint32_t imageSize = s.height * s.width;

/*****************************Build Edges Test****************************************/

/*************************************************************************************/

        for(;;) {
                
                
                auto start = high_resolution_clock::now();
                cam >> colorMat;
                bgr2grey(colorMat, grayMat);
                imshow("Color", colorMat);
                imshow("Gray", grayMat);
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(stop - start);
                cout << duration.count() << " Microseconds " << endl;    
                
                char c=(char)waitKey(25);
                if(c==27)
                        break;

        }

        return 1;
}
