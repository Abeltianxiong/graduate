#include "facedetect.h"

//头文件里边封装的功能函数得到具体实现

//多人脸检测
bool Facedetect::detectFaces(cv::Mat& frame, CascadeClassifier& face_cascade) {//第二次执行该方法的时候，变量不会重置
	Mat frame_gray;
	//;//保存检测到的人脸坐标
	frame.copyTo(img);//将用于检测的图片复制到img中，方便其他方法访问

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0, Size(80, 80));//
	//覆盖方式，从头开始写。

	for (size_t i = 0; i < faces.size(); i++)
	{
		rectangle(frame,faces[i],Scalar(255,0,0),3,8);
		cout << faces[i] << endl;

	}
	
	if (faces.size())
		return true;
	else
		return false;

}//正常情况下是检测一次保存一次。
//将多次检测的结果放在队列中是最安全的
vector<Point2f> Facedetect::GetRect_center() {
	vector<Point2f> face_center;
	Point2f center;
	for (size_t i = 0; i < faces.size(); i++)
	{
		//cout << faces[i] << endl;
		//并且得到中心坐标
		center.x = faces[i].x + faces[i].width / 2;
		center.y = faces[i].y + faces[i].height / 2;
		face_center.push_back(center);
	}

	
	return face_center;
}//检测窗口的值也是没有替换的。

vector<Rect> Facedetect::Get_faces() {
	return faces;
}



void Facedetect::BackHist(Mat image)
{
	//计算的是整幅图像上的反向投影
/*
	Mat imgHSV;
	float histR[] = {0,255};
	const float *phranges = histR;
	vector<Mat> hsvChannels;

	
	cvtColor(image,imgHSV,COLOR_BGR2HSV);
	split(imgHSV, hsvChannels);//提取H通道图像有重复操作
	inRange(imgHSV, Scalar(0, 30, 10), Scalar(180, 256, 256), mask);//滤波也只计算过一次

	calcBackProject(&hsvChannels[0], 1, 0, hist, backproj, &phranges);//没有始终更新img
	backproj &= mask;
	imshow("backproj",backproj);
*/
	//不仅是维度，还要确保大小一样。
}

void Facedetect::CAMSHIFT() {
	//反向投影，使用mask操作下
	
	//若跟踪区域受影响扩大，则与HSV的灰度图像与操作？

#if 0
	RotatedRect trackBox = CamShift(backproj, trackWindow, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));


	ellipse(img, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
	imshow("img", img);

#endif // 0


}

void Facedetect::SaveFaces(char* con) {//con是图片需要保存的目录
	char co[100];
	char jpg[] = ".jpg";
	
	Mat ROI_img;

	for (size_t i = 0; i < faces.size(); i++)//此循环，包含了检测到人脸的条件
	{
		img(faces[i]).copyTo(ROI_img);
		sprintf(co, "%s%d%s", con, m, jpg);//string和char的操作不能够混用
		m++;//编号自动更新了
		imwrite(co, ROI_img);
		cout << co << endl;
		cout << "save face true" << endl;
	}

	//保存的之前，读取vector里边的数据，然后将数据清除
	
}

//该变量也可在main中定义，然后使用参数传递进来
CascadeClassifier Facedetect::GetClassifier() {//命名空间

	cout << faces.size() << endl;
	return face_cascade;
}

void Facedetect::trans_img(Mat frame) {
	img = frame;
}//函数若不可分，就不要分的太细了


//逻辑太混乱了，还是得修改