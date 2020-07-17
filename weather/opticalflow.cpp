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

using namespace std;
using namespace cv;

Point2f point;

/*static void help()
{
    // print a welcome message, and the OpenCV version
    cout << "\nThis is a demo of Lukas-Kanade optical flow lkdemo(),\n"
            "Using OpenCV version " << CV_VERSION << endl;
    cout << "\nIt uses camera by default, but you can provide a path to video as an argument.\n";
    cout << "\nHot keys: \n"
            "\tESC - quit the program\n"
            "\tr - auto-initialize tracking\n"
            "\tc - delete all the points\n"
            "\tn - switch the \"night\" mode on/off\n"
            "To add/remove a feature point click it\n" << endl;
}
bool addRemovePt = false;
static void onMouse( int event, int x, int y, int /*flags*//*, void* /*param*//* )
{
    if( event == EVENT_LBUTTONDOWN )
    {
        point = Point2f((float)x, (float)y);
        addRemovePt = true;
    }
}*/
int main( int argc, char** argv )
{
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);
    const int MAX_COUNT = 500;
 //   bool needToInit = false;
 //bool nightMode = false;
    //help();

    VideoCapture cap("/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/тучи4.mp4");
    // "/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/Timelapse.mp4"
    // "/home/liza/Загрузки/158 Лед.mp4"
    // open the video file for reading

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 0;
    }
    namedWindow( "LK Demo", 1 );
//setMouseCallback( "LK Demo", onMouse, 0 );
    Mat gray, prevGray, image, frame;
    int counter = 0, sum_cloud = 0, cloud_counter = 0, count_tang = 0;
    int sumtang = 0;
    int sumcos = 0;
    Point2f center, arrow_end;
    vector<Point2f> points[2];
    for(;;)
    {
        counter++;
        cap >> frame;
        if( frame.empty() )
            break;
        resize(frame, frame, cv::Size(), 0.4, 0.4);
        frame.copyTo(image);
        cvtColor(image, gray, COLOR_BGR2GRAY); //чернобелим
/*        if( nightMode )
            image = Scalar::all(0);*/
        if( counter % 80 == 0 )
        {
            cout << sumcos << "tan = " << sumtang << endl;

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
            if (cloud_counter > 50) {
                cloud_counter = 0;
                cout << "Скоро грянет буря!" << endl;
            }
            // automatic initialization
            goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10); //, Mat(), 3, 3, 0, 0.04
            // Finds edges in an image using the [Canny86] algorithm. Ищет контуры
            cornerSubPix(gray, points[1], subPixWinSize, Size(-1,-1), termcrit);
            //Refines the corner locations.   Ищет углы в найденных контурах
 //addRemovePt = false;  // добавление точки
        }
        else if( !points[0].empty() )
        {
            line(image,center, arrow_end,Scalar(0,0,0),2);
            circle( image, arrow_end, 5, Scalar(0,0,0), -1, 8);

            sum_cloud = 0;
            count_tang = 0;
            sumtang = 0;
            sumcos = 0;

            vector<uchar> status;
            vector<float> err;
            if(prevGray.empty())
                gray.copyTo(prevGray);
            calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
                                 3, termcrit, 0, 0.001);
 // Calculates an optical flow for a sparse feature set using the iterative Lucas-Kanade method with pyramids.
   // вычисляет оптический поток между точками в двух кадрах
            size_t i, k;
            for( i = k = 0; i < points[1].size(); i++ )
            {
                /*if( addRemovePt )
                {
                    if( norm(point - points[1][i]) <= 5 )
                    {
                        addRemovePt = false;
                        continue;
                    }
                }*/
                if( !status[i] )
                    continue;
                points[1][k++] = points[1][i];
                line(image,points[0][i], points[1][i],Scalar(255,0,0));
                circle( image, points[1][i], 3, Scalar(0,255,0), -1, 8);

               // if ((abs(points[0][i].x - points[1][i].x) > 0.1) or
                  //      (abs(points[0][i].y - points[1][i].y) > 0.1))  {
                    sum_cloud ++;
                    // считаем тангенс векторов движения
                    sumtang += ((points[1][i].y - points[0][i].y)/(points[1][i].x - points[0][i].x));
                    count_tang ++;
                    // считаем косинусы векторов движения
                    sumcos += (points[1][i].x - points[0][i].x);
                            //                        (sqrt((points[1][i].y - points[0][i].y)
                          //  + (points[1][i].x - points[0][i].x)));
                   // cout << (points[1][i].y - points[0][i].y)*(points[1][i].y - points[0][i].y) << endl;

               // }

            }
            if (sum_cloud > 0.9*points[0].size())
                cloud_counter ++;
            points[1].resize(k);
        }
        if( /*addRemovePt &&*/ points[1].size() < (size_t)MAX_COUNT )
        {
            vector<Point2f> tmp;
            tmp.push_back(point);
            cornerSubPix( gray, tmp, winSize, Size(-1,-1), termcrit);
            //Refines the corner locations.   Ищет углы в найденных контурах
            points[1].push_back(tmp[0]);
// addRemovePt = false;
        }
      //  needToInit = false;
        imshow("LK Demo", image);
        char c = (char)waitKey(10);
        if( c == 27 )
            break;
        /*switch( c )
        {
        case 'r':
            needToInit = true;
            break;
        case 'c':
            points[0].clear();
            points[1].clear();
            break;
        case 'n':
            nightMode = !nightMode;
            break;
        }*/
        std::swap(points[1], points[0]);
        cv::swap(prevGray, gray);
    }
    return 0;
}

/*int main6(int argc, char* argv[]) {
//Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD); - нужна версия позднее
VideoCapture cap("/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/Timelapse.mp4");
// open the video file for reading
if(!cap.isOpened())
// check if we succeeded
return -1;
Mat image;
namedWindow("optical flow",1);
vector<Point2f> points[2];

while(1) {
Mat frame;
cap >> frame; // get a new frame from camera

frame.copyTo(image);
cvtColor(image, gray, COLOR_BGR2GRAY);
if( needToInit ) { // automatic initialization
    goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 3, 0, 0.04); //находим края в points[1]
    cornerSubPix(gray, points[1], subPixWinSize, Size(-1,-1), termcrit); //находим углы  в points[1]
    addRemovePt = false;
}
else if( !points[0].empty() ){
    vector<uchar> status;
    vector<float> err;
    if(prevGray.empty())
        gray.copyTo(prevGray);
    calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,3, termcrit, 0, 0.001);
    size_t i, k;
...
    points[1][k++] = points[1][i];
    circle( image, points[1][i], 3, Scalar(0,255,0), -1, 8);
}

imshow("optical flow", image);


if(waitKey(30) >= 0) break;
}
return 0;
}*/
