#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;


int main1(int, char**) {
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
imshow( "Gray", gray );
waitKey(0);
}
