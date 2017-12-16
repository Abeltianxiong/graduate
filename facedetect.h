#include "opencv2/opencv.hpp"

#include <ctype.h>
#include <iostream>

using namespace cv;
using namespace std;

//ÿ�����õĹ��ܺ������ļ�����ʽ��������´�ʹ�õ�ʱ�����±�д
class Facedetect {//����������������߽����ڴ���ͷ�
public:
	//���캯��
	Facedetect() :face_cascade(),m(0) {}

	bool detectFaces(cv::Mat& frame, CascadeClassifier&);
	void SaveFaces(char*);//ʹ��char�������������������
	vector<Rect> GetRect();

	void FaceHist();//������õ���������ֱ��ͼ
	void BackHist(Mat);

	void trans_img(Mat);
	void CAMSHIFT();
	
	CascadeClassifier GetClassifier();

	//friend void Tracking::faceTracking(Mat& image,Facedetect&);
private:
	int m;//��¼����ͼƬ�ĸ���
	Mat hue;
	Mat hist;//���������������Ҳ��ʹ�õ�ֱ��ͼ
	Mat backproj;
	Mat mask;
	Mat hsv;
	vector<Rect> faces;
	Rect trackWindow;
	Mat img;//���֮�����ڱ����������ͼƬ
	CascadeClassifier face_cascade;//ʹ�ø��ഴ������֮�󣬿���ʹ�ø����еĶ���

	friend class Tracking;
//Ϊ����Ϊ��ĳ�Ա������ʱ���ܹ�����ʹ��
};//�������Զ�������Ϊ˽�У����еķ���������Ϊpublic
//������ඨ���Ϊ��Ԫ���������Է�����Ԫ���е�˽��Ԫ��


