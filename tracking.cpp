#include "tracking.h"
#include "math.h"

//需要用到友元，只需要将facedetect传入就好
bool Tracking::UpdateTracking(Facedetect& facedetect) {//不需要参数，每次讲检测得到的vector<Rect> faces 与trackWindows对别
	vector<Rect> update;

	if (!trackWindow.data())//初始状态，没有跟踪点的情况
	{
		for(int i=0;i<facedetect.faces.size();i++)
			update.push_back(facedetect.faces[i]);
	}
	else {//有跟踪
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
		
	updateWindow = update;//如果没有就讲容器清空？
	//此变量重新赋值，空间是否还存在？，如果是局部变量，全局指针，指向局部变量，是否能访问到？
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
		inRange(hsv, Scalar(0, 30, 10), Scalar(90, 256, 256), mask);//滤波使用

		
		int ch[] = { 0, 0 };
		hue.create(hsv.size(), hsv.depth());
		mixChannels(&hsv, 1, &hue, 1, ch, 1);//使hue保存H通道颜色图像

		if (trackObject < 0) {
			
			//直方图，只计算一次
			if (!hist_flag)//只计算一次hist
			{
				Mat roi(hue, facedetect.faces[0]);//确保第一张人脸图片没有检测出错
				
				
				calcHist(&roi, 1, 0, Mat(), hist, 1, &hsize, &phranges);

				hist_flag = 1;//检测一次，更新标志位

				
				//hist只计算一次，画出直方图
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
				imshow("Histogram", histimg);//画直方图也只跟新过一次    。
				cout << "hello world" << endl;
			}//画图出错？
			
			//将检测到的更新的人脸，保存在updatewindows中，trackwindows保存所有正在跟踪的人脸

			
			for (int i = 0; i < updateWindow.size(); i++) {//将新有的跟踪窗口加入总共的跟踪窗口
				
				trackWindow.push_back(updateWindow[i]);
			}



		}
		//计算方向投影，camshift
		calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
		backproj &= mask;
		imshow("backproj", backproj);
		for (int i = 0; i < trackWindow.size(); i++)//跟踪窗口的初始位置，是一直不变的？？？
		{
			RotatedRect trackBox = CamShift(backproj, trackWindow[i], TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
			ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);//跟踪窗口采用的也是引用的方式，trackWindows[i]始终在改变
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

	for (size_t i = 0; i < updateWindow.size(); i++)//此循环，包含了检测到人脸的条件
	{
		img(updateWindow[i]).copyTo(ROI_img);//当前帧图片
		sprintf(co, "%s%d%s", con, m, jpg);//string和char的操作不能够混用
		m++;//编号自动更新了
		imwrite(co, ROI_img);
		cout << co << endl;
		cout << "save face true" << endl;
	}
}