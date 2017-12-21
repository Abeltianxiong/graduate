#include "tracking.h"
#include "math.h"

//��Ҫ�õ���Ԫ��ֻ��Ҫ��facedetect����ͺ�
bool Tracking::UpdateTracking(Facedetect& facedetect) {//����Ҫ������ÿ�ν����õ���vector<Rect> faces ��trackWindows�Ա�
	vector<Rect> update;

	if (!trackWindow.data())//��ʼ״̬��û�и��ٵ�����
	{
		for(int i=0;i<facedetect.faces.size();i++)
			update.push_back(facedetect.faces[i]);
	}
	else {//�и���
		for (int i = 0; i < facedetect.faces.size(); i++)
		{
			for (int j = 0; j < trackWindow.size(); j++)
			{
				if (abs(facedetect.faces[i].x - trackWindow[j].x) < 40 ||
					abs(facedetect.faces[i].y - trackWindow[j].y)<40
					)
				{
					break;
				}
				if (j== trackWindow.size()-1)
				{
					update.emplace_back(facedetect.faces[i]);
				}

			}
			

		}
		
	}
		
	updateWindow = update;//���û�оͽ�������գ�
	//�˱������¸�ֵ���ռ��Ƿ񻹴��ڣ�������Ǿֲ�������ȫ��ָ�룬ָ��ֲ��������Ƿ��ܷ��ʵ���
	if (updateWindow.size() == 0)
		return false;
	else
		return true;



}


void Tracking::faceTracking(Mat& image, Facedetect& facedetect) {
	Mat hsv,mask,hue, backproj,histimg = Mat::zeros(200, 320, CV_8UC3);

	int hsize = 16;
	float hranges[] = { 0,180 };
	const float* phranges = hranges;

	if (trackObject) {
		cv::cvtColor(image, hsv, COLOR_BGR2HSV);
		inRange(hsv, Scalar(0, 30, 10), Scalar(90, 256, 256), mask);//�˲�ʹ��

		
		int ch[] = { 0, 0 };
		hue.create(hsv.size(), hsv.depth());
		mixChannels(&hsv, 1, &hue, 1, ch, 1);//ʹhue����Hͨ����ɫͼ��

		if (trackObject < 0) {
			
			//ֱ��ͼ��ֻ����һ��
			if (!hist_flag)//ֻ����һ��hist
			{
				Mat roi(hue, facedetect.faces[0]);//ȷ����һ������ͼƬû�м�����
				
				
				calcHist(&roi, 1, 0, Mat(), hist, 1, &hsize, &phranges);

				hist_flag = 1;//���һ�Σ����±�־λ

				
				//histֻ����һ�Σ�����ֱ��ͼ
				//Mat histimg;
				histimg= Scalar::all(0);
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
				cout << "hist" << endl;
				imshow("Histogram", histimg);//��ֱ��ͼҲֻ���¹�һ��    ��
				cout << "hello world" << endl;
			}//��ͼ����
			
			//����⵽�ĸ��µ�������������updatewindows�У�trackwindows�����������ڸ��ٵ�����

			
			for (int i = 0; i < updateWindow.size(); i++) {//�����еĸ��ٴ��ڼ����ܹ��ĸ��ٴ���
				
				trackWindow.push_back(updateWindow[i]);
			}



		}
		//���㷽��ͶӰ��camshift
		calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
		backproj &= mask;
		imshow("backproj", backproj);
		for (int i = 0; i < trackWindow.size(); i++)//���ٴ��ڵĳ�ʼλ�ã���һֱ����ģ�����
		{
			RotatedRect trackBox = CamShift(backproj, trackWindow[i], TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
			ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);//���ٴ��ڲ��õ�Ҳ�����õķ�ʽ��trackWindows[i]ʼ���ڸı�
			imshow("image",image);
			cout << "trackwindow: " << trackWindow[i] << endl;
		}

	}
}

void Tracking::Set_trackObj(int flag) {
	trackObject = flag;
}

void Tracking::SaveFaces(Mat& img,char* con) {
	char co[100];
	char jpg[] = ".jpg";

	Mat ROI_img;

	for (size_t i = 0; i < updateWindow.size(); i++)//��ѭ���������˼�⵽����������
	{
		img(updateWindow[i]).copyTo(ROI_img);//��ǰ֡ͼƬ
		sprintf(co, "%s%d%s", con, m, jpg);//string��char�Ĳ������ܹ�����
		m++;//����Զ�������
		imwrite(co, ROI_img);
		cout << co << endl;
		cout << "save face true" << endl;
	}
}