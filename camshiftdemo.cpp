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
#include "tracking.h"//����ʹ�õ�ʱ��ᶨ��facedetect.hͷ�ļ������ε��õ�ʱ�򣬲��ᶨ���ͷ�ļ�
//ͷ�ļ���˳��

using namespace cv;
using namespace std;

Mat image;

bool backprojMode = false;//��ʾ�Ƿ���뷴��ͶӰģʽ��true��ʾ���뷴��ͶӰģʽ
bool selectObject = false;//��ʾ�Ƿ���ѡ����ٵĳ�ʼĿ��
int trackObject = 0;//�������Ŀ�������
bool showHist = true;//�Ƿ���ʾֱ��ͼ
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
//������ʱ�䣬Ҳ��ʹ���˽����л���

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
//����facedetect����������֮ǰ��selection�������һ�θ��٣���ʹ��facedetect��
//�涨������һ�Σ�����ڸ��پ����⵽����������ٳɹ����������ʧ�ܡ�
//�����Ĵ󲿷�����û���ᵽ�����Ҳ��һ�����µ�
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
		if (!classifier_flag)//����ʧ�ܷ��ر�־�����سɹ��Ƿ���true
		{
			std::cout << "load cascade error";
			return -1;
		}

		if (!detect_flag)
		{
			detect_flag = facedetect.detectFaces(frame, face_cascade);
			
			
			if (detect_flag)
			{
				facedetect.SaveFaces(path);//�����⵽��ͼƬ
				//selection = facedetect.GetRect()[0];//��Ԫֱ�ӷ�����
				tracking.Set_trackObj(-1);//
				
			}

		}

		//�������������1��һ��ͼƬ��ֱ�Ӽ�⵽��������ͼƬ��2���������߼����˸���Ŀ��
		tracking.faceTracking(image,facedetect);
        imshow( "CamShift Demo", image );//��һ�����������õķ�ʽ���õĵ�
        //imshow( "Histogram", histimg );

		waitKey(10);
    }

    return 0;
}




