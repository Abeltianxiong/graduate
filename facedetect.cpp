#include "facedetect.h"

bool Facedetect::detectFaces(cv::Mat& frame, CascadeClassifier& face_cascade) 
{
//	Mat frame_gray;//����������嵽�����ڲ������¶����ʱ�򣬻����
	

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//��faces���嵽���У�ʹ�ö�Ƕȼ�⺯��֮��Ҳ�Ὣfaces���������
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0, Size(80, 80));//Ҳ�����õķ�ʽ�����û�м�⵽faces��

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
