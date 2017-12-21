#include "opencv2/opencv.hpp"

#include <ctype.h>
#include <iostream>

using namespace cv;
using namespace std;

class Facedetect {
public:
	Facedetect() :m(0){ };//¹¹Ôìº¯Êý

	bool detectFaces(cv::Mat& frame, CascadeClassifier&);

private:
	int m;
	vector<Rect> faces;

	friend class Tracking;
};