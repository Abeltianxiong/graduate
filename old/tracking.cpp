#include "tracking.h"


void Tracking::faceTracking(Mat& image)
{

}


//Ŀǰֻ��Ҫ��facetracking��һ������޸ľͺ�
void Tracking::faceTracking(Mat& image, Facedetect& facedetect) {//���ú����������Ϊfacedetect���е���Ԫ����
	Mat hsv,hue,mask, histimg = Mat::zeros(200, 320, CV_8UC3),b;//ʹ�ú������أ�ʡ��һֱ�޸� , Facedetect& Facedetect
	vector<Mat> histimgs, backproj;
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
		{//�����������������Ǹ��������ӵĸ���Ŀ�����ġ� 

			for (int i=0;i<updateWindow.size();i++)
			{
				Mat roi(hue, updateWindow[i]),h;//ѡ�񵽸���Ȥ����
				calcHist(&roi, 1, 0, Mat(), h, 1, &hsize, &phranges);
				normalize(h, h, 0, 255, NORM_MINMAX);//����
				hist.emplace_back(h);

				trackWindow.push_back(updateWindow[i]);//��������ĸ��ٴ���

				trackObject = 1;//��֮ǰ��if else���㷽ʽ��һ����


#if 0
				histimgs.push_back(histimg);
				histimgs[i] = Scalar::all(0);//һ�¹��ܻ�ֱ��ͼ
				int binW = histimgs[i].cols / hsize;
				Mat buf(1, hsize, CV_8UC3);
				for (int i = 0; i < hsize; i++)
					buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);
				cvtColor(buf, buf, COLOR_HSV2BGR);

				for (int i = 0; i < hsize; i++)
				{
					int val = saturate_cast<int>(hist[i].at<float>(i)*histimgs[i].rows / 255);
					rectangle(histimgs[i], Point(i*binW, histimgs[i].rows),
						Point((i + 1)*binW, histimgs[i].rows - val),
						Scalar(buf.at<Vec3b>(i)), -1, 8);
				}
				imshow("Histogram", histimgs[i]);//��ֱ��ͼҲֻ���¹�һ��    
#endif // 0





			}//histֻ����һ�Σ�����trackWindowȷ������Ŀ�꣬�����ķ�ɫ�����������ͬ


			
			

			
			//���Ǹ���ֱ��ͼ���޸ĸ���Ŀ��ĸ��� ����
			//for(int i=0;i<facedetect.faces.size();i++)//vectorû�п��ٿռ䲻�ܹ�ֱ�Ӹ�ֵ
				//trackWindow.push_back( facedetect.faces[i]);
				//trackWindow[i] = facedetect.faces[i];//���ٴ�����Ҫ����������

			

			

			
			//�˴����Բ�д����ʧһ֡û��

			for (int i = 0; i < trackWindow.size(); i++)//��ͬ��ֱ��ͼ������õ�������ͼ��ķ���ͶӰӦ��Ҳ��ͬ
			{
				//cout << "hello tracking" << endl;
				calcBackProject(&hue, 1, 0, hist[i], b, &phranges);
				backproj.emplace_back(b);
				backproj[i] &= mask;
				imshow("backproj", backproj[i]);//�˲�֮���ֱ��ͼ����ͶӰ (ȫͼ)

				RotatedRect trackBox = CamShift(backproj[i], trackWindow[i], TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
				//�滻trac_point������

				Point2f tp;
				tp = trackBox.center;
				trac_point.emplace_back(tp);//���ٴ��ڣ�Ӧ�����滻�����û���µļ����ʱ��

				cout << "the tracking nums: " << trac_point.size() << endl;
		}

#if 0
			calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
			backproj &= mask;
			imshow("backproj", backproj);//�˲�֮���ֱ��ͼ����ͶӰ (ȫͼ)

			//�˴�����Ӧ�ý�������������ֱ��ͼ���㱣���������С�����ֱ��ͼ������Ӧ����Ŀ�ꡣ
			for (int i = 0; i < facedetect.faces.size(); i++)
			{
				RotatedRect trackBox = CamShift(backproj, trackWindow[i], TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
				//cout << trackBox.center.x << " " << trackBox.center.y << endl;
				//���������걣�浽������//��Ҫ�����������أ�
				trac_point.push_back(trackBox.center);//��һ���ٵ�ʱ����ֱ�Ӳ��������У�����֡���ٵ�ʱ�����滻
				cout << "the tracking center: " << trackBox.center << endl;
				cout << "the tracking nums: " << trac_point.size() << endl;
			}//����HistҲû��Ӱ�쵽��ߵĸ��١�  
#endif // 0



		}
		else {
			
#if 1
			for (int i = 0; i < trackWindow.size(); i++)//��ͬ��ֱ��ͼ������õ�������ͼ��ķ���ͶӰӦ��Ҳ��ͬ
			{
				//cout << "hello tracking" << endl;
				calcBackProject(&hue, 1, 0, hist[i], b, &phranges);
				backproj.emplace_back(b);
				backproj[i] &= mask;
				imshow("backproj", backproj[i]);//�˲�֮���ֱ��ͼ����ͶӰ (ȫͼ)

				RotatedRect trackBox = CamShift(backproj[i], trackWindow[i], TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
				//�滻trac_point������

				//Point2f tp;
				//tp= trackBox.center;
				//trac_point.emplace_back(tp);//���ٴ��ڣ�Ӧ�����滻�����û���µļ����ʱ��

				//cout << "the tracking nums: " <<  trac_point.size() << endl;
			}
#endif // 


			
			
			

#if 0
			//���θ�����Ҫ����faces.size()���ٴθ��٣�trac_point
			for (int i = 0; i < trac_point.size(); i++)
			{


			}
#endif // 0

		
		}

	}
}
//������һ���ֵ��޸ģ�����Ǻ��鷳��
//�ٴ���һ�����������´ӵ�Ŀ��������޸ģ��޸�һ���֣���֤��Ӱ��֮ǰ�Ĳ��֡�

vector<Point2f> Tracking::Get_track_point() {
	return trac_point;
}

int Tracking::Get_trackObj()
{
	return trackObject;
}

void Tracking::Set_trackObj(int flag){
	trackObject = flag;
}

void Tracking::Set_update_point(vector<Rect> flag) {//ʹ��֮ǰ�����һ�ξͺ�
	//���µĸ��ٴ���
	vector<Rect> up_win;
	up_win = flag;//���еĳ�ʼλ�ã�������������У�
	updateWindow = up_win;//ֱ�Ӹ�ֵ���Ὣ֮ǰ���������ǵ�


}//ֻ��Ҫ�ж�updateWindow�Ƿ�Ϊ�վͿ�����