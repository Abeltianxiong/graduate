#ifndef FACEDETECT_H
#define FACEDETECT_H
#include "facedetect.h"

#endif // !FACEDETECT_H

#include <vector>

#include "opencv2/opencv.hpp"
using namespace cv;

class Tracking {
public:
	Tracking() { trackObject = 0; hist_flag = 0; m = 0; }//构造函数

	void faceTracking(Mat&, Facedetect&);

	bool UpdateTracking(Facedetect&);//将检测到的每一帧人脸图片存入，此函数判断是否添加新的跟踪对象，或者是删除跟踪对象
	//void SetUpdateWin();
	void Set_trackObj(int);

	void SaveFaces(Mat& img ,char* con);

private:
	int m;
	int trackObject;
	int hist_flag;
	Mat hist;

	vector<Rect> trackWindow;
	vector<Rect> updateWindow;//更新窗口的值通过updatetracking获得
	friend class Facedetect;

};