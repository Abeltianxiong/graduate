#include "tracking.h"


void Tracking::faceTracking(Mat& image)
{

}


//目前只需要对facetracking做一个大的修改就好
void Tracking::faceTracking(Mat& image, Facedetect& facedetect) {//将该函数，定义成为facedetect类中的友元函数
	Mat hsv,hue,mask, histimg = Mat::zeros(200, 320, CV_8UC3),b;//使用函数重载，省的一直修改 , Facedetect& Facedetect
	vector<Mat> histimgs, backproj;
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
		{//该条件下整个，都是根据新增加的跟踪目标计算的。 

			for (int i=0;i<updateWindow.size();i++)
			{
				Mat roi(hue, updateWindow[i]),h;//选择到感兴趣区域
				calcHist(&roi, 1, 0, Mat(), h, 1, &hsize, &phranges);
				normalize(h, h, 0, 255, NORM_MINMAX);//均衡
				hist.emplace_back(h);

				trackWindow.push_back(updateWindow[i]);//添加新增的跟踪窗口

				trackObject = 1;//跟之前的if else计算方式是一样的


#if 0
				histimgs.push_back(histimg);
				histimgs[i] = Scalar::all(0);//一下功能画直方图
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
				imshow("Histogram", histimgs[i]);//画直方图也只跟新过一次    
#endif // 0





			}//hist只计算一次，根据trackWindow确定跟踪目标，人脸的肤色区域大体上相同


			
			

			
			//还是根据直方图，修改跟踪目标的个数 新增
			//for(int i=0;i<facedetect.faces.size();i++)//vector没有开辟空间不能够直接赋值
				//trackWindow.push_back( facedetect.faces[i]);
				//trackWindow[i] = facedetect.faces[i];//跟踪窗口需要用容器保存

			

			

			
			//此处可以不写，丢失一帧没事

			for (int i = 0; i < trackWindow.size(); i++)//不同的直方图，计算得到的整幅图像的反向投影应该也不同
			{
				//cout << "hello tracking" << endl;
				calcBackProject(&hue, 1, 0, hist[i], b, &phranges);
				backproj.emplace_back(b);
				backproj[i] &= mask;
				imshow("backproj", backproj[i]);//滤波之后的直方图反向投影 (全图)

				RotatedRect trackBox = CamShift(backproj[i], trackWindow[i], TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
				//替换trac_point的坐标

				Point2f tp;
				tp = trackBox.center;
				trac_point.emplace_back(tp);//跟踪窗口，应该是替换，如果没有新的加入的时候

				cout << "the tracking nums: " << trac_point.size() << endl;
		}

#if 0
			calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
			backproj &= mask;
			imshow("backproj", backproj);//滤波之后的直方图反向投影 (全图)

			//此处错误。应该将多个跟踪区域的直方图计算保存在容器中。根据直方图，来对应跟踪目标。
			for (int i = 0; i < facedetect.faces.size(); i++)
			{
				RotatedRect trackBox = CamShift(backproj, trackWindow[i], TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
				//cout << trackBox.center.x << " " << trackBox.center.y << endl;
				//将中心坐标保存到容器中//需要完成运算符重载，
				trac_point.push_back(trackBox.center);//第一跟踪的时候，是直接插入容器中，后续帧跟踪的时候，是替换
				cout << "the tracking center: " << trackBox.center << endl;
				cout << "the tracking nums: " << trac_point.size() << endl;
			}//计算Hist也没有影响到后边的跟踪。  
#endif // 0



		}
		else {
			
#if 1
			for (int i = 0; i < trackWindow.size(); i++)//不同的直方图，计算得到的整幅图像的反向投影应该也不同
			{
				//cout << "hello tracking" << endl;
				calcBackProject(&hue, 1, 0, hist[i], b, &phranges);
				backproj.emplace_back(b);
				backproj[i] &= mask;
				imshow("backproj", backproj[i]);//滤波之后的直方图反向投影 (全图)

				RotatedRect trackBox = CamShift(backproj[i], trackWindow[i], TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
				ellipse(image, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
				//替换trac_point的坐标

				//Point2f tp;
				//tp= trackBox.center;
				//trac_point.emplace_back(tp);//跟踪窗口，应该是替换，如果没有新的加入的时候

				//cout << "the tracking nums: " <<  trac_point.size() << endl;
			}
#endif // 


			
			
			

#if 0
			//初次跟踪需要根据faces.size()，再次跟踪，trac_point
			for (int i = 0; i < trac_point.size(); i++)
			{


			}
#endif // 0

		
		}

	}
}
//跟踪这一部分的修改，真的是很麻烦。
//再创建一个函数，重新从单目标跟踪中修改，修改一部分，保证不影响之前的部分。

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

void Tracking::Set_update_point(vector<Rect> flag) {//使用之前，清空一次就好
	//更新的跟踪窗口
	vector<Rect> up_win;
	up_win = flag;//所有的初始位置，都会存入容器中？
	updateWindow = up_win;//直接赋值，会将之前的容器覆盖掉


}//只需要判断updateWindow是否为空就可以了