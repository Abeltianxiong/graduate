#include "tracking.h"


void Tracking::faceTracking(Mat& image)
{

}

void Tracking::faceTracking(Mat& image, Facedetect& facedetect) {//���ú����������Ϊfacedetect���е���Ԫ����
	Mat hsv,hue,mask, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;//ʹ�ú������أ�ʡ��һֱ�޸� , Facedetect& Facedetect
	int hsize = 16;
	float hranges[] = { 0,180 };
	const float* phranges = hranges;
	

	cv::cvtColor(image, hsv, COLOR_BGR2HSV);//ת����ΪHSVͼ��

	if (trackObject) {//������ٵ�����
		inRange(hsv, Scalar(0, 30, 10),Scalar(180, 256, 256), mask);
		//ʹ�ö�ֵ��mask��ֱ��ͼ����ͶӰ��һ���򵥵��˲�������Ч��������˲�ȡֵ�кܴ��ϵ

		int ch[] = { 0, 0 };
		hue.create(hsv.size(), hsv.depth());
		mixChannels(&hsv, 1, &hue, 1, ch, 1);//ʹhue����Hͨ����ɫͼ��

		if (trackObject < 0)//��⵽���������Ȧ�и���Ŀ��
		{
			Mat roi(hue, facedetect.faces[0]);//ѡ�񵽸���Ȥ����
			calcHist(&roi, 1, 0, Mat(), hist, 1, &hsize, &phranges);
			normalize(hist, hist, 0, 255, NORM_MINMAX);//����

			//����Ҫ����һ�������������Ŀ��ĸ���
			trackWindow = facedetect.faces[0];//���ٴ�����Ҫ����������
			trackObject = 1;//��֮ǰ��if else���㷽ʽ��һ����

			histimg = Scalar::all(0);//һ�¹��ܻ�ֱ��ͼ
			int binW = histimg.cols / hsize;
			Mat buf(1, hsize, CV_8UC3);
			for (int i = 0; i < hsize; i++)
				buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);
			cvtColor(buf, buf, COLOR_HSV2BGR);

			for (int i = 0; i < hsize; i++)
			{
				int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);
				rectangle(histimg, Point(i*binW, histimg.rows),
					Point((i + 1)*binW, histimg.rows - val),
					Scalar(buf.at<Vec3b>(i)), -1, 8);
			}
			imshow("Histogram", histimg);//��ֱ��ͼҲֻ���¹�һ��
		}

		calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
		backproj &= mask;
		imshow("backproj",backproj);//�˲�֮���ֱ��ͼ����ͶӰ
		RotatedRect trackBox = CamShift(backproj, trackWindow,TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

		ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
		
	}
}

int Tracking::Get_trackObj()
{
	return trackObject;
}

void Tracking::Set_trackObj(int flag){
	trackObject = flag;
}