
#ifndef FACEDETECT_H
#define FACEDETECT_H
#include "facedetect.h"

#endif // !FACEDETECT_H

#include <vector>

#include "opencv2/opencv.hpp"
using namespace cv;


class Tracking {
public:
	Tracking() { trackObject = 0; }//构造函数
	void faceTracking(Mat&);
	void faceTracking(Mat&,Facedetect&);
	int Get_trackObj();
	void Set_trackObj(int);
	void Set_update_point(vector<Rect>);//保存了位置就好，不需要知道是第几个
	vector<Point2f> Get_track_point();

private:
	int trackObject;
	//所有同时跟踪的法跟踪窗口
	vector<Rect> trackWindow;//始终使用到的是第一次检测到的窗口//Rect
	vector<Rect> updateWindow;//新增的跟踪目标

	//Rect trackWindow;
	Mat hist;//直方图只计算一次，不能够反复重新定义
	vector<Point2f> trac_point;

	friend class Facedetect;


};