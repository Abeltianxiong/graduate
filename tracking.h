#ifndef FACEDETECT_H
#define FACEDETECT_H
#include "facedetect.h"

#endif // !FACEDETECT_H

#include <vector>

#include "opencv2/opencv.hpp"
using namespace cv;

class Tracking {
public:
	Tracking() { trackObject = 0; hist_flag = 0; m = 0; }//���캯��

	void faceTracking(Mat&, Facedetect&);

	bool UpdateTracking(Facedetect&);//����⵽��ÿһ֡����ͼƬ���룬�˺����ж��Ƿ�����µĸ��ٶ��󣬻�����ɾ�����ٶ���
	//void SetUpdateWin();
	void Set_trackObj(int);

	void SaveFaces(Mat& img ,char* con);

private:
	int m;
	int trackObject;
	int hist_flag;
	Mat hist;

	vector<Rect> trackWindow;
	vector<Rect> updateWindow;//���´��ڵ�ֵͨ��updatetracking���
	friend class Facedetect;

};