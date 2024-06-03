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
#include "buildcube.h"
#include<QVBoxLayout>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class cubeDetect;
class BuildCube;

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
    BuildCube *bdc;

    // opengl 魔方展示對象
    // 存储魔方色块
    unordered_map<string, vector<string>> ruckCube;
    unordered_map<string, vector<string>> cubeForGL;

    vector<string> COLORS;

    // OPENgl方块数据
    float vertices[5184];
    // 颜色与opengl颜色数据的映射表
    unordered_map<string, vector<float>> openGLCubeColor;

    unordered_map<string, vector<int>> tranIndex;

private slots:
    // 打开识别窗口
    void on_openCameraButton_clicked();

    // 关闭识别窗口
    void on_closeCameraButton_clicked();

    // 处理一次传过来的数据
    void get_once_info(vector<string>);

    void on_constructButton_clicked();

private:
    void changeBottonColor(const QString & buttonName, const QString & color);

    void initVertices();

    void initTransformForOpenGL();

    void drawingCubeColor();

    void hideCubeButton();
};

#endif // MAINWINDOW_H
