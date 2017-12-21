#include <windows.h>
#include "all.h"


#if 0
typedef struct lpthread
{
	Facedetect facedetect;
	//Tracking tracking;
	Mat img;

}lpthread;//��ʹ��ȫ�ֱ��������֮����ʹ�ò�������  
#endif // 0

Facedetect facedetect;
Tracking tracking;

#if 0
DWORD WINAPI ThreadProc2(LPVOID plpthread) {//�����ò������ݣ�ʹ��ȫ�ֱ���

//lpthread *p = (lpthread *)plpthread;  ������ִ�и���������������߻����Ƿ��ʼ���˸�����


	return 0;
}
#endif // 0


int main()
{
	VideoCapture cap(0);
	Mat frame;

	
	
	//���߳��Ǽ���������߳��Ǹ�����

	//struct lpthread *plpthread = new struct lpthread;
//	DWORD lpThreadId2 = 0;
//	HANDLE h2 = CreateThread(NULL, 0, ThreadProc2, NULL, 0, &lpThreadId2);//plpthread


	CascadeClassifier face_cascade; bool detect_flag = false;
	bool classifier_flag;
	classifier_flag = face_cascade.load("F:haarcascade_frontalface_alt2.xml");
	if (!classifier_flag)//����ʧ�ܷ��ر�־�����سɹ��Ƿ���true
	{
		std::cout << "load cascade error";
		return -1;
	}

	while (true) {
		bool flag = cap.read(frame);//��ͼƬ���ٶȿ�ϵ�����ٵ�Ч����
		if (!flag)
			break;//����continue
		if(!detect_flag)
			detect_flag=facedetect.detectFaces(frame, face_cascade);

		if (detect_flag) {//�ж��Ƿ���Ҫ���¼����
			
			bool flag= tracking.UpdateTracking(facedetect);//���һ��֮���ж��Ƿ���Ҫ����updatewindows
			cout << flag << endl;
			if (flag)
			{
				tracking.Set_trackObj(-1);//��Ҫ���¸��ٴ��ڵ���Ϣ
				tracking.SaveFaces(frame, "I:/image/test/");
			}
				
			detect_flag = false;
		}//�����Ȳ����ö��̣߳����֮��͸��٣����ң��ж��Ƿ���¸���

		tracking.faceTracking(frame,facedetect);//�����Ƿ���¸��ٴ��ڣ�����Ҫִ�и�����
		
	}//trackwindowsû�и���
	

	return 0;
}