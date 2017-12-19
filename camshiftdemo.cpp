#include <windows.h>//��windows.h��������ͷ�ļ�֮��ᱨ��

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

//������һ��windows.hͷ�ļ���λ�þͻᱨ��

//#include <string.h>
//#include <stdlib.h>

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

typedef struct lpthread
{
	Facedetect facedetect;
	Tracking tracking;
	Mat img;

}lpthread;//��ʹ��ȫ�ֱ��������֮����ʹ�ò�������

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

//���ݽ�ȥ��������������һ���ṹ��
//����Ϊȫ�ֱ����ͱ��⴫�ݲ����ˣ�ֱ��ʹ��
DWORD WINAPI ThreadProc2(LPVOID plpthread)//���ݽ��˸����߳�
{//����û�д��ݽ�ȥ
	lpthread *p = (lpthread *)plpthread;
	char path[30] = "I:/image/test/";

	cout << "----------ceshi-----------" << endl;
	//cout << lpThreadParameter << endl;
	CascadeClassifier face_cascade; bool detect_flag = false; bool update_flag = false;
	bool classifier_flag;
	classifier_flag = face_cascade.load("F:haarcascade_frontalface_alt2.xml");
	if (!classifier_flag)//����ʧ�ܷ��ر�־�����سɹ��Ƿ���true
	{
		std::cout << "load cascade error";
		return -1;
	}
	
	vector<Rect> update_faces;
	for (;;)
	{
		//��ѭ����Ĵ����ƶ���tracking����
		if (p->img.data)
		{//���ݽ�������ôƵ����Ӧ�ý��˲��������浽 tracking ���У���Ϊ����ʼ������Ŀ��ʹ��

			if (!detect_flag)//�ü������Ӧ��ȥ��  !detect_flag
			{
				detect_flag = (p->facedetect).detectFaces(p->img, face_cascade);//��ȡ�ٴμ�⵽����������������
				
				vector<Point2f> up_det_point;
				//�ж��Ƿ����µĸ���Ŀ�����ӣ���⵽������������������ٵ�����������������ĶԱ�
				if (detect_flag)
				{//�������ӵĸ���Ŀ�꣬���浽һ��������
					for (size_t i = 0; i < p->facedetect.GetRect_center().size(); i++)
					{
						//��ø��ٵľ��ο�λ�ñ��棬�����������Ӧ
						//p->facedetect.Get_faces()[i];
						//int j;
						for (int j = 0; j < p->tracking.Get_track_point().size(); j++)//��ʼ״̬�����ٵ��������������Ϊ��
						{//�˴β�������һ�����ҵĹ���
							//  <30
							if ( abs((p->facedetect).GetRect_center()[i].x - (p->tracking).Get_track_point()[j].x)<30
								)
							{
								break;
							}//���ҵ����������һ������	

							if (j == p->tracking.Get_track_point().size() - 1)
							{//��ʼ�������ʱ0����������
								up_det_point.push_back((p->facedetect).GetRect_center()[i]);
								(p->facedetect).SaveFaces(path);//�����⵽��ͼƬ

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
					//��ʼ״̬û�н���ڶ���ѭ��
					if (update_faces.size() > 0)//ÿһ�μ��֮�������ģ�һ��һ����
					{//�������������������±�־λ
						p->tracking.Set_update_point(update_faces);//ֱ�ӽ�ԭ���������滻
						(p->tracking).Set_trackObj(-1);
						cout << "hello world" << endl;
						//std::cout << update_faces[0] << std::endl;
					}

					//�����꣬���û����ƥ�������±����־λupdate_flag=true
				}//����Ҫ���ҳ�������һ��û��ƥ�䵽�����ң����⵽�Ķ�Ӧ��
				

				//cout << p->facedetect.GetRect_center() << endl;
				//cout << p->facedetect.GetRect_center().size() << endl;


			}
			imshow("p->img",p->img);
			cv::waitKey(10);//ֻ��waitkey֮����ܹ���ʾͼƬ
			Sleep(2000);
		}


	}
	
	return 0;
}

//����facedetect����������֮ǰ��selection�������һ�θ��٣���ʹ��facedetect��
//�涨������һ�Σ�����ڸ��پ����⵽����������ٳɹ����������ʧ�ܡ�
//�����Ĵ󲿷�����û���ᵽ�����Ҳ��һ�����µ�
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

	//������ʱ�򣬾��Ѿ��г�ʼ����  //���������typedef ��ʹ��new������������ʼ��
	struct lpthread *plpthread=new struct lpthread;//�ṹ��ָ�벻��ֱ��ʹ�õģ�


	
	DWORD lpThreadId2 = 0;
	HANDLE h2 = CreateThread(NULL,0,ThreadProc2, plpthread,0,&lpThreadId2);//���ĸ������Ǵ��ݹ�ȥ�Ĳ�����
	//Ӧ����Ҫ���ݼ������ȥ
	//������������ʹ��Ӧ�õķ�ʽ�������£����������̵߳�ID
	//���������������������̣߳��������̴߳�������������
	//���ĸ����������ݵĲ�����
	//�������������̺߳�������(�̺߳��������봴����ȷ)

    for(;;)//�ڴ�֮ǰ������һ�����̣߳�������ִ�У������
    {
        if( !paused )
        {
			cap >> frame;
			plpthread->img=frame;//ֱ��ʹ�ø�ֵ���ɹ�
			//frame = plpthread->img;
			//frame = imread("I:/image/4.jpg");
            if( frame.empty() )
                break;
        }

        frame.copyTo(image);
		
		

		//�������������1��һ��ͼƬ��ֱ�Ӽ�⵽��������ͼƬ��2���������߼����˸���Ŀ��
		(plpthread->tracking).faceTracking(image,(plpthread->facedetect));//�ú�����������������û��û�м�⵽����Ŀ�꣬�򲻼��
        imshow( "CamShift Demo", image );//��һ�����������õķ�ʽ���õĵ�
        //imshow( "Histogram", histimg );

		cv::waitKey(10);
    }
	CloseHandle(h2);

    return 0;
}








