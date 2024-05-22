#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include<QThread>
#include<QDebug>
#include<vector>
#include<unordered_map>
#include<string>


using namespace std;

using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class cubeDetect;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // 魔方信息识别对象
    cubeDetect *rcb;

    unordered_map<string, vector<string>> ruckCube;

private slots:
    // 打开识别窗口
    void on_openCameraButton_clicked();

    // 关闭识别窗口
    void on_closeCameraButton_clicked();

    // 处理一次传过来的数据
    void get_once_info(vector<string>);
private:
    void changeBottonColor(const QString & buttonName, const QString & color);
};


#endif // MAINWINDOW_H
