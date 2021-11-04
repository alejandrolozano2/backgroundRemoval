
#include "imageUtilities.h"


int bgr2grey (cv::Mat & src, cv::Mat & dst) {
        cv::Size s = src.size();

        uint8_t * gImage = dst.ptr<uint8_t>(0);
        RGB * data = src.ptr<RGB>(0);
        for ( int h = 0; h < s.height; h++) {
                for (int w = 0; w < s.width; w++) {
                        *gImage++ = (data->blue* 0.114 + data->green* 0.587 + data->red* 0.299);
                        data++;
                }
        }

}

/*Build edges
  The amount of edges to build is determined by size. 
  To build the graph for the image, the amount of edges will be:
  n_pixels * N. N is determied by a radius . e [j,k,w] j and k are pixels   
*/
uint32_t buildEdges(cv::Mat & image, uint32_t (* e)[3], uint32_t radius) {
        cv::Size s = image.size();
        RGB * p = image.ptr<RGB>(0);
        uint32_t i = 0, j =0 , diff = 0, maxdiff= 0;

        for ( int r = 0; r < s.height; r++) {          
                for (int c = 0; c < s.width; c++) {
                        j = r * s.width + c;
                        if (c < s.width - 1 ) {
                                diff = bgrDiff(p, p + 1);            //Get difference with the pixel on the right
                                e[i][2] = diff;
                                e[i][1] = j + 1;                        //One pixel to the right
                                e[i++][0] = j;                          //Current Pixel
                                if ( diff >  maxdiff) maxdiff = diff;
                        }

                        if (r < s.height - 1 ) {
                                diff = bgrDiff(p, p + s.width);      //Get difference with the pixel below
                                e[i][2] = diff;
                                e[i][1] = j + s.width;                  //Pixel below
                                e[i++][0] = j;
                                if ( diff >  maxdiff) maxdiff = diff;
                        }
                        p++;
                }
        }

        return maxdiff;

}