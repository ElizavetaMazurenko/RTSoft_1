#include <iostream>
#include <string>
#include <ctype.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/core/cvstd.hpp"
#include "opencv2/core/core.hpp"

//#include "opticalflow_sparse.cpp"

using namespace std;
using namespace cv;


int main( int argc, char** argv )
{
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);
    const int MAX_COUNT = 500;
    namedWindow( "Farneback Demo", 1 );

    VideoCapture cap("/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/тучи2-2.mp4");
    cap.set(CV_CAP_PROP_POS_FRAMES, 1500);
    // "/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/Timelapse.mp4"
    // "/home/liza/Загрузки/158 Лед.mp4"
    // open the video file for reading

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 0;
    }
    Mat gray, prevGray, image, frame, cartoon, image2, flow;
    int counter = 0, sum_cloud = 0, cloud_counter = 0, count_tang = 0;
    int sumtang = 0;
    int sumcos = 0;
    Point2f center, arrow_end;

    for(;;)
    {
        counter++;
        cap >> frame;
        if( frame.empty() )
            break;
        resize(frame, frame, cv::Size(), 0.7, 0.7);
        frame.copyTo(image);

        cvtColor(image, gray, COLOR_BGR2GRAY); //чернобелим
       // GaussianBlur(gray, gray, Size(7,7), 1.5, 1.5); //размываем
        GaussianBlur(image, image2, Size(25,25), 10, 10); //размываем
      //  kMeansClustering(image2, cartoon);

        if( counter % 200 == 0 )
        {
          //  cout << sumcos << "tan = " << sumtang << endl;

            if (count_tang != 0) {
                center.x = image.size().width/2;
                center.y = image.size().height/2;

                if (sumcos > 0) {
                    arrow_end.x = center.x + 60;
                    arrow_end.y = center.y + 60*sumtang / count_tang;
                } else {
                    arrow_end.x = center.x - 60;
                    arrow_end.y = center.y - 60*sumtang / count_tang;
                }
            }

            count_tang = 0;
            sumtang = 0;
            sumcos = 0;
        }

            // рисуем стрелку
            line(image,center, arrow_end,Scalar(0,0,0),2);
            circle(image, arrow_end, 5, Scalar(0,0,0), -1, 8);

            if (counter % 5 == 0)
            {
                if(prevGray.empty())
                    gray.copyTo(prevGray);
                // dense flow для всех точек
                calcOpticalFlowFarneback(prevGray, gray, flow, 0.4, 1, 12, 2, 8, 1.2, 0);


                for (int y = 0; y < image.rows; y += 5) {
                 for (int x = 0; x < image.cols; x += 5)
                     {   // get the flow from y, x position * 10 for better visibility
                      const Point2f flowatxy = flow.at<Point2f>(y, x) * 20;
                       // draw line at flow direction
                     // if ((flowatxy.x > 0.1) or (flowatxy.y > 0.1))
                      line(image, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(255,0,0));

                      // считаем тангенс векторов движения
                      if ((abs(flowatxy.x)>0.3) or (abs(flowatxy.y)>0.3)) {
                         sumtang += flowatxy.y/flowatxy.x;
                         count_tang ++;
                         // считаем ~~косинусы векторов движения
                         sumcos += flowatxy.x;
                      }
                     }
                    }

                 swap(prevGray, gray);
             }
        imshow("Farneback Demo", image);
        char c = (char)waitKey(10);
        if( c == 27 )
            break;



    }
    return 0;
}



