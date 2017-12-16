#include "tracking.h"


void Tracking::faceTracking(Mat& image)
{

}

void Tracking::faceTracking(Mat& image, Facedetect& facedetect) {//将该函数，定义成为facedetect类中的友元函数
	Mat hsv,hue,mask, histimg = Mat::zeros(200, 320, CV_8UC3), backproj;//使用函数重载，省的一直修改 , Facedetect& Facedetect
	int hsize = 16;
	float hranges[] = { 0,180 };
	const float* phranges = hranges;
	

	cv::cvtColor(image, hsv, COLOR_BGR2HSV);//转换成为HSV图像

	if (trackObject) {//检测或跟踪到人脸
		inRange(hsv, Scalar(0, 30, 10),Scalar(180, 256, 256), mask);
		//使用二值化mask对直方图反向投影做一个简单的滤波。跟踪效果跟这个滤波取值有很大关系

		int ch[] = { 0, 0 };
		hue.create(hsv.size(), hsv.depth());
		mixChannels(&hsv, 1, &hue, 1, ch, 1);//使hue保存H通道颜色图像

		if (trackObject < 0)//检测到，或者鼠标圈中跟踪目标
		{
			Mat roi(hue, facedetect.faces[0]);//选择到感兴趣区域
			calcHist(&roi, 1, 0, Mat(), hist, 1, &hsize, &phranges);
			normalize(hist, hist, 0, 255, NORM_MINMAX);//均衡

			//还需要设置一个变量保存跟踪目标的个数
			trackWindow = facedetect.faces[0];//跟踪窗口需要用容器保存
			trackObject = 1;//跟之前的if else计算方式是一样的

			histimg = Scalar::all(0);//一下功能画直方图
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
			imshow("Histogram", histimg);//画直方图也只跟新过一次
		}

		calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
		backproj &= mask;
		imshow("backproj",backproj);//滤波之后的直方图反向投影
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