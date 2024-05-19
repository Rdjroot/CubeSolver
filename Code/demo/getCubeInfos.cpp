#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<iostream>
#include<unordered_map>

using namespace cv;
using namespace std;

////////////////////////////// Webcam  ///////////////////////////////
// hmin, hmax, smin, smax, vmin, vmax
vector<vector<int>> myColors = { {63, 179,0, 148,108, 255},  // white
								{18, 51,56, 179,91, 255},	   // yellow
								{ 98, 136,109, 255,35, 113},		// blue
								{ 0, 0,177, 255,61, 129},		// red
								{ 58, 86,85, 255,29, 255},		// green
								{  2, 29,105, 199,72, 255}   // orange
};

vector<Scalar> myColorValues = { {255, 255, 255},		// white
								{0, 255, 255},			// yellow   need
								{255, 0, 0},			// blue
								{0, 0, 255},			// red		need
								{0, 255, 0},			// green	a little need
								{0, 165, 255},			// orange  must need
	// blue
};
unordered_map<int, string> colorMap = { {0, "White"},{1,"Yellow"},{2,"blue"} ,{3,"Red"},{4,"Green"},{5,"Orange"} };
Mat img;// 用于处理图像数据

int PIC_INT = 0;
int check = 1;

unordered_map<int, string> colors = { {0,"white"},{1,"yellow"},
									{2,"blue"},{3,"red"} ,
	{4,"green"},{5,"orange"} };

void getContours(Mat imgDil, string objectType) {
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	/**
	* findContours()用于在二值图像中查找轮廓
	* contours 将存储找到的轮廓，每个轮廓都是一个包含一系列点的向量
	* hierarchy 用于存储轮廓之间的关系，即层次结构信息
	* mode(检测模式):  RETR_EXTERNAL -- 》 表示只检测最外层轮廓。
	* method(检测方式)： CHAIN_APPROX_SIMPLE  --》 表示对轮廓中的冗余点进行压缩，只保留端点
	*/
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// 根据contours在img中勾勒轮廓
	// args: pic, outlines, 勾勒范围（-1表示所有）, line_color, line_thin
	//drawContours(img, contours, -1, Scalar(255, 0, 255),2);

	for (int i = 0; i < contours.size(); i++) {
		// 计算该轮廓的面积
		int area = contourArea(contours[i]);
		//cout << area << endl;

		vector<vector<Point>> conPoly(contours.size());
		vector<Rect> boundRect(contours.size());
		if (objectType == "Red" || area > 1000)
		{
			// arcLength()  用于计算轮廓的周长，true表示轮廓是封闭的
			float peri = arcLength(contours[i], true);

			// 对轮廓根据一定精度进行逼近，可以想象以下微积分
			// approxPolyDp(src, dst, precision, bool) bool表示轮廓是否为封闭的
			// precision表示逼近精度，是原始轮廓周长的百分比，越小表示逼近越精细
			approxPolyDP(contours[i], conPoly[i], 0.002 * peri, true);
			boundRect[i] = boundingRect(conPoly[i]);
			//// 勾勒魔方色块的轮廓
			//drawContours(img, boundRect, i, Scalar(255, 0, 255), 2);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 255), 2);
			// 获取每个轮廓矩形的坐标
			
			// 标记颜色
			putText(img, objectType, { boundRect[i].x, boundRect[i].y - 5 }, FONT_HERSHEY_PLAIN, 1, Scalar(0, 69, 255), 2);
		}
	}
}


void findColor(Mat img)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	// 定义的六种颜色将会被识别
	for (int i = 0; i < myColors.size(); i++) {
		Scalar lower(myColors[i][0], myColors[i][2], myColors[i][4]);
		Scalar upper(myColors[i][1], myColors[i][3], myColors[i][5]);
		Mat mask;
		// 识别出来的颜色，将会是白色块，其余是黑色
		inRange(imgHSV, lower, upper, mask);

		Mat maskTemp;
		GaussianBlur(mask, maskTemp, Size(3, 3), 3, 0);
		//if (i == 3) {
		//	imshow("origin", img);
		//	imshow("before", mask);
		//	imshow("after", maskTemp);
		//	waitKey(1);
		//}
		
		// 在原图中勾勒获取识别的色块
		getContours(maskTemp, colorMap[i]);
	}
}

void main() {
	// 使用相机ID 打开摄像头，如果只有一个摄像头，输入0即可
	VideoCapture cap(1);
	//namedWindow("Trackbars", (640, 200));
	//createTrackbar("pic ", "Trackbars", &PIC_INT, 5);		// 第幾張圖片
	
	while (true) {
		cap.read(img);		// 將會逐幀讀取視頻里的内容，然后存储到img对象中
		//string path = "Resources/" + colors[PIC_INT] + ".jpg";
		//img = imread(path);
		findColor(img);

		imshow("Image", img);
		waitKey(1);			// 每帧暂停1毫秒
	}
}

//void main()
//{
//	main_func();
//}