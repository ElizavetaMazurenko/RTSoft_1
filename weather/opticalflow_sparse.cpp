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

void kMeansClustering(Mat MainFrame, Mat &retCartoon);

Point2f point;

int mainsparse( int argc, char** argv )
{
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);
    const int MAX_COUNT = 500;

    VideoCapture cap(0);
    // "/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/Timelapse.mp4"
    // "/home/liza/Загрузки/158 Лед.mp4"
    // open the video file for reading

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 0;
    }
    namedWindow( "LK Demo", 1 );
    Mat gray, prevGray, image, frame, cartoon, image2, flow;
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
        resize(frame, frame, cv::Size(), 0.5, 0.5);
        frame.copyTo(image);

        cvtColor(image, gray, COLOR_BGR2GRAY); //чернобелим
       // GaussianBlur(gray, gray, Size(7,7), 1.5, 1.5); //размываем
        GaussianBlur(image, image2, Size(25,25), 10, 10); //размываем

        kMeansClustering(image2, cartoon);

        if((counter == 1) or (counter % 80 == 0))  {
            goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10); //, Mat(), 3, 3, 0, 0.04
           // cornerSubPix(gray, points[1], subPixWinSize, Size(-1,-1), termcrit);
        }

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
        }
            // automatic initialization
            // Finds edges in an image using the [Canny86] algorithm. Ищет контуры
            //Refines the corner locations.   Ищет углы в найденных контурах
 //addRemovePt = false;  // добавление точки

        else if( !points[0].empty() )
        {

            // рисуем стрелку
            line(cartoon,center, arrow_end,Scalar(0,0,0),2);
            circle( cartoon, arrow_end, 5, Scalar(0,0,0), -1, 8);

            sum_cloud = 0;
            count_tang = 0;
            sumtang = 0;
            sumcos = 0;

            vector<uchar> status;
            vector<float> err;


            calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
                                 3, termcrit, 0, 0.001);  // winsize = размер окна расчетов точек, можно 21-21

            //  3 = maxLevel = количество слоев в пирамиде перемещения точки, можно 5
 // Calculates an optical flow for a sparse feature set using the iterative Lucas-Kanade method with pyramids.
   // вычисляет оптический поток между точками в двух кадрах
            size_t i, k;
            for( i = k = 0; i < points[1].size(); i++ )
            {
                if( !status[i] )
                    continue;
                points[1][k++] = points[1][i];
                // рисуем точки и хвостики
                line(cartoon,points[0][i], points[1][i],Scalar(255,0,0));
                circle(cartoon, points[1][i], 3, Scalar(0,255,0), -1, 8);

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
        if( points[1].size() < (size_t)MAX_COUNT )
        {
            vector<Point2f> tmp;
            tmp.push_back(point);
            cornerSubPix( gray, tmp, winSize, Size(-1,-1), termcrit);
            //Refines the corner locations.   Ищет углы в найденных контурах
            points[1].push_back(tmp[0]);
// addRemovePt = false;
        }
      //  needToInit = false;
        imshow("LK Demo", cartoon);
        char c = (char)waitKey(10);
        if( c == 27 )
            break;
        std::swap(points[1], points[0]);
        cv::swap(prevGray, gray);
    }
    return 0;
}



void kMeansClustering(Mat MainFrame, Mat &retCartoon)
{
    cv::Mat img = MainFrame.clone();
   // cv::cvtColor(img, img, CV_BGR2RGB);
  //  cv::resize(img, img , cv::Size(), 0.2, 0.2);
    img.convertTo(img,CV_8UC3);
    Mat ocv = img;
    // convert to float & reshape to a [3 x W*H] Mat
    //  (so every pixel is on a row of it's own)
    Mat data;
    ocv.convertTo(data,CV_32F);
    data = data.reshape(1,data.total());
    // do kmeans
    Mat labels, centers1;
    kmeans(data, 2, labels, TermCriteria(CV_TERMCRIT_ITER, 10, 1.0), 10,
           KMEANS_PP_CENTERS, centers1);

    // reshape both to a single row of Vec3f pixels:
    centers1 = centers1.reshape(3,centers1.rows);
    data = data.reshape(3,data.rows);

    // replace pixel values with their center value:
    Vec3f *p = data.ptr<Vec3f>(); //ptr дает указатель на строку
    // p = pixel of image?
    float one, zero = 0;
    for (size_t i=0; i<data.rows; i++) {
        if (labels.at<int>(i) == 0)
            zero++;
        else
            one++;
    }

    bool one_gr_zero;
    if (one>zero)
    {
        one_gr_zero = 1;
        cout << "Процентное отношение туч и неба:" << endl\
             << "туч = " << one/(one+zero)*100 << "%, небо = "\
             << zero/(one+zero)*100 << "%" << endl;
    }

    for (size_t i=0; i<data.rows; i++) {
       int center_id = labels.at<int>(i);
       if (one_gr_zero)  {
       if (center_id == 1)
        p[i][0]+=100;
       else
           p[i][2]+=100;
       }
       else  {
           if (center_id == 0)
            p[i][0]+=100;
           else
               p[i][2]+=100;
       }

    }

    // back to 2d, and uchar:
    ocv = data.reshape(3, ocv.rows);
    ocv.convertTo(ocv, CV_8U);
    retCartoon = ocv;
}
