#include "facedetect.h"

bool Facedetect::detectFaces(cv::Mat& frame, CascadeClassifier& face_cascade) 
{
//	Mat frame_gray;//如果人脸定义到函数内部，重新定义的时候，会清空
	

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//将faces定义到类中，使用多角度检测函数之后，也会将faces容器中清空
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0, Size(80, 80));//也是引用的方式，如果没有检测到faces空

	for (size_t i = 0; i < faces.size(); i++)
	{
		rectangle(frame, faces[i], Scalar(255, 0, 0), 3, 8);
		cout << faces[i] << endl;

	}
	//imshow("frame", frame);
	waitKey(30);

	if (faces.size())
		return true;
	else
		return false;
}
