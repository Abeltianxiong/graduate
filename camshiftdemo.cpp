#include <windows.h>//将windows.h放在其他头文件之后会报错

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

//与以上一样windows.h头文件的位置就会报错

//#include <string.h>
//#include <stdlib.h>

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

typedef struct lpthread
{
	Facedetect facedetect;
	Tracking tracking;
	Mat img;

}lpthread;//先使用全局变量，完成之后，再使用参数传递

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

//传递进去两个参数，或者一个结构体
//定义为全局变量就避免传递参数了，直接使用
DWORD WINAPI ThreadProc2(LPVOID plpthread)//传递近了跟踪线程
{//参数没有传递进去
	lpthread *p = (lpthread *)plpthread;
	char path[30] = "I:/image/test/";

	cout << "----------ceshi-----------" << endl;
	//cout << lpThreadParameter << endl;
	CascadeClassifier face_cascade; bool detect_flag = false; bool update_flag = false;
	bool classifier_flag;
	classifier_flag = face_cascade.load("F:haarcascade_frontalface_alt2.xml");
	if (!classifier_flag)//加载失败返回标志。加载成功是返回true
	{
		std::cout << "load cascade error";
		return -1;
	}
	
	vector<Rect> update_faces;
	for (;;)
	{
		//将循环类的代码移动到tracking类中
		if (p->img.data)
		{//数据交互，这么频繁，应该讲此操作，保存到 tracking 类中，作为，初始化跟踪目标使用

			if (!detect_flag)//该检测条件应该去掉  !detect_flag
			{
				detect_flag = (p->facedetect).detectFaces(p->img, face_cascade);//获取再次检测到的人脸的中心坐标
				
				vector<Point2f> up_det_point;
				//判断是否有新的跟踪目标增加，检测到的人脸中心坐标如跟踪到的人脸的中心坐标的对比
				if (detect_flag)
				{//将新增加的跟踪目标，保存到一个容器中
					for (size_t i = 0; i < p->facedetect.GetRect_center().size(); i++)
					{
						//获得跟踪的矩形框位置保存，与中心坐标对应
						//p->facedetect.Get_faces()[i];
						//int j;
						for (int j = 0; j < p->tracking.Get_track_point().size(); j++)//初始状态，跟踪的中心坐标的容器为空
						{//此次操作，是一个查找的功能
							//  <30
							if ( abs((p->facedetect).GetRect_center()[i].x - (p->tracking).Get_track_point()[j].x)<30
								)
							{
								break;
							}//查找到，则查找下一个检测点	

							if (j == p->tracking.Get_track_point().size() - 1)
							{//初始情况跟踪时0不会进入查找
								up_det_point.push_back((p->facedetect).GetRect_center()[i]);
								(p->facedetect).SaveFaces(path);//保存检测到的图片

								cout << "" << p->facedetect.Get_faces()[i] << endl;
								update_faces.push_back(p->facedetect.Get_faces()[i]);
							}
								
						}
						if (p->tracking.Get_track_point().size() == 0)
						{
							cout << "the face rect: " << p->facedetect.Get_faces()[i] << endl;
							update_faces.push_back(p->facedetect.Get_faces()[i]);
						}
						

					}
					//初始状态没有进入第二层循环
					if (update_faces.size() > 0)//每一次检测之后新增的，一般一两个
					{//保存新增的人脸，更新标志位
						p->tracking.Set_update_point(update_faces);//直接将原来的容器替换
						(p->tracking).Set_trackObj(-1);
						cout << "hello world" << endl;
						//std::cout << update_faces[0] << std::endl;
					}

					//查找完，如果没有相匹配的则跟新保存标志位update_flag=true
				}//还需要查找出，是哪一个没有匹配到，并且，与检测到的对应上
				

				//cout << p->facedetect.GetRect_center() << endl;
				//cout << p->facedetect.GetRect_center().size() << endl;


			}
			imshow("p->img",p->img);
			cv::waitKey(10);//只有waitkey之后才能够显示图片
			Sleep(2000);
		}


	}
	
	return 0;
}

//加入facedetect函数，代替之前的selection。在完成一次跟踪，就使用facedetect在
//规定区域检测一次，如果在跟踪局域检测到人脸，则跟踪成功，否则跟踪失败。
//看到的大部分论文没有提到这个。也算一个创新点
int main( int argc, const char** argv )
{
    VideoCapture cap;
  
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
    cap.open(camNum);//cap.open(0)?

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
	

	

	//Facedetect *facedetect=new Facedetect;
	//Tracking *tracking=new Tracking;

	//创建的时候，就已经有初始化了  //如果不适用typedef 则使用new创建对象，来初始化
	struct lpthread *plpthread=new struct lpthread;//结构体指针不是直接使用的？


	
	DWORD lpThreadId2 = 0;
	HANDLE h2 = CreateThread(NULL,0,ThreadProc2, plpthread,0,&lpThreadId2);//第四个参数是传递过去的参数？
	//应该需要传递检测器进去
	//第六个参数，使用应用的方式，保存下，创建的新线程的ID
	//第五个参数，创建挂起的线程，或者是线程创建后立即激活
	//第四个参数，传递的参数，
	//第三个参数，线程函数名称(线程函数，必须创建正确)

    for(;;)//在此之前，创建一个子线程，周期性执行，检测器
    {
        if( !paused )
        {
			cap >> frame;
			plpthread->img=frame;//直接使用赋值不成功
			//frame = plpthread->img;
			//frame = imread("I:/image/4.jpg");
            if( frame.empty() )
                break;
        }

        frame.copyTo(image);
		
		

		//两种情况分析：1、一张图片上直接检测到两张人脸图片。2、新增或者减少了跟踪目标
		(plpthread->tracking).faceTracking(image,(plpthread->facedetect));//该函数里边有条件，如果没有没有检测到跟踪目标，则不检测
        imshow( "CamShift Demo", image );//上一函数是以引用的方式调用的的
        //imshow( "Histogram", histimg );

		cv::waitKey(10);
    }
	CloseHandle(h2);

    return 0;
}








