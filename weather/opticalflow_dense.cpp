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
#include "opencv2/opencv_modules.hpp"
//#include <opencv2\imgproc\imgproc_c.h>
#include "opencv2/ml/ml.hpp"
//#include "opencv2\features2d\features2d.hpp"
//#include "opencv2\nonfree\nonfree.hpp"

//#include "opticalflow_sparse.cpp"

using namespace std;
using namespace cv;

void kNN(Mat image, Mat &labels);
void kNN_train(string train_file_path, Mat &data);
void kNN_paint(Mat labels, Mat &image, int width, int height);


int main( int argc, char** argv )
{
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);
    const int MAX_COUNT = 500;
    namedWindow( "Farneback Demo", 1 );

    VideoCapture cap("/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/небо5.avi");
    //cap.set(CV_CAP_PROP_POS_FRAMES, 4500);
    // "/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/Timelapse.mp4  небо5.avi"
    // "/home/liza/Загрузки/158 Лед.mp4"
    // open the video file for reading

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 0;
    }
    Mat gray, prevGray, image, frame, cartoon, image2, flow, labels;
    int counter = 0, sum_cloud = 0, cloud_counter = 0, count_tang = 0;
    int sumtang, lasttang, sumcos = 0;
    Point2f center, arrow_end;

    for(;;)
    {
        counter++;
        cap >> frame;
        if( frame.empty() )
            break;
        resize(frame, frame, cv::Size(), 0.7, 0.7);
        frame.copyTo(image);

        if (counter == 1) {
            kNN(image, labels);

            center.x = image.size().width/2;
            center.y = image.size().height/2;
            arrow_end.x = center.x;
            arrow_end.y = center.y;
        }

       // if (counter % 100 == 0)

            cvtColor(image, gray, COLOR_BGR2GRAY); //чернобелим
           // GaussianBlur(gray, gray, Size(7,7), 1.5, 1.5); //размываем
            GaussianBlur(image, image2, Size(25,25), 10, 10); //размываем

            if( counter % 25 == 0 )
            {
                kNN(image, labels);
                //cout << sumcos << "tan = " << sumtang << endl;
               // sumtang = (sumtang + lasttang)/2;

                if (count_tang != 0) {

                    if (sumcos > 0) {
                        sumtang = sumtang/count_tang;
                        arrow_end.x = center.x + 50/sqrt(1+sumtang*sumtang);  //50 - длина вектора
                        arrow_end.y = center.y + 50*sumtang/sqrt(1+sumtang*sumtang);
                    } else {
                        sumtang = sumtang/count_tang;
                        arrow_end.x = center.x - 50/sqrt(1+sumtang*sumtang);  //50 - длина вектора
                        arrow_end.y = center.y - 50*sumtang/sqrt(1+sumtang*sumtang);
                    }
                }

               // lasttang = sumtang;

               // count_tang = 0;
               // sumtang = 0;
               // sumcos = 0;
            }


                    if(prevGray.empty())
                        gray.copyTo(prevGray);
                    // dense flow для всех точек
                    calcOpticalFlowFarneback(prevGray, gray, flow, 0.4, 2, 15, 2, 8, 1.2, 0);
                    // 4 параметр =  pyr_scale определяет отношение масштабов между слоями пирамиды,
                    // 5 = levels – количество уровней в пирамиде
                    // 6 = winsize – размер окна, по которому производится усреднение
                    // 7 = iterations – количество итераций на каждом уровне
                    // 8 = poly_n – размер полинома, по которому оцениваются значения A и b
                    // 9 = poly_sigma – сигма гауссовского размытия при сглаживании производных
                    // 10 = flags – дополнительные флаги


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

       // }
         // рисум разметку на облака
         kNN_paint(labels, image, image.rows, image.cols);

        // рисуем стрелку
        line(image,center, arrow_end,Scalar(0,0,0),2);
        circle(image, arrow_end, 5, Scalar(0,0,0), -1, 8);
        //рисуем координатный круг
        circle(image, center, 60, Scalar(0,0,0), 1, 0);
        putText(image,"N",Point(center.x-10,center.y-70),FONT_HERSHEY_TRIPLEX,1,Scalar(0,0,0));
        putText(image,"S",Point(center.x-10,center.y+85),FONT_HERSHEY_TRIPLEX,1,Scalar(0,0,0));
        putText(image,"W",Point(center.x-85,center.y),FONT_HERSHEY_TRIPLEX,1,Scalar(0,0,0));
        putText(image,"E",Point(center.x+70,center.y),FONT_HERSHEY_TRIPLEX,1,Scalar(0,0,0));

        imshow("Farneback Demo", image);
        char c = (char)waitKey(10);
        if( c == 27 )
            break;



    }
    return 0;

}

void kNN(Mat image, Mat &labels)
{
    Mat train_result(400, 1, CV_32FC1, float(0)); //0 = небо
    Mat data;
    kNN_train("/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/train/sky1.jpg",data);
    CvKNearest knn(data, train_result);

    kNN_train("/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/train/clouds1.jpg",data);
    Mat train_result_clouds(400, 1, CV_32FC1, float(1)); //1 = облака
    knn.train(data,train_result_clouds, Mat(), false, 32, true);

    kNN_train("/home/liza/Документы/Нужно/6_сем/Практика/ЗАДАНИЕ СР/hist/train/clouds3.jpg",data);
    Mat train_result_clouds2(16, 1, CV_32FC1, float(1)); //1 = облака
    knn.train(data,train_result_clouds2, Mat(), false, 32, true);


    // estimate the response and get the neighbors' labels
    Mat results(322056,1,CV_32FC1, float(0)), neighborResponses(322056,1,CV_64F, float(0)),
            dists(322056,1,CV_32FC1, float(0));

    Mat original = image;

    image.convertTo(image,CV_8UC3);
    Mat data2;
    image.convertTo(data2,CV_32F);
    data2 = data2.reshape(1,data2.total());

    float response = 3;
    response = knn.find_nearest(data2,5,results,neighborResponses,dists);

    results = results.reshape(0, original.rows);

    labels = results;

    //kNN_paint(results, image, original.rows, original.cols);

}

void kNN_paint(Mat labels, Mat &image, int width, int height)
{
    for(int i = 0; i < width; ++i )    {
        for(int j = 0; j < height; ++j )    {
         // cout << results.at<float>(i,j) << "; ";
          if (labels.at<float>(i,j) == 0) //небо
             { //image.at<cv::Vec3b>(i,j)[1] += 40;
          }// зеленое небо, красное = облака,
          else   {
              image.at<cv::Vec3b>(i,j)[1] += 170;
          }
        }
    }


}

void kNN_train(string train_file_path, Mat &data)
{
    Mat train;
    Mat train_clouds = imread(train_file_path);
     train_clouds.convertTo(train,CV_8UC3);
     train_clouds.convertTo(data,CV_32F);
     // convert to float & reshape to a [3 x W*H] Mat
     //  (so every pixel is on a row of it's own)
     data = data.reshape(1,data.total());
}



