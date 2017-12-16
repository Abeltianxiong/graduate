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
		rectangle(frame,faces[i],Scalar(255,0,0),1,8);

	}
	if (faces.size())
		return true;
	else
		return false;

}//正常情况下是检测一次保存一次。
//将多次检测的结果放在队列中是最安全的
vector<Rect> Facedetect::GetRect() {
	cout << faces[0] << endl;
	return faces;
}

void Facedetect::FaceHist() {
	Mat roi;//
	vector<Mat> hsvChannels;//使用split函数，参数比mixChannels参数更简单
	int hsize = 16;
	float hranges[] = { 0,180 };
	const float* phranges = hranges;

	cvtColor(img, hsv, COLOR_BGR2HSV);
	inRange(hsv, Scalar(0, 30, MIN(10, 256)), Scalar(180, 256, MAX(10, 256)), mask);//
	Mat maskroi(mask, faces[0]);

	for (size_t i = 0; i < faces.size(); i++)//faces.size()始终为1？。后边的会覆盖，但是不会清空
	{
		trackWindow = faces[0];//第二次计算时，应该使用计算得到的新的位置

		img(faces[i]).copyTo(roi);//图片跟参数不匹配，需要将图片预处理为对应参数的图片
		cvtColor(roi,roi,COLOR_BGR2HSV);//最终转化为的是单通道图像，如果是HSV通道图像，需要分离出H通道图像

		
		split(roi,hsvChannels);//hue
		//imshow("roi", hsvChannels[0]);//需要归一化？
		

//		waitKey(0);
		calcHist(&hsvChannels[0], 1, 0, maskroi, hist, 1, &hsize, &phranges);//计算直方图。只有计算直方图是一次
		normalize(hist, hist, 0, 255, NORM_MINMAX);//归一化
		cout << hist << endl;
		//imshow("hist",hist);
		//直方图计算出错
		//cout << hist << endl;
		//第四个参数掩码的作用？
		
	}
	//Mat maskroi;
	
//	mask = maskroi;
		
}

void Facedetect::BackHist()
{
	//计算的是整幅图像上的反向投影
	Mat imgHSV;
	float histR[] = {0,255};
	const float *phranges = histR;
	vector<Mat> hsvChannels;

	cvtColor(img,imgHSV,COLOR_BGR2HSV);
	split(imgHSV, hsvChannels);

	calcBackProject(&hsvChannels[0], 1, 0, hist, backproj, &phranges);
	imshow("backproj",backproj);

	backproj &= mask;//不仅是维度，还要确保大小一样。
}

void Facedetect::CAMSHIFT() {
	//反向投影，使用mask操作下

	//若跟踪区域受影响扩大，则与HSV的灰度图像与操作？
	RotatedRect trackBox = CamShift(backproj, trackWindow,TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

	
	ellipse(img, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
	imshow("img",img);
}

void Facedetect::SaveFaces(char* con) {//con是图片需要保存的目录
	char co[100];
	char jpg[] = ".jpg";
	
	Mat ROI_img;

	for (size_t i = 0; i < faces.size(); i++)//此循环，包含了检测到人脸的条件
	{
		img(faces[i]).copyTo(ROI_img);
		sprintf(co, "%s%d%s", con, m, jpg);//string和char的操作不能够混用
		m++;
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
}