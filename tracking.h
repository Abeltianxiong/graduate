
#ifndef FACEDETECT_H
#define FACEDETECT_H
#include "facedetect.h"

#endif // !FACEDETECT_H

#include <vector>

#include "opencv2/opencv.hpp"
using namespace cv;


class Tracking {
public:
	Tracking() { trackObject = 0; }//���캯��
	void faceTracking(Mat&);
	void faceTracking(Mat&,Facedetect&);
	int Get_trackObj();
	void Set_trackObj(int);
	void Set_update_point(vector<Rect>);//������λ�þͺã�����Ҫ֪���ǵڼ���
	vector<Point2f> Get_track_point();

private:
	int trackObject;
	//����ͬʱ���ٵķ����ٴ���
	vector<Rect> trackWindow;//ʼ��ʹ�õ����ǵ�һ�μ�⵽�Ĵ���//Rect
	vector<Rect> updateWindow;//�����ĸ���Ŀ��

	//Rect trackWindow;
	Mat hist;//ֱ��ͼֻ����һ�Σ����ܹ��������¶���
	vector<Point2f> trac_point;

	friend class Facedetect;


};