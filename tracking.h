
#ifndef FACEDETECT_H
#define FACEDETECT_H
#include "facedetect.h"

#endif // !FACEDETECT_H


#include "opencv2/opencv.hpp"
using namespace cv;


class Tracking {
public:
	Tracking() { trackObject = 0; }//���캯��
	void faceTracking(Mat&);
	void faceTracking(Mat&,Facedetect&);
	int Get_trackObj();
	void Set_trackObj(int);

private:
	int trackObject;
	Rect trackWindow;//ʼ��ʹ�õ����ǵ�һ�μ�⵽�Ĵ���
	Mat hist;//ֱ��ͼֻ����һ�Σ����ܹ��������¶���
	friend class Facedetect;


};