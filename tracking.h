
#ifndef FACEDETECT_H
#define FACEDETECT_H
#include "facedetect.h"

#endif // !FACEDETECT_H


#include "opencv2/opencv.hpp"
using namespace cv;


class Tracking {
public:
	Tracking() { trackObject = 0; }//构造函数
	void faceTracking(Mat&);
	void faceTracking(Mat&,Facedetect&);
	int Get_trackObj();
	void Set_trackObj(int);

private:
	int trackObject;
	Rect trackWindow;//始终使用到的是第一次检测到的窗口
	Mat hist;//直方图只计算一次，不能够反复重新定义
	friend class Facedetect;


};