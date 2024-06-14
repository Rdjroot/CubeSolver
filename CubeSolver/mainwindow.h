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
#include<QCloseEvent>
#include<QVBoxLayout>
#include "solver.h"
#include "cubiecube.h"
#include <QMessageBox>
#include<QMutex>
#include<memory>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class cubeDetect;
class BuildCube;
class Solver;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    bool spinover;
    bool glShow;

    //當前魔方
    CubieCube ccb;

    // 魔方信息识别对象
    cubeDetect *rcb;

    // 构建3D魔方
    shared_ptr<BuildCube> bdc;

    // 求解
    shared_ptr<Solver> solver;

    // 存储魔方色块
    unordered_map<string, vector<string>> ruckCube;

    // opengl 魔方展示對象
    unordered_map<string, vector<string>> cubeForGL;

    vector<string> COLORS;

    // OPENgl方块数据
    float vertices[5184];

    // 颜色与opengl颜色数据的映射表
    unordered_map<string, vector<float>> openGLCubeColor;

    unordered_map<string, vector<int>> tranIndex;

    // 存储当前解法
    vector<string> answerLatex;

    // 存储输入框
    vector<string> inputLatex;

    // 互斥量，防止動畫覆蓋
    QMutex qmtx, cqmtx;

private slots:
    // 打开识别窗口
    void on_openCameraButton_clicked();

    // 关闭识别窗口
    void on_closeCameraButton_clicked();

    // 处理一次传过来的数据
    void get_once_info(vector<string>);

    // 用掃描的顔色構建模仿
    void on_constructButton_clicked();

    // 調試代碼
    void on_pushButton_4_clicked();
    void on_pushButton_U2_clicked();
    void on_pushButton_L1_clicked();
    void on_pushButton_F_clicked();

    // 求解
    void on_solveButton_clicked();

    // 關閉3D展示
    void on_closeShowButton_clicked();

    // 還原動畫演示
    void on_startSolveButton_clicked();

    // 匹配動畫動作
    void showMoveLabel(QString m);

    // 需要轉動的隊列
    void spinCubeAction(vector<string> moves, int speed);

    // 復原3D魔方
    void on_restoreButton_clicked();

    // 後臺旋轉結束
    void spinOverDeal();

private:
    // 魔方是否復原
    bool reduction;
    
    // 根據掃描顔色，改變主界面矩陣
    void changeBottonColor(const QString & buttonName, const QString & color);

    // 初始化还原状态魔方的颜色矩阵
    void initVertices();

    // 存储的颜色map和opengl图像矩阵的顺序映射关系
    void initTransformForOpenGL();

    // 将现有的颜色map根据映射贴图到颜色矩阵中
    void drawingCubeColor();

    // 显示opengl窗口
    void opengl_showCube();

    void constructByColor();

    void constructByLatex(QString inputInfo);

protected:
    // 窗口关闭，释放对象
    void closeEvent(QCloseEvent *event) override;

signals:
    void ExitWin();

    void showCube();

    void hideCube();

    void sendStrToCube(QString m);

    void spinCube(vector<string> moves, int spped);

    void clearQueue();

    void executeCommand(int speed);
};

#endif // MAINWINDOW_H
