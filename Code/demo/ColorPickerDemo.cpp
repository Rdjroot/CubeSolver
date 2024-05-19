#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<iostream>
#include<unordered_map>

using namespace cv;
using namespace std;


////////////////////////////// Color Detection  ///////////////////////////////

Mat imgHSV, mask;
int hmin = 0, smin = 0, vmin = 0;
int hmax = 179, smax = 255, vmax = 255;

//VideoCapture cap(1);
Mat img;
int flag = 0;
int COLOR_INT = 0;
int cur_color = 0;
int PIC_INT = 2;
int check = 1;

unordered_map<int, string> colors = { {0,"white"},{1,"yellow"},
									{2,"blue"},{3,"red"},
									{4,"green"},{5,"orange"} };

vector<vector<int>> myColors = { {63, 179,0, 148,108, 255},  // white
								{18, 51,56, 179,91, 255},	   // yellow
								{ 98, 136,109, 255,35, 113},		// blue
								{ 0, 0,177, 255,61, 129},		// red
								{ 58, 86,85, 255,29, 255},		// green
								{  2, 29,105, 199,72, 255}   // orange
};

void setHSV(int i) {
	hmin = myColors[i][0];
	hmax = myColors[i][1];
	smin = myColors[i][2];
	smax = myColors[i][3];
	vmin = myColors[i][4];
	vmax = myColors[i][5];
}

void print_cur_color() {
	// 当前筛选的值为
	cout << "[ cur_dev_color is: " << colors[COLOR_INT] << " ]" << endl;
}

void upgradeHSV(vector<int> newColors)
{
	myColors[COLOR_INT] = newColors;
}

void main() {

	// 设置一个窗口（640 x 200），控制这些值的大小
	namedWindow("Trackbars", (640, 200));
	createTrackbar("Hue Min", "Trackbars", &hmin, 179);
	createTrackbar("Hue Max", "Trackbars", &hmax, 179);
	createTrackbar("Sat Min", "Trackbars", &smin, 255);
	createTrackbar("Sat Max", "Trackbars", &smax, 255);
	createTrackbar("Val Min", "Trackbars", &vmin, 255);
	createTrackbar("Val Max", "Trackbars", &vmax, 255);
	createTrackbar("print vals", "Trackbars", &flag, 2);
	createTrackbar("pic ", "Trackbars", &PIC_INT, 5);		// 第幾張圖片
	createTrackbar("cur color", "Trackbars", &COLOR_INT, 5);		// 檢測什麽顔色
	createTrackbar("setup check color", "Trackbars", &check, 1);		// 檢測什麽顔色

	while (true)
	{
		// 获取当前图片
		string path = "Resources/" + colors[PIC_INT] + ".jpg";
		// 代表当前检测什么颜色
		string MASK_TITEL = colors[COLOR_INT];
		img = imread(path);

		// 判断图片是否读取成功
		if (img.empty()) {
			cout << "warning" << endl;
			cout << endl;
		}
		cvtColor(img, imgHSV, COLOR_BGR2HSV);  // 二值圖像

		// 這裏進行一個 值的設定
		if (check == 1)
		{
			setHSV(COLOR_INT);
			cout << "please change check to 0, are u ok ?  ";
			check = 0;
		}

		Scalar lower(hmin, smin, vmin);
		Scalar upper(hmax, smax, vmax);

		inRange(imgHSV, lower, upper, mask);		// 根據值進行遮罩
		//cout << "HSV is : " << hmin << ", " << hmax << "," << smin << ", " << smax << "," << vmin << ", " << vmax << endl;

		imshow("Image", img);
		imshow(MASK_TITEL, mask);
		waitKey(1);

		if (flag == 1)
		{
			cout << colors[COLOR_INT] << endl;
			cout << "HSV is : " << hmin << ", " << hmax << "," << smin << ", " << smax << "," << vmin << ", " << vmax << endl;
			vector<int> newHSV = { hmin, hmax, smin, smax, vmin, vmax };
			upgradeHSV(newHSV);
			int c;
			// 調整flag = 0， 切換需要檢測的顔色
			cout << "please turn flag to 0, and change the new check color, are you ok ?:";
			flag = 0;
			COLOR_INT++;
			if (COLOR_INT > 5)
				COLOR_INT = 0;
		}
	}

	// 摄像头款
	//while (true) {
	//	//cap.read(img);		// 将此时画面给予img		

	//	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	//	Scalar lower(hmin, smin, vmin);
	//	Scalar upper(hmax, smax, vmax);

	//	inRange(imgHSV, lower, upper, mask);	// 获取二值图

	//	if (flag == 1) {
	//		cout << "【" << color << " 】" << endl;
	//		cout << "HSV is " << hmin << ", " << hmax << "," << smin << ", " << smax << "," << vmin << ", " << vmax << endl;
	//	}

	//	imshow("Image", img);
	//	imshow("Image Mask", mask);

	//	waitKey(1);
	//}

}