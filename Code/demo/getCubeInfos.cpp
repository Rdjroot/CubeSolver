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
Mat img;// ���ڴ���ͼ������

int PIC_INT = 0;
int check = 1;

unordered_map<int, string> colors = { {0,"white"},{1,"yellow"},
									{2,"blue"},{3,"red"} ,
	{4,"green"},{5,"orange"} };

void getContours(Mat imgDil, string objectType) {
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	/**
	* findContours()�����ڶ�ֵͼ���в�������
	* contours ���洢�ҵ���������ÿ����������һ������һϵ�е������
	* hierarchy ���ڴ洢����֮��Ĺ�ϵ������νṹ��Ϣ
	* mode(���ģʽ):  RETR_EXTERNAL -- �� ��ʾֻ��������������
	* method(��ⷽʽ)�� CHAIN_APPROX_SIMPLE  --�� ��ʾ�������е���������ѹ����ֻ�����˵�
	*/
	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// ����contours��img�й�������
	// args: pic, outlines, ���շ�Χ��-1��ʾ���У�, line_color, line_thin
	//drawContours(img, contours, -1, Scalar(255, 0, 255),2);

	for (int i = 0; i < contours.size(); i++) {
		// ��������������
		int area = contourArea(contours[i]);
		//cout << area << endl;

		vector<vector<Point>> conPoly(contours.size());
		vector<Rect> boundRect(contours.size());
		if (objectType == "Red" || area > 1000)
		{
			// arcLength()  ���ڼ����������ܳ���true��ʾ�����Ƿ�յ�
			float peri = arcLength(contours[i], true);

			// ����������һ�����Ƚ��бƽ���������������΢����
			// approxPolyDp(src, dst, precision, bool) bool��ʾ�����Ƿ�Ϊ��յ�
			// precision��ʾ�ƽ����ȣ���ԭʼ�����ܳ��İٷֱȣ�ԽС��ʾ�ƽ�Խ��ϸ
			approxPolyDP(contours[i], conPoly[i], 0.002 * peri, true);
			boundRect[i] = boundingRect(conPoly[i]);
			//// ����ħ��ɫ�������
			//drawContours(img, boundRect, i, Scalar(255, 0, 255), 2);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 255), 2);
			// ��ȡÿ���������ε�����
			
			// �����ɫ
			putText(img, objectType, { boundRect[i].x, boundRect[i].y - 5 }, FONT_HERSHEY_PLAIN, 1, Scalar(0, 69, 255), 2);
		}
	}
}


void findColor(Mat img)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	// �����������ɫ���ᱻʶ��
	for (int i = 0; i < myColors.size(); i++) {
		Scalar lower(myColors[i][0], myColors[i][2], myColors[i][4]);
		Scalar upper(myColors[i][1], myColors[i][3], myColors[i][5]);
		Mat mask;
		// ʶ���������ɫ�������ǰ�ɫ�飬�����Ǻ�ɫ
		inRange(imgHSV, lower, upper, mask);

		Mat maskTemp;
		GaussianBlur(mask, maskTemp, Size(3, 3), 3, 0);
		//if (i == 3) {
		//	imshow("origin", img);
		//	imshow("before", mask);
		//	imshow("after", maskTemp);
		//	waitKey(1);
		//}
		
		// ��ԭͼ�й��ջ�ȡʶ���ɫ��
		getContours(maskTemp, colorMap[i]);
	}
}

void main() {
	// ʹ�����ID ������ͷ�����ֻ��һ������ͷ������0����
	VideoCapture cap(1);
	//namedWindow("Trackbars", (640, 200));
	//createTrackbar("pic ", "Trackbars", &PIC_INT, 5);		// �ڎ׏��DƬ
	
	while (true) {
		cap.read(img);		// ���������xȡҕ�l������ݣ�Ȼ��洢��img������
		//string path = "Resources/" + colors[PIC_INT] + ".jpg";
		//img = imread(path);
		findColor(img);

		imshow("Image", img);
		waitKey(1);			// ÿ֡��ͣ1����
	}
}

//void main()
//{
//	main_func();
//}