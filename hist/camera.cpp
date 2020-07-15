#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/core/cvstd.hpp"
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

int main3(int argc, char* argv[]) {
//Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD); - нужна версия позднее
VideoCapture cap("/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/Timelapse.mp4"); // open the video file for reading
if(!cap.isOpened())
// check if we succeeded
return -1;
Mat edges;
namedWindow("edges",1);
while(1) {
Mat frame;
cap >> frame; // get a new frame from camera
cvtColor(frame, edges, COLOR_BGR2GRAY);
// Перевод в градации серого
//dilate(edges, edges,cv::Mat(),cv::Point(-1,-1),3);
//erode(edges, edges,cv::Mat(),cv::Point(-1,-1),1);
GaussianBlur(edges, edges, Size(7,7), 5, 5); // Размытие
Canny(edges, edges, 0, 30, 3);
// Выделение границ
imshow("edges", edges);
if(waitKey(30) >= 0) break;
}
return 0;
}
