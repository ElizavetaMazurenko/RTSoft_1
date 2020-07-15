#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

int main2(int, char**) {
    Mat gray = imread("/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/image.jpg",0);
    namedWindow( "Gray", WINDOW_AUTOSIZE );
    int histSize = 256;
    // bin size
    float range[] = { 0, 255 };
    const float *ranges[] = { range };
    MatND hist;
    calcHist( &gray, 1, 0, Mat(), hist, 1, &histSize, ranges, true, false ); // Calculate histogram
    double total;
    total = gray.rows * gray.cols;
    for( int h = 0; h < histSize; h++ ) { // Show the calculated histogram in command window
    float binVal = hist.at<float>(h);
    cout<<" "<<binVal;
    }

int hist_w = 512; int hist_h = 400;
int bin_w = cvRound( (double) hist_w/histSize );
Mat histImage( hist_h, hist_w, CV_8UC1, Scalar( 0,0,0) );
normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
for( int i = 1; i < histSize; i++ )
{
line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
Scalar( 255, 0, 0), 2, 8, 0 );
}
namedWindow( "Result", 1 );
imshow( "Result", histImage );
imshow( "Gray", gray );
waitKey(0);
return 0;
}
