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
		rectangle(frame,faces[i],Scalar(255,0,0),1,8);

	}
	if (faces.size())
		return true;
	else
		return false;

}//����������Ǽ��һ�α���һ�Ρ�
//����μ��Ľ�����ڶ��������ȫ��
vector<Rect> Facedetect::GetRect() {
	cout << faces[0] << endl;
	return faces;
}

void Facedetect::FaceHist() {
	Mat roi;//
	vector<Mat> hsvChannels;//ʹ��split������������mixChannels��������
	int hsize = 16;
	float hranges[] = { 0,180 };
	const float* phranges = hranges;

	cvtColor(img, hsv, COLOR_BGR2HSV);
	inRange(hsv, Scalar(0, 30, MIN(10, 256)), Scalar(180, 256, MAX(10, 256)), mask);//
	Mat maskroi(mask, faces[0]);

	for (size_t i = 0; i < faces.size(); i++)//faces.size()ʼ��Ϊ1������ߵĻḲ�ǣ����ǲ������
	{
		trackWindow = faces[0];//�ڶ��μ���ʱ��Ӧ��ʹ�ü���õ����µ�λ��

		img(faces[i]).copyTo(roi);//ͼƬ��������ƥ�䣬��Ҫ��ͼƬԤ����Ϊ��Ӧ������ͼƬ
		cvtColor(roi,roi,COLOR_BGR2HSV);//����ת��Ϊ���ǵ�ͨ��ͼ�������HSVͨ��ͼ����Ҫ�����Hͨ��ͼ��

		
		split(roi,hsvChannels);//hue
		//imshow("roi", hsvChannels[0]);//��Ҫ��һ����
		

//		waitKey(0);
		calcHist(&hsvChannels[0], 1, 0, maskroi, hist, 1, &hsize, &phranges);//����ֱ��ͼ��ֻ�м���ֱ��ͼ��һ��
		normalize(hist, hist, 0, 255, NORM_MINMAX);//��һ��
		cout << hist << endl;
		//imshow("hist",hist);
		//ֱ��ͼ�������
		//cout << hist << endl;
		//���ĸ�������������ã�
		
	}
	//Mat maskroi;
	
//	mask = maskroi;
		
}

void Facedetect::BackHist()
{
	//�����������ͼ���ϵķ���ͶӰ
	Mat imgHSV;
	float histR[] = {0,255};
	const float *phranges = histR;
	vector<Mat> hsvChannels;

	cvtColor(img,imgHSV,COLOR_BGR2HSV);
	split(imgHSV, hsvChannels);

	calcBackProject(&hsvChannels[0], 1, 0, hist, backproj, &phranges);
	imshow("backproj",backproj);

	backproj &= mask;//������ά�ȣ���Ҫȷ����Сһ����
}

void Facedetect::CAMSHIFT() {
	//����ͶӰ��ʹ��mask������

	//������������Ӱ����������HSV�ĻҶ�ͼ���������
	RotatedRect trackBox = CamShift(backproj, trackWindow,TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

	
	ellipse(img, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
	imshow("img",img);
}

void Facedetect::SaveFaces(char* con) {//con��ͼƬ��Ҫ�����Ŀ¼
	char co[100];
	char jpg[] = ".jpg";
	
	Mat ROI_img;

	for (size_t i = 0; i < faces.size(); i++)//��ѭ���������˼�⵽����������
	{
		img(faces[i]).copyTo(ROI_img);
		sprintf(co, "%s%d%s", con, m, jpg);//string��char�Ĳ������ܹ�����
		m++;
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
}