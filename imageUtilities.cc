
#include "imageUtilities.h"
#include <list>
#include <iostream>

using namespace std;
#define WHITE  255
#define BLACK  0
#define BACKGROUND  BLACK
/*Own BGR to Gray*/
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
        return  0;
}

/*Gaussian 3x3 Blur*/
int Gaussian3_3 (cv::Mat & src, cv::Mat & dst) {
        cv::Size s = src.size();

        uint8_t (* bImage)[s.width] = (uint8_t (*)[s.width])dst.ptr<uint8_t>(0);
        uint8_t (* sImage)[s.width] = (uint8_t (*)[s.width])src.ptr<uint8_t>(0);
        
        for ( int h = 1; h < s.height-1; h++) {
                for (int w = 1; w < s.width-1; w++) {
                        bImage[h][w] =  (float)sImage[h-1][w-1]/16 + (float)sImage[h-1][w]/8 + (float)sImage[h-1][w+1]/16 + 
                                        (float)sImage[h][w-1]/8    + (float)sImage[h][w]/4   + (float)sImage[h][w+1]/8    +
                                        (float)sImage[h+1][w-1]/16 + (float)sImage[h+1][w]/8 + (float)sImage[h+1][w+1]/16;
                }
        }
        return 0;
}


/*Sobel*/
int Sobel (cv::Mat & src, cv::Mat & dst, cv::Mat  & angles) {
        cv::Size s = src.size();

        uint8_t (* bImage)[s.width] = (uint8_t (*)[s.width])dst.ptr<uint8_t>(0);
        uint8_t (* sImage)[s.width] = (uint8_t (*)[s.width])src.ptr<uint8_t>(0);
        float (* aImage)[s.width] = (float (*)[s.width])angles.ptr<float>(0);
        float x, y, a;
        for ( int h = 1; h < s.height-1; h++) {
                for (int w = 1; w < s.width-1; w++) {
                        x =     -1.0 * (float)sImage[h-1][w-1] + (float)sImage[h-1][w+1] + 
                                -2.0*(float)sImage[h][w-1] + 2.0*(float)sImage[h][w+1]    +
                                -1.0*(float)sImage[h+1][w-1] + (float)sImage[h+1][w+1];
                        y =     -1.0 * (float)sImage[h-1][w-1] + -2.0*(float)sImage[h-1][w] + -1*(float)sImage[h-1][w+1] + 
                                (float)sImage[h+1][w-1] + 2.0*(float)sImage[h+1][w] + (float)sImage[h+1][w+1];
                        bImage[h][w] = sqrt(x*x + y*y);
                        a = atan2(y,x) * 180.0 / M_PI;
                        aImage[h][w] = a < 0 ? a + 180.0 : a; 
                }
        }

        return  0;
}

int nonMaxSuppresion (cv::Mat & img, cv::Mat & T, cv::Mat  & dst) {
        cv::Size s = img.size();

        uint8_t (* bImage)[s.width] = (uint8_t (*)[s.width])dst.ptr<uint8_t>(0);
        uint8_t (* gImage)[s.width] = (uint8_t (*)[s.width])img.ptr<uint8_t>(0);
        float (* angle)[s.width] = (float (*)[s.width])T.ptr<float>(0);
        float q, r;
        for ( int h = 1; h < s.height-1; h++) {
                for (int w = 1; w < s.width-1; w++) {
                        q = r = 255.0;
                        
                        if ((0 <= angle[h][w] < 22.5) || (157.5 <= angle[h][w] <= 180)) {
                                q = gImage[h][w+1];
                                r = gImage[h][w-1];
                        }
                        else if (22.5 <= angle[h][w] < 67.5) {
                                q = gImage[h+1][w-1];
                                r = gImage[h-1][w+1];
                        }
                        else if (67.5 <= angle[h][w] < 112.5) {
                                q = gImage[h+1][w];
                                r = gImage[h-1][w];
                        }
                        else if (112.5 <= angle[h][w] < 157.5) {
                                q = gImage[h-1][w-1];
                                r = gImage[h+1][w+1];
                        }
                        if ((gImage[h][w] >= q) && (gImage[h][w] >= r)) {
                                bImage[h][w] = gImage[h][w];
                        }
                        else {
                                bImage[h][w] = 0;
                        }
                }
        }
        return 0;
}


int mthreshold (cv::Mat & src, cv::Mat & dst, float lowRatio, float highRatio, uint8_t max) {
        cv::Size s = src.size();
        uint8_t width = 2;
        uint8_t weak = 0;
        uint8_t strong = 255;

        uint8_t highTh = (uint8_t)highRatio;//max * highRatio;
        uint8_t lowTh = (uint8_t)lowRatio;//highTh * lowRatio;

        uint8_t (* bImage)[s.width] = (uint8_t (*)[s.width])dst.ptr<uint8_t>(0);
        uint8_t (* sImage)[s.width] = (uint8_t (*)[s.width])src.ptr<uint8_t>(0);
        
        for ( int h = 0; h < s.height; h++) {
                for (int w = 0; w < s.width; w++) {

                        if ( (h >= 0 && h <= width) || (h < s.height && h >= s.height - width) ||
                                (w >= 0 && w <= width) || (w < s.width && w >= s.width - width ) ) {
                                        bImage[h][w] = 0;
                                        continue;
                        }

                        if (sImage[h][w] >= highTh ) {
                                bImage[h][w] = strong;
                        } else if (sImage[h][w] < highTh && sImage[h][w] >= lowTh) {
                                bImage[h][w] = weak;
                        } else {
                                bImage[h][w] = 0;
                        }       
                }
        }
        return 0;
}

int mhysteresis(cv::Mat & src, cv::Mat & dst) {
        cv::Size s = src.size();
        uint8_t weak = 25;
        uint8_t strong = 255;

        uint8_t (* bImage)[s.width] = (uint8_t (*)[s.width])dst.ptr<uint8_t>(0);
        uint8_t (* sImage)[s.width] = (uint8_t (*)[s.width])src.ptr<uint8_t>(0);
        
        for ( int h = 1; h < s.height-1; h++) {
                for (int w = 1; w < s.width-1; w++) { 
                        if (((sImage[h+1][w-1] == strong) || (sImage[h+1][w] == strong) || (sImage[h+1][w+1] == strong)
                                || (sImage[h][w-1] == strong) || (sImage[h][w+1] == strong)
                                || (sImage[h-1][w-1] == strong) || (sImage[h-1][w] == strong) || (sImage[h-1][w+1] == strong)) ) {
                                bImage[h][w] = strong;
                        }
                        else {
                                bImage[h][w] = 0 ;               
                        }
                    
                }
        }
        return 0;
        
}

/*Build edges
  The amount of edges to build is determined by size. 
  To build the graph for the image, the amount of edges will be:
  n_pixels * N. N is determied by a radius . e [j,k,w] j and k are pixels   
*/
uint32_t buildEdges(cv::Mat & image, uint32_t (* e)[3], uint32_t in , uint32_t out ,uint32_t step) {
        cv::Size s = image.size();
        RGB * p = image.ptr<RGB>(0);
        uint32_t i = 0, j =0 , diff = 0, maxdiff= 0;

        for ( int r = 0; r < s.height; r++) {          
                for (int c = 0; c < s.width; c++) {
                        j = r * s.width + c;

                        for ( int st = in; st < out; st+=step) {
                                if (c < s.width  - st ) {
                                        diff = bgrDiff(p, p + st);            //Get difference with the pixel on the right
                                        e[i][2] = diff;
                                        e[i][1] = j + st;                        //One pixel to the right
                                        e[i++][0] = j;                          //Current Pixel
                                        if ( diff >  maxdiff) maxdiff = diff;
                                }

                                if (r < s.height - st ) {
                                        diff = bgrDiff(p, p + (s.width*st));      //Get difference with the pixel below
                                        e[i][2] = diff;
                                        e[i][1] = j + (s.width*st);                  //Pixel below
                                        e[i++][0] = j;
                                        if ( diff >  maxdiff) maxdiff = diff;
                                }
                        }
                        p++;
                }
        }

        return maxdiff;

}

uint32_t correlation(cv::Mat & src, cv::Mat & shape, uint32_t * row, uint32_t * col) {
        cv::Size imageSize = src.size();
        cv::Size shapeSize = shape.size();
        uint32_t corr;
        uint32_t max_corr = 0;
        uint32_t currentP;        
        uint8_t * pI = src.ptr<uint8_t>(0);
        uint8_t * pS = shape.ptr<uint8_t>(0);

        for (uint32_t r = 0; r < imageSize.height - shapeSize.height; r++) {
                for (uint32_t c = 0; c < imageSize.width - shapeSize.width; c++) {
                        corr = 0;
                        uint8_t * pS = shape.ptr<uint8_t>(0);
                        for (uint32_t rt = 0; rt < shapeSize.height; rt++) {
                                for (uint32_t ct = 0; ct < shapeSize.width; ct++) {
                                        currentP = (r + rt) * imageSize.width + c + ct;
                                        corr += *(pI + currentP) * (*pS++);
                                        if (corr > max_corr) {
                                                *row = r;
                                                *col = c;
                                                max_corr = corr;
                                        }

                                }
                        }        
                }
        }

        return max_corr;
}

bool isPixelInside(uint32_t row, uint32_t col, uint32_t * lM,
                 uint32_t * rM, uint32_t * tM) {

        if ( (row >= tM[col]) && (col <= rM[row]) && (col >= lM[row])) 
                return true;

        return false;
}

void fillOutside(cv::Mat & colorOut, cv::Mat & edges) {
        cv::Size s = edges.size();

        RGB * out = colorOut.ptr<RGB>(0);
        uint8_t * edge = edges.ptr<uint8_t>(0);
        bool found =  false;
        uint32_t startFound = 0;
        for (uint32_t row = 0; row < s.height; row++) {
                found = false;
                startFound = 0;
                for (uint32_t col = 0; col < s.width; col++, out++) {

                        uint8_t * p = edge + (row * s.width) + col;
                        if (row == 0 || col == 0 || row == s.height || col == s.width) {
                                if (*p == 0) out->blue = out->green = out->red = BACKGROUND;
                                continue;
                        }

                        if (!found && *p == 0) {
                                out->blue = out->green = out->red = BACKGROUND;
                                continue;
                        }

                        if (found && *p == 0 && *(p - s.width)== 0) {
                                out->blue = out->green = out->red = BACKGROUND;
                                found = false;
                                continue;
                        }

                        if (found && *(p - s.width)== WHITE && *p == 0) {
                                continue;
                        }

                        if (found && *(p - s.width)== WHITE && *p == WHITE) {
                                for (uint32_t i = 0; i < col - startFound; i++) *(p-i) = 255;
                                startFound = col;
                                continue;
                        }
                        
                        if (*p == 255 ) {
                                found = true;
                                startFound = col;
                                continue;
                        }

                }
        }

}


void fillMask(cv::Mat & edges) {
        cv::Size s = edges.size();

        uint8_t * edge = edges.ptr<uint8_t>(0);
        bool found =  false;
        uint32_t startFound = 0;
        for (uint32_t row = 0; row < s.height; row++) {
                found = false;
                startFound = 0;
                for (uint32_t col = 0; col < s.width; col++) {

                        uint8_t * p = edge + (row * s.width) + col;
                        if (row == 0 || col == 0 || row == s.height || col == s.width) {
                                
                                continue;
                        }

                        if (!found && *p == 0) {
                                
                                continue;
                        }

                        if (found && *p == 0 && *(p - s.width)== 0) {
                                
                                found = false;
                                continue;
                        }

                        if (found && *(p - s.width)== 255 && *p == 0) {
                                continue;
                        }

                        if (found && *(p - s.width)== 255 && *p == 255) {
                                for (uint32_t i = 0; i < col - startFound; i++) *(p-i) = 255;
                                startFound = col;
                                continue;
                        }
                        
                        if (*p == 255 ) {
                                found = true;
                                startFound = col;
                                continue;
                        }

                }
        }

}

void removeMask(cv::Mat & colorOut, cv::Mat & edges, cv::Mat & mask, cv::Mat & Image) {
        #define THRESHOLD 20
        cv::Size s = edges.size();
        RGB * out = colorOut.ptr<RGB>(0);
        RGB * img = Image.ptr<RGB>(0);
        uint8_t * edge = edges.ptr<uint8_t>(0);
        uint8_t * msk = mask.ptr<uint8_t>(0);

        for (uint32_t row = 0; row < s.height; row++) {
                for (uint32_t col = 0; col < s.width; col++, out++, img++, msk++, edge++ ) {

                        if (*edge == 255 && *msk == 255) {
                                if (bgrDiff(out,img) < THRESHOLD ) {
                                        out->blue = out->green = out->red = BACKGROUND;
                                }
                        } 
                }
        }

}

uint32_t DFS(cv::Mat & image, cv::Mat & out, uint32_t start, uint32_t end) {
        
        uint32_t pixelCount = 0;
        cv::Size s = image.size();
        uint8_t * sImage = image.ptr<uint8_t>(0);
        uint8_t * outImage = out.ptr<uint8_t>(0);
        uint32_t cPixel;
        int32_t row, col;
        list <uint32_t> stack;

        if ( *(sImage + start) == WHITE) 
                stack.push_front(start);

        while (!stack.empty()) {
                cPixel = stack.front();
                stack.pop_front();

                if (cPixel == end)
                        return pixelCount;

                row = cPixel / s.width;
                col = cPixel % s.width;

                if (*(outImage + cPixel) == 0) {
                        pixelCount += 1;
                        *(outImage + cPixel) = WHITE;

                        if ( row - 1 >= 0 ) {
                                if ( *(sImage + cPixel - s.width) == WHITE)
                                        stack.push_front( cPixel - s.width);
                        }
                        
                        if (row + 1 <= s.height) {
                                 if ( *(sImage + cPixel + s.width) == WHITE)
                                        stack.push_front( cPixel + s.width);
                        }
                        
                        if (col - 1 >= 0) {
                                 if ( *(sImage + cPixel - 1) == WHITE)
                                        stack.push_front( cPixel - 1);
                        }
                        
                        if (col + 1 <= s.width) {
                                 if ( *(sImage + cPixel + 1) == WHITE)
                                        stack.push_front( cPixel + 1);
                        }
                 }

        }

        return pixelCount;
}

void getMask(cv::Mat & edges, cv::Mat & person, cv::Mat & mask) {
        #define WHITE  255
        cv::Size s = edges.size();

        mask = edges.clone();
        uint8_t * pp = person.ptr<uint8_t>(0);
        uint8_t * mp = mask.ptr<uint8_t>(0);
        uint8_t * ep = edges.ptr<uint8_t>(0);


        for (uint32_t row = 0; row < s.height; row++) {
                for (uint32_t col = 0; col < s.width; col++, pp++, mp++, ep++ ) {
                        if (*pp == WHITE) {
                                *mp = 0;
                        } else {
                                *mp = *ep;
                        }
                }
        }
}
