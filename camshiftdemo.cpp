#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "facedetect.h"

#include <iostream>
#include <ctype.h>

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
    createTrackbar( "Vmin", "CamShift Demo", &vmin, 256, 0 );
    createTrackbar( "Vmax", "CamShift Demo", &vmax, 256, 0 );
    createTrackbar( "Smin", "CamShift Demo", &smin, 256, 0 );

    Mat frame, hsv, hue, mask, hist, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;
    bool paused = false;

	CascadeClassifier face_cascade; bool detect_flag = false;
	Facedetect facedetect;
	bool classifier_flag;

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
				selection = facedetect.GetRect()[0];
				trackObject = -1;
			}

		}

        if( !paused )//初始状态是false，初始状态暂停键没有按下
        {
            cvtColor(image, hsv, COLOR_BGR2HSV);//转换成为HSV图像

			if (trackObject)// 检测到人脸，以及跟踪的时候计算
			{//-1条件为真？
				int _vmin = vmin, _vmax = vmax;

				inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)),
					Scalar(180, 256, MAX(_vmin, _vmax)), mask);//二值化？ HSV图像转换成为了二值图像？
			//将三通道图像二值化？  mask保存范围内的h分量

				int ch[] = { 0, 0 };//赋值通道？
				hue.create(hsv.size(), hsv.depth());//图像的初始化  ??等同于赋值hsv图像了？
				mixChannels(&hsv, 1, &hue, 1, ch, 1);// fromTo参数设置   split可将通道分离
				//将hsv第一个通道(H通道)的数赋值到hue中

				//区域选中的那一次执行该条件语句 选中完之后trackObject=1
				if (trackObject < 0)//选中区域之后，只会计算一次。该标记，检测之后计算
				{//是图像复制
					// Object has been selected by user, set up CAMShift search properties once
					Mat roi(hue, selection), maskroi(mask, selection);//roi 分离出来的H通道，mask分离之前转化为HSV通道二值图像
					calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);//单通道直方图
					cout << hist << endl;//
					normalize(hist, hist, 0, 255, NORM_MINMAX);//16个hist,每个最大到像素点的个数
					cout << hist << endl;//将统计得到的数量，归一化到0-255之间

					trackWindow = selection;
					trackObject = 1; // Don't set up again, unless user selects new ROI

					histimg = Scalar::all(0);
					int binW = histimg.cols / hsize;
					Mat buf(1, hsize, CV_8UC3);
					for (int i = 0; i < hsize; i++)
						buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);
					cvtColor(buf, buf, COLOR_HSV2BGR);

					for (int i = 0; i < hsize; i++)
					{
						int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);
						rectangle(histimg, Point(i*binW, histimg.rows),
							Point((i + 1)*binW, histimg.rows - val),
							Scalar(buf.at<Vec3b>(i)), -1, 8);
					}
				}//鼠标选中的时候，计算一次直方图(可以设置为，计算检测到的那张人脸的直方图)

				// Perform CAMShift  计算的时候，使用到的一直是第一次计算得到的直方图
				calcBackProject(&hue, 1, 0, hist, backproj, &phranges);//计算直方图反向投影(可理解为与计算直方图的方式相反)
				backproj &= mask;//与操作之后，表面受到周围区域的影响...三通道跟单通道图计算？？？
				RotatedRect trackBox = CamShift(backproj, trackWindow,
					TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				//跟踪窗口不变，一直改变的是跟踪区域
				//可以使用两种判断依据，运动目标到达视频边缘+小于最小窗体 则判断跟踪目标消失
				if (trackBox.center.x<100 && trackBox.center.y<100)//参数包括矩形中心，边长，旋转角度
				{
					cout << "the objective lossing" << endl;
					trackObject = 0;//detect_flag 打开检测到标志
					detect_flag = false;
				}

#if 0
				if (trackWindow.area() <= 1)
				{
					int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
					trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
						trackWindow.x + r, trackWindow.y + r) &
						Rect(0, 0, cols, rows);
				}
#endif // 0


                //if( backprojMode )//如果顺利进入反向投影，将灰度图像转换为BGR图像
                    //cvtColor( backproj, image, COLOR_GRAY2BGR );//以区别选中的矩形区域
                ellipse( image, trackBox, Scalar(0,0,255), 3, LINE_AA );//trackBox计算得到的跟踪区域
            }//跟踪的时候以椭圆代表跟踪目标
        }
        else if( trackObject < 0 )//追踪中(跟踪目标的数目)
            paused = false;//撤销暂停键

#if 1
		if (selectObject && selection.width > 0 && selection.height > 0)//只有在鼠标左键按下到松开的工程中才会执行
		{//何时改变selectObject的？
			Mat roi(image, selection);
			bitwise_not(roi, roi);//每个像素取非操作
		}//roi ？？效果是显示了鼠标的划动范围
#endif // 1


        imshow( "CamShift Demo", image );
        imshow( "Histogram", histimg );

        char c = (char)waitKey(10);
        if( c == 27 )
            break;
        switch(c)
        {
        case 'b':
            backprojMode = !backprojMode;
            break;
        case 'c':
            trackObject = 0;
            histimg = Scalar::all(0);
            break;
        case 'h':
            showHist = !showHist;
            if( !showHist )
                destroyWindow( "Histogram" );
            else
                namedWindow( "Histogram", 1 );
            break;
        case 'p':
            paused = !paused;
            break;
        default:
            ;
        }
    }

    return 0;
}
