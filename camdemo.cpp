#include <windows.h>
#include "all.h"


#if 0
typedef struct lpthread
{
	Facedetect facedetect;
	//Tracking tracking;
	Mat img;

}lpthread;//先使用全局变量，完成之后，再使用参数传递  
#endif // 0

Facedetect facedetect;
Tracking tracking;

#if 0
DWORD WINAPI ThreadProc2(LPVOID plpthread) {//不适用参数传递，使用全局变量

//lpthread *p = (lpthread *)plpthread;  周期性执行跟踪器，跟踪器里边会检测是否初始化了跟踪器


	return 0;
}
#endif // 0


int main()
{
	VideoCapture cap(0);
	Mat frame;

	
	
	//主线程是检测器，子线程是跟踪器

	//struct lpthread *plpthread = new struct lpthread;
//	DWORD lpThreadId2 = 0;
//	HANDLE h2 = CreateThread(NULL, 0, ThreadProc2, NULL, 0, &lpThreadId2);//plpthread


	CascadeClassifier face_cascade; bool detect_flag = false;
	bool classifier_flag;
	classifier_flag = face_cascade.load("F:haarcascade_frontalface_alt2.xml");
	if (!classifier_flag)//加载失败返回标志。加载成功是返回true
	{
		std::cout << "load cascade error";
		return -1;
	}

	while (true) {
		bool flag = cap.read(frame);//读图片的速度款系到跟踪的效果？
		if (!flag)
			break;//不是continue
		if(!detect_flag)
			detect_flag=facedetect.detectFaces(frame, face_cascade);

		if (detect_flag) {//判断是否需要更新检测器
			
			bool flag= tracking.UpdateTracking(facedetect);//检测一次之后，判断是否需要更新updatewindows
			cout << flag << endl;
			if (flag)
			{
				tracking.Set_trackObj(-1);//需要更新跟踪窗口的信息
				tracking.SaveFaces(frame, "I:/image/test/");
			}
				
			detect_flag = false;
		}//假设先不适用多线程，检测之后就跟踪，并且，判断是否更新跟踪

		tracking.faceTracking(frame,facedetect);//无论是否更新跟踪窗口，都需要执行跟踪器
		
	}//trackwindows没有更新
	

	return 0;
}