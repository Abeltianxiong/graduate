#include "opencv2/opencv.hpp"

#include <ctype.h>
#include <iostream>

using namespace cv;
using namespace std;

//每个常用的功能函数以文件的形式保存避免下次使用的时候重新编写
class Facedetect {//可以在析构函数里边进行内存的释放
public:
	//构造函数
	Facedetect() :face_cascade(),m(0) {}

	bool detectFaces(cv::Mat& frame, CascadeClassifier&);
	void SaveFaces(char*);//使用char类型数组的数组名即可
	vector<Rect> GetRect();

	void FaceHist();//计算检测得到的人脸的直方图
	void BackHist(Mat);

	void trans_img(Mat);
	void CAMSHIFT();
	
	CascadeClassifier GetClassifier();

	//friend void Tracking::faceTracking(Mat& image,Facedetect&);
private:
	int m;//记录保存图片的个数
	Mat hue;
	Mat hist;//该类的其他函数，也会使用到直方图
	Mat backproj;
	Mat mask;
	Mat hsv;
	vector<Rect> faces;
	Rect trackWindow;
	Mat img;//检测之后，用于保存检测的那张图片
	CascadeClassifier face_cascade;//使用该类创建对象之后，可以使用该类中的对象

	friend class Tracking;
//为何作为类的成员函数的时候不能够正常使用
};//类中属性都是设置为私有，类中的方法是设置为public
//如果将类定义成为友元函数，可以访问友元类中的私有元素


