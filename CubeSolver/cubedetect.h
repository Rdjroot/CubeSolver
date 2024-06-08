#ifndef CUBEDETECT_H
#define CUBEDETECT_H

#include<vector>
#include<string>
#include <QWidget>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<unordered_map>
#include<QMessageBox>
#include<QDebug>
#include "camerathread.h"
#include "mainwindow.h"
#include<QMutex>

using namespace std;

namespace Ui {
class cubeDetect;
}

class cubeDetect;
// 前向声明
class MainWindow;

// 魔方色块图像识别
class cubeDetect : public QWidget
{
    Q_OBJECT

public:
    explicit cubeDetect(QWidget *parent = nullptr);
    ~cubeDetect();

signals:

    // 返回魔方一面数据回主界面
    void sendData(const vector<string> data);

    void allInfoGet(const unordered_map<string, vector<string>> cmap);

    // 修改画面信息传输处理函数
    void changeFlag(const bool flag);


private slots:

    // 识别画面获取魔方颜色
    void dealImgInfo(cv::Mat img);

    // 存储当前色面信息，并向主线程发送信号
    void on_saveButton_clicked();

    // 调用线程，启动摄像头开始识别魔方
    void on_startButton_clicked();

    // 暂停识别当前画面信息
    void on_pauseButton_clicked();

    // 暂停画面展示
    void pauseImg(cv::Mat img);

    void getCameraFailed();

private:
    Ui::cubeDetect *ui;

    // 当前色面信息的缓存
    vector<string> cache;

    // 摄像头开启线程
    CameraThread *camera_t;

    // 存储当前画面
    cv::Mat myImg;

    // 互斥量，用于修改线程的信号传输标志位
    QMutex *mtx;

    // 以下数据为固定数据
    // 魔方六面信息
    unordered_map<string, vector<string>> sixFaces;

    // 画面中的魔方识别框架
    vector<cv::Point> Frame;

    // 九个色块点位
    vector<vector<cv::Point>> ninePoints;

    // 六个颜色的HSV值
    vector<vector<int>> colorsHSV;
    // 用于画图的颜色
    vector<cv::Scalar> myColorValues;
    // 六个颜色名称对应
    unordered_map<int, string> colorMap;

private:
    // 判断当前色块的颜色并返回
    string getContours(cv::Mat imgDil, const int colorInt, const int pos);
    // mat转换为QImg，在label中展示
    void MatToQImage();
};

#endif // CUBEDETECT_H
