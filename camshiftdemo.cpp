#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
//#include "facedetect.h"


#include <iostream>
#include <ctype.h>

#ifndef FACEDETECT_H
#define FACEDETECT_H
#include "facedetect.h"
#endif // !FACEDETECT_H
#include "tracking.h"//自身使用的时候会定义facedetect.h头文件，二次调用的时候，不会定义该头文件
//头文件的顺序

using namespace cv;
using namespace std;

Mat image;

bool backprojMode = false;//表示是否进入反向投影模式，true表示进入反向投影模式
bool selectObject = false;//表示是否在选择跟踪的初始目标
int trackObject = 0;//代表跟踪目标的数量
bool showHist = true;//是否显示直方图
Point origin;
Rect selection;
int vmin = 10, vmax = 256, smin = 30;

// User draws box around object to track. This triggers CAMShift to start tracking
static void onMouse( int event, int x, int y, int, void* )
{
    if( selectObject )
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);

        selection &= Rect(0, 0, image.cols, image.rows);
    }

    switch( event )
    {
    case EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selection = Rect(x,y,0,0);
        selectObject = true;
        break;
    case EVENT_LBUTTONUP:
        selectObject = false;
        if( selection.width > 0 && selection.height > 0 )
            trackObject = -1;   // Set up CAMShift properties in main() loop
        break;
    }
}
//多任务时间，也许使用了进程切换吧

string hot_keys =
    "\n\nHot keys: \n"
    "\tESC - quit the program\n"
    "\tc - stop the tracking\n"
    "\tb - switch to/from backprojection view\n"
    "\th - show/hide object histogram\n"
    "\tp - pause video\n"
    "To initialize tracking, select the object with mouse\n";

static void help()
{
    cout << "\nThis is a demo that shows mean-shift based tracking\n"
            "You select a color objects such as your face and it tracks it.\n"
            "This reads from video camera (0 by default, or the camera number the user enters\n"
            "Usage: \n"
            "   ./camshiftdemo [camera number]\n";
    cout << hot_keys;
}

const char* keys =
{
    "{help h | | show help message}{@camera_number| 0 | camera number}"
};
//加入facedetect函数，代替之前的selection。在完成一次跟踪，就使用facedetect在
//规定区域检测一次，如果在跟踪局域检测到人脸，则跟踪成功，否则跟踪失败。
//看到的大部分论文没有提到这个。也算一个创新点
int main( int argc, const char** argv )
{
    VideoCapture cap;
    Rect trackWindow;
    int hsize = 16;
    float hranges[] = {0,180};
    const float* phranges = hranges;
    CommandLineParser parser(argc, argv, keys);
    if (parser.has("help"))
    {
        help();
        return 0;
    }
    int camNum = parser.get<int>(0);
    cap.open(camNum);

    if( !cap.isOpened() )
    {
        help();
        cout << "***Could not initialize capturing...***\n";
        cout << "Current parameter's value: \n";
        parser.printMessage();
        return -1;
    }
    cout << hot_keys;
    namedWindow( "Histogram", 0 );
    namedWindow( "CamShift Demo", 0 );
    setMouseCallback( "CamShift Demo", onMouse, 0 );
//    createTrackbar( "Vmin", "CamShift Demo", &vmin, 256, 0 );
//    createTrackbar( "Vmax", "CamShift Demo", &vmax, 256, 0 );
//    createTrackbar( "Smin", "CamShift Demo", &smin, 256, 0 );

	Mat frame;
    bool paused = false;
	char path[30] = "I:/image/test/";

	CascadeClassifier face_cascade; bool detect_flag = false;
	Facedetect facedetect;
	bool classifier_flag;

	Tracking tracking;

    for(;;)
    {
        if( !paused )
        {
            cap >> frame;
            if( frame.empty() )
                break;
        }

        frame.copyTo(image);
		
		classifier_flag = face_cascade.load("F:haarcascade_frontalface_alt2.xml");
		if (!classifier_flag)//加载失败返回标志。加载成功是返回true
		{
			std::cout << "load cascade error";
			return -1;
		}

		if (!detect_flag)
		{
			detect_flag = facedetect.detectFaces(frame, face_cascade);
			
			
			if (detect_flag)
			{
				facedetect.SaveFaces(path);//保存检测到的图片
				//selection = facedetect.GetRect()[0];//友元直接访问了
				tracking.Set_trackObj(-1);//
				
			}

		}

		//两种情况分析：1、一张图片上直接检测到两张人脸图片。2、新增或者减少了跟踪目标
		tracking.faceTracking(image,facedetect);
        imshow( "CamShift Demo", image );//上一函数是以引用的方式调用的的
        //imshow( "Histogram", histimg );

		waitKey(10);
    }

    return 0;
}




