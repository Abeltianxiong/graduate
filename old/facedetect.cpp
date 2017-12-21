#include "facedetect.h"

//ͷ�ļ���߷�װ�Ĺ��ܺ����õ�����ʵ��

//���������
bool Facedetect::detectFaces(cv::Mat& frame, CascadeClassifier& face_cascade) {//�ڶ���ִ�и÷�����ʱ�򣬱�����������
	Mat frame_gray;
	//;//�����⵽����������
	frame.copyTo(img);//�����ڼ���ͼƬ���Ƶ�img�У�����������������

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0, Size(80, 80));//
	//���Ƿ�ʽ����ͷ��ʼд��

	for (size_t i = 0; i < faces.size(); i++)
	{
		rectangle(frame,faces[i],Scalar(255,0,0),3,8);
		cout << faces[i] << endl;

	}
	
	if (faces.size())
		return true;
	else
		return false;

}//����������Ǽ��һ�α���һ�Ρ�
//����μ��Ľ�����ڶ��������ȫ��
vector<Point2f> Facedetect::GetRect_center() {
	vector<Point2f> face_center;
	Point2f center;
	for (size_t i = 0; i < faces.size(); i++)
	{
		//cout << faces[i] << endl;
		//���ҵõ���������
		center.x = faces[i].x + faces[i].width / 2;
		center.y = faces[i].y + faces[i].height / 2;
		face_center.push_back(center);
	}

	
	return face_center;
}//��ⴰ�ڵ�ֵҲ��û���滻�ġ�

vector<Rect> Facedetect::Get_faces() {
	return faces;
}



void Facedetect::BackHist(Mat image)
{
	//�����������ͼ���ϵķ���ͶӰ
/*
	Mat imgHSV;
	float histR[] = {0,255};
	const float *phranges = histR;
	vector<Mat> hsvChannels;

	
	cvtColor(image,imgHSV,COLOR_BGR2HSV);
	split(imgHSV, hsvChannels);//��ȡHͨ��ͼ�����ظ�����
	inRange(imgHSV, Scalar(0, 30, 10), Scalar(180, 256, 256), mask);//�˲�Ҳֻ�����һ��

	calcBackProject(&hsvChannels[0], 1, 0, hist, backproj, &phranges);//û��ʼ�ո���img
	backproj &= mask;
	imshow("backproj",backproj);
*/
	//������ά�ȣ���Ҫȷ����Сһ����
}

void Facedetect::CAMSHIFT() {
	//����ͶӰ��ʹ��mask������
	
	//������������Ӱ����������HSV�ĻҶ�ͼ���������

#if 0
	RotatedRect trackBox = CamShift(backproj, trackWindow, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));


	ellipse(img, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
	imshow("img", img);

#endif // 0


}

void Facedetect::SaveFaces(char* con) {//con��ͼƬ��Ҫ�����Ŀ¼
	char co[100];
	char jpg[] = ".jpg";
	
	Mat ROI_img;

	for (size_t i = 0; i < faces.size(); i++)//��ѭ���������˼�⵽����������
	{
		img(faces[i]).copyTo(ROI_img);
		sprintf(co, "%s%d%s", con, m, jpg);//string��char�Ĳ������ܹ�����
		m++;//����Զ�������
		imwrite(co, ROI_img);
		cout << co << endl;
		cout << "save face true" << endl;
	}

	//�����֮ǰ����ȡvector��ߵ����ݣ�Ȼ���������
	
}

//�ñ���Ҳ����main�ж��壬Ȼ��ʹ�ò������ݽ���
CascadeClassifier Facedetect::GetClassifier() {//�����ռ�

	cout << faces.size() << endl;
	return face_cascade;
}

void Facedetect::trans_img(Mat frame) {
	img = frame;
}//���������ɷ֣��Ͳ�Ҫ�ֵ�̫ϸ��


//�߼�̫�����ˣ����ǵ��޸�