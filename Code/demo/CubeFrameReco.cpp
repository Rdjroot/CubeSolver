#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<iostream>
#include<unordered_map>
#include<thread>
#include<chrono>

using namespace cv;
using namespace std;

/**
* 識別六面色塊，並輸出
*/
unordered_map<string, vector<string>> sixFaces;

// 畫一個矩形
Scalar WHITE = { 255, 255,255 };
Scalar PURPLE = { 255, 0, 255 };
Mat img;
// 魔方矩陣
vector<Point> Frame = { {90,90},{450, 450} };

int flag = 0;

vector<vector<Point>> ninePoints = { {{130, 130},{170,170}},{{250,130},{290,170}},{{370,130},{410,170}},
	{{130, 250},{170,290}},{{250,250},{290,290}},{{370,250},{410,290}},
	{{130,370}, {170,410}},{{250,370},{290,410}},{{370,370},{410,410}}
};
// 六個顔色的HSV
vector<vector<int>> myColors = { {63, 179,0, 148,108, 255},  // white
								{18, 51,56, 179,91, 255},	   // yellow
								{ 98, 136,109, 255,35, 113},		// blue
								{ 0, 0,177, 255,61, 129},		// red
								{ 58, 86,85, 255,29, 255},		// green
								{  2, 29,105, 199,72, 255}   // orange
};

// 標記六個顔色
vector<Scalar> myColorValues = { {255, 255, 255},		// white
								{0, 255, 255},			// yellow   
								{255, 0, 0},			// blue
								{0, 0, 255},			// red		
								{0, 255, 0},			// green	
								{0, 165, 255},			// orange 
};
unordered_map<int, string> colorMap = { {0, "White"},{1,"Yellow"},{2,"blue"} ,
										{3,"Red"},{4,"Green"},{5,"Orange"} };

string getContours(Mat imgDil, static int colorInt, static int pos) {
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	if (!contours.empty()) 
	{
		rectangle(img, ninePoints[pos][0], ninePoints[pos][1], myColorValues[colorInt], 2);
		return colorMap[colorInt];
	}	
	return "";
}

// 識別魔方九個色塊
void recogeniseColors(Mat img, int flag)
{
	Mat divCube,divHSV;
	vector<string> temp;
	for (int i = 0; i < 9; i++)
	{
		Rect roi(ninePoints[i][0],ninePoints[i][1]);
		divCube = img(roi);
		cvtColor(divCube, divHSV, COLOR_BGR2HSV);

		// 定义的六种颜色将会被识别
		for (int j = 0; j < myColors.size(); j++) {
			Scalar lower(myColors[j][0], myColors[j][2], myColors[j][4]);
			Scalar upper(myColors[j][1], myColors[j][3], myColors[j][5]);
			Mat mask;
			// 识别出来的颜色，将会是白色块，其余是黑色
			inRange(divHSV, lower, upper, mask);

			// 在原图中勾勒获取识别的色块
			string color = getContours(mask, j, i);
			// 識別出了顔色
			if (!color.empty()) {
				temp.push_back(color);
				break;
			}	
		}
	}
	if (temp.size() == 9 && flag == 1) 
	{
		// 獲取中心色塊
		string center = temp[4];
		sixFaces.insert({ center, temp });
		flag = 0;
	}
}


void main()
{
	VideoCapture cap(1);
	namedWindow("Trackbars", (640, 200));
	createTrackbar("initial", "Trackbars", &flag, 1);
	while (true)
	{
		cap.read(img);
		//Rect temp = getCubeFrame(img);

		// 識別框
		// 在img绘制一个矩形
		// args: 左上角，右下角，颜色，线条厚度
		rectangle(img, Frame[0], Frame[1], WHITE, 2);

		recogeniseColors(img, flag);

		imshow("camara", img);
		waitKey(1);
		if (sixFaces.size() == 6) {
			break;
		}
	}
	for (auto& mp : sixFaces) 
	{
		cout << mp.first << endl;
		cout << "{ ";
		for (auto& s : mp.second) {
			cout << s << ", ";
		}
		cout <<" }"<< endl;
	}

}