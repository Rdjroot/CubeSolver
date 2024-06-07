#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cubedetect.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , rcb(nullptr)
    , solver(nullptr)
{
    this->setWindowIcon(QIcon(":/Resource/cubeIcon.png"));
    this->setWindowTitle("CubeSolver");
    ui->setupUi(this);
    ui->blue5->setStyleSheet("background-color: blue");
    ui->white5->setStyleSheet("background-color: white");
    ui->orange5->setStyleSheet("background-color: orange");
    ui->red5->setStyleSheet("background-color: red");
    ui->green5->setStyleSheet("background-color: green");
    ui->yellow5->setStyleSheet("background-color: yellow");

    this->COLORS = {"blue","green","yellow","white","orange","red"};

    this->ccb = CubieCube(0);       // 获取一个还原状态的魔方
    this->reduction = true;            // 是否为还原状态
    solver = Solver::getInstance();
    ui->openGLWidget->hide();

    this->bdc = nullptr;

    // 調試代碼隱藏
    ui->pushButton_4->hide();
    ui->pushButton_F->hide();
    ui->pushButton_L1->hide();
    ui->pushButton_U2->hide();

    initTransformForOpenGL();
    connect(this, &MainWindow::showCube, this, &MainWindow::on_constructButton_clicked);
    connect(this, &MainWindow::hideCube, this, &MainWindow::on_closeShowButton_clicked);
    // 关联旋转信号
    connect(this, &MainWindow::spinCube,this ,&MainWindow::spinCubeAction);
    // 展示魔方
    emit showCube();
}

MainWindow::~MainWindow()
{
    if(rcb !=nullptr)
    {
        rcb->close();
        rcb->deleteLater();
    }
    bdc = nullptr;
    solver = nullptr;
    emit ExitWin();
    delete ui;
}

// 打开摄像头
void MainWindow::on_openCameraButton_clicked()
{
    emit hideCube();
    if (rcb == nullptr) {
        rcb = new cubeDetect();
        qDebug() << "cubeDetect object created";
        connect(rcb, &cubeDetect::sendData, this, &MainWindow::get_once_info);
    }
    rcb->show();
    qDebug() << "cubeDetect window shown";
}

// 关闭摄像头
void MainWindow::on_closeCameraButton_clicked()
{
    if(rcb != nullptr)
    {
        rcb->close();
        rcb->deleteLater();
        rcb = nullptr;
    }
}

// 存储数据
void MainWindow::get_once_info(vector<string> infos)
{
    if(infos.size() < 9)
    {
        qDebug() << " the infos size is: "<< infos.size();
        return;
    }
    this->ruckCube[infos[4]] = infos;
    QString faceCenter = QString::fromStdString(infos[4]);
    for(int i = 1; i <= 9; i++)
    {
        QString buttonName = faceCenter + QString::number(i);
        QString color = QString::fromStdString(infos[i-1]);
//        qDebug() <<"color not found: " << color;
        changeBottonColor(buttonName, color);
    }
}

// 改变button的颜色，代表识别出的颜色
void MainWindow::changeBottonColor(const QString &buttonName, const QString &color)
{
    QPushButton *button = findChild<QPushButton *>(buttonName);
    if(button)
    {
        QString style = QString("background-color: %1").arg(color);
        button->setStyleSheet(style);
    }
    else
    {
        qDebug() <<"Button not found: " << buttonName;
    }
}

// 初始化图像数组
void MainWindow::initVertices()
{
    float arr[] = {
    //前蓝
        //中心块 //0
        -0.33f, 0.33f, 1.0f,    0.0f, 0.0f, 1.0f,
        -0.33f, -0.33f, 1.0f,   0.0f, 0.0f, 1.0f,
        0.33f, -0.33f, 1.0f,    0.0f, 0.0f, 1.0f,
        0.33f, 0.33f, 1.0f,     0.0f, 0.0f, 1.0f,
        //棱块 //1 left
        -0.34f, 0.33f, 1.0f,    0.0f, 0.0f, 1.0f,
        -0.34f, -0.33f, 1.0f,   0.0f, 0.0f, 1.0f,
        -1.0f, -0.33f, 1.0f,    0.0f, 0.0f, 1.0f,
        -1.0f, 0.33f, 1.0f,     0.0f, 0.0f, 1.0f,
    //2 right
        1.0f, 0.33f, 1.0f,      0.0f, 0.0f, 1.0f,
        1.0f, -0.33f, 1.0f,     0.0f, 0.0f, 1.0f,
        0.34f, -0.33f, 1.0f,    0.0f, 0.0f, 1.0f,
        0.34f, 0.33f, 1.0f,     0.0f, 0.0f, 1.0f,
    //3 down
        -0.33f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f,
        -0.33f, -0.34f, 1.0f,   0.0f, 0.0f, 1.0f,
        0.33f, -0.34f, 1.0f,    0.0f, 0.0f, 1.0f,
        0.33f, -1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    //4 up
        -0.33f, 0.34f, 1.0f,    0.0f, 0.0f, 1.0f,
        -0.33f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
        0.33f, 1.0f, 1.0f,      0.0f, 0.0f, 1.0f,
        0.33f, 0.34f, 1.0f,     0.0f, 0.0f, 1.0f,
        //角块 //5 左下
        -0.34f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f,
        -0.34f, -0.34f, 1.0f,   0.0f, 0.0f, 1.0f,
        -1.0f, -0.34f, 1.0f,    0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    //6 左上
        -0.34f, 0.34f, 1.0f,    0.0f, 0.0f, 1.0f,
        -0.34f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,
        -1.0f, 0.34f, 1.0f,     0.0f, 0.0f, 1.0f,
    //7 右下
        1.0f, -1.0f, 1.0f,      0.0f, 0.0f, 1.0f,
        1.0f, -0.34f, 1.0f,     0.0f, 0.0f, 1.0f,
        0.34f, -0.34f, 1.0f,    0.0f, 0.0f, 1.0f,
        0.34f, -1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    //8 右上
        1.0f, 0.34f, 1.0f,      0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
        0.34f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,
        0.34f, 0.34f, 1.0f,     0.0f, 0.0f, 1.0f,

    //后绿 //9
        -0.33f, 0.33f, -1.0f,   0.0f, 1.0f, 0.0f,
        -0.33f, -0.33f, -1.0f,  0.0f, 1.0f, 0.0f,
        0.33f, -0.33f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.33f, 0.33f, -1.0f,    0.0f, 1.0f, 0.0f,

        //棱块 //10 left
        -0.34f, 0.33f, -1.0f,   0.0f, 1.0f, 0.0f,
        -0.34f, -0.33f, -1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f, -0.33f, -1.0f,   0.0f, 1.0f, 0.0f,
        -1.0f, 0.33f, -1.0f,    0.0f, 1.0f, 0.0f,
    //11 right
        1.0f, 0.33f, -1.0f,   0.0f, 1.0f, 0.0f,
        1.0f, -0.33f, -1.0f,  0.0f, 1.0f, 0.0f,
        0.34f, -0.33f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.34f, 0.33f, -1.0f,    0.0f, 1.0f, 0.0f,
    //12 down
        -0.33f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        -0.33f, -0.34f, -1.0f,  0.0f, 1.0f, 0.0f,
        0.33f, -0.34f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.33f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,
    //13 up
        -0.33f, 0.34f, -1.0f,   0.0f, 1.0f, 0.0f,
        -0.33f, 1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        0.33f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.33f, 0.34f, -1.0f,    0.0f, 1.0f, 0.0f,
        //角块 //14 左下
        -0.34f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        -0.34f, -0.34f, -1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f, -0.34f, -1.0f,   0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,
    //15 左上
        -0.34f, 0.34f, -1.0f,   0.0f, 1.0f, 0.0f,
        -0.34f, 1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        -1.0f, 0.34f, -1.0f,    0.0f, 1.0f, 0.0f,
    //16 右下
        1.0f, -1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        1.0f, -0.34f, -1.0f,  0.0f, 1.0f, 0.0f,
        0.34f, -0.34f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.34f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,
    //17右上
        1.0f, 0.34f, -1.0f,   0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        0.34f, 1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.34f, 0.34f, -1.0f,    0.0f, 1.0f, 0.0f,

    //上黄 //18
        -0.33f, 1.0f, -0.33f,   1.0f, 1.0f, 0.0f,
        -0.33f, 1.0f, 0.33f,    1.0f, 1.0f, 0.0f,
        0.33f, 1.0f, 0.33f,     1.0f, 1.0f, 0.0f,
        0.33f, 1.0f, -0.33f,    1.0f, 1.0f, 0.0f,
        //棱块 //19 left
        -0.34f, 1.0f, -0.33f,   1.0f, 1.0f, 0.0f,
        -0.34f, 1.0f, 0.33f,    1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.33f,     1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -0.33f,    1.0f, 1.0f, 0.0f,
    //20 right
        1.0f, 1.0f, -0.33f,   1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.33f,    1.0f, 1.0f, 0.0f,
        0.34f, 1.0f, 0.33f,     1.0f, 1.0f, 0.0f,
        0.34f, 1.0f, -0.33f,    1.0f, 1.0f, 0.0f,
    //21 back
        -0.33f, 1.0f, -0.34f,   1.0f, 1.0f, 0.0f,
        -0.33f, 1.0f, -1.0f,    1.0f, 1.0f, 0.0f,
        0.33f, 1.0f, -1.0f,     1.0f, 1.0f, 0.0f,
        0.33f, 1.0f, -0.34f,    1.0f, 1.0f, 0.0f,
    //22 front
        -0.33f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f,
        -0.33f, 1.0f, 0.34f,    1.0f, 1.0f, 0.0f,
        0.33f, 1.0f, 0.34f,     1.0f, 1.0f, 0.0f,
        0.33f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f,
        //角块 //23 左后
        -0.34f, 1.0f, -0.34f,   1.0f, 1.0f, 0.0f,
        -0.34f, 1.0f, -1.0f,    1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -0.34f,    1.0f, 1.0f, 0.0f,
    //24 左前
        -0.34f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f,
        -0.34f, 1.0f, 0.34f,    1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.34f,     1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f,
    //25 右后
        1.0f, 1.0f, -0.34f,   1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,    1.0f, 1.0f, 0.0f,
        0.34f, 1.0f, -1.0f,     1.0f, 1.0f, 0.0f,
        0.34f, 1.0f, -0.34f,    1.0f, 1.0f, 0.0f,
    //26 右前
        1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.34f,    1.0f, 1.0f, 0.0f,
        0.34f, 1.0f, 0.34f,     1.0f, 1.0f, 0.0f,
        0.34f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f,

    //下白 //27
        -0.33f, -1.0f, -0.33f,  1.0f, 1.0f, 1.0f,
        -0.33f, -1.0f,  0.33f,  1.0f, 1.0f, 1.0f,
        0.33f, -1.0f,  0.33f,   1.0f, 1.0f, 1.0f,
        0.33f, -1.0f, -0.33f,   1.0f, 1.0f, 1.0f,
        //棱块 //28 左
        -0.34f, -1.0f, -0.33f,  1.0f, 1.0f, 1.0f,
        -0.34f, -1.0f,  0.33f,  1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f,  0.33f,   1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -0.33f,   1.0f, 1.0f, 1.0f,
    //29 右
        1.0f, -1.0f, -0.33f,  1.0f, 1.0f, 1.0f,
        1.0f, -1.0f,  0.33f,  1.0f, 1.0f, 1.0f,
        0.34f, -1.0f,  0.33f,   1.0f, 1.0f, 1.0f,
        0.34f, -1.0f, -0.33f,   1.0f, 1.0f, 1.0f,
    //30 后
        -0.33f, -1.0f, -0.34f,  1.0f, 1.0f, 1.0f,
        -0.33f, -1.0f,  -1.0f,  1.0f, 1.0f, 1.0f,
        0.33f, -1.0f,  -1.0f,   1.0f, 1.0f, 1.0f,
        0.33f, -1.0f, -0.34f,   1.0f, 1.0f, 1.0f,
    //31 前
        -0.33f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
        -0.33f, -1.0f,  0.34f,  1.0f, 1.0f, 1.0f,
        0.33f, -1.0f,  0.34f,   1.0f, 1.0f, 1.0f,
        0.33f, -1.0f, 1.0f,   1.0f, 1.0f, 1.0f,
        //角块 //32 左后
        -0.34f, -1.0f, -0.34f,  1.0f, 1.0f, 1.0f,
        -0.34f, -1.0f,  -1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f,  -1.0f,   1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -0.34f,   1.0f, 1.0f, 1.0f,
    //33 左前
        -0.34f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
        -0.34f, -1.0f,  0.34f,  1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f,  0.34f,   1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,   1.0f, 1.0f, 1.0f,
    //34 右后
        1.0f, -1.0f, -0.34f,  1.0f, 1.0f, 1.0f,
        1.0f, -1.0f,  -1.0f,  1.0f, 1.0f, 1.0f,
        0.34f, -1.0f,  -1.0f,   1.0f, 1.0f, 1.0f,
        0.34f, -1.0f, -0.34f,   1.0f, 1.0f, 1.0f,
    //35 右前
        1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
        1.0f, -1.0f,  0.34f,  1.0f, 1.0f, 1.0f,
        0.34f, -1.0f,  0.34f,   1.0f, 1.0f, 1.0f,
        0.34f, -1.0f, 1.0f,   1.0f, 1.0f, 1.0f,

    //左橙 36
        -1.0f, -0.33f, -0.33f,  1.0f, 0.5f, 0.0f,
        -1.0f, -0.33f,  0.33f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.33f,  0.33f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.33f, -0.33f,  1.0f, 0.5f, 0.0f,
        //棱块 //37 down
        -1.0f, -0.34f, -0.33f,  1.0f, 0.5f, 0.0f,
        -1.0f, -0.34f,  0.33f,  1.0f, 0.5f, 0.0f,
        -1.0f,  -1.0f,  0.33f,  1.0f, 0.5f, 0.0f,
        -1.0f,  -1.0f, -0.33f,  1.0f, 0.5f, 0.0f,
    //38 up
        -1.0f, 1.0f, -0.33f,  1.0f, 0.5f, 0.0f,
        -1.0f, 1.0f,  0.33f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.34f,  0.33f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.34f, -0.33f,  1.0f, 0.5f, 0.0f,
    //39 back
        -1.0f, -0.33f, -0.34f,  1.0f, 0.5f, 0.0f,
        -1.0f, -0.33f,  -1.0f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.33f,  -1.0f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.33f, -0.34f,  1.0f, 0.5f, 0.0f,
    //40 front
        -1.0f, -0.33f, 1.0f,  1.0f, 0.5f, 0.0f,
        -1.0f, -0.33f,  0.34f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.33f,  0.34f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.33f, 1.0f,  1.0f, 0.5f, 0.0f,
        //角块 //41 下后
        -1.0f, -0.34f, -0.34f,  1.0f, 0.5f, 0.0f,
        -1.0f, -0.34f,  -1.0f,  1.0f, 0.5f, 0.0f,
        -1.0f,  -1.0f,  -1.0f,  1.0f, 0.5f, 0.0f,
        -1.0f,  -1.0f, -0.34f,  1.0f, 0.5f, 0.0f,
    //42 下前
        -1.0f, -0.34f, 1.0f,  1.0f, 0.5f, 0.0f,
        -1.0f, -0.34f,  0.34f,  1.0f, 0.5f, 0.0f,
        -1.0f,  -1.0f,  0.34f,  1.0f, 0.5f, 0.0f,
        -1.0f,  -1.0f, 1.0f,  1.0f, 0.5f, 0.0f,
    //43 上后
        -1.0f, 1.0f, -0.34f,  1.0f, 0.5f, 0.0f,
        -1.0f, 1.0f,  -1.0f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.34f,  -1.0f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.34f, -0.34f,  1.0f, 0.5f, 0.0f,
    //44 上前
        -1.0f, 1.0f, 1.0f,  1.0f, 0.5f, 0.0f,
        -1.0f, 1.0f,  0.34f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.34f,  0.34f,  1.0f, 0.5f, 0.0f,
        -1.0f,  0.34f, 1.0f,  1.0f, 0.5f, 0.0f,

    //右红 //45
        1.0f, -0.33f, -0.33f,   1.0f, 0.0f, 0.0f,
        1.0f, -0.33f,  0.33f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.33f,  0.33f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.33f, -0.33f,   1.0f, 0.0f, 0.0f,
        //棱块 //46 down
        1.0f, -0.34f, -0.33f,   1.0f, 0.0f, 0.0f,
        1.0f, -0.34f,  0.33f,   1.0f, 0.0f, 0.0f,
        1.0f,  -1.0f,  0.33f,   1.0f, 0.0f, 0.0f,
        1.0f,  -1.0f, -0.33f,   1.0f, 0.0f, 0.0f,
    //47 up
        1.0f, 1.0f, -0.33f,   1.0f, 0.0f, 0.0f,
        1.0f, 1.0f,  0.33f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.34f,  0.33f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.34f, -0.33f,   1.0f, 0.0f, 0.0f,
    //48 back
        1.0f, -0.33f, -0.34f,   1.0f, 0.0f, 0.0f,
        1.0f, -0.33f,  -1.0f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.33f,  -1.0f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.33f, -0.34f,   1.0f, 0.0f, 0.0f,
    //49 front
        1.0f, -0.33f, 1.0f,   1.0f, 0.0f, 0.0f,
        1.0f, -0.33f,  0.34f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.33f,  0.34f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.33f, 1.0f,   1.0f, 0.0f, 0.0f,
        //角块 //50 下后
        1.0f, -0.34f, -0.34f,   1.0f, 0.0f, 0.0f,
        1.0f, -0.34f,  -1.0f,   1.0f, 0.0f, 0.0f,
        1.0f,  -1.0f,  -1.0f,   1.0f, 0.0f, 0.0f,
        1.0f,  -1.0f, -0.34f,   1.0f, 0.0f, 0.0f,
    //51 下前
        1.0f, -0.34f, 1.0f,   1.0f, 0.0f, 0.0f,
        1.0f, -0.34f,  0.34f,   1.0f, 0.0f, 0.0f,
        1.0f,  -1.0f,  0.34f,   1.0f, 0.0f, 0.0f,
        1.0f,  -1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
    //52 上后
        1.0f, 1.0f, -0.34f,   1.0f, 0.0f, 0.0f,
        1.0f, 1.0f,  -1.0f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.34f,  -1.0f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.34f, -0.34f,   1.0f, 0.0f, 0.0f,
    //53 上前
        1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
        1.0f, 1.0f,  0.34f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.34f,  0.34f,   1.0f, 0.0f, 0.0f,
        1.0f,  0.34f, 1.0f,   1.0f, 0.0f, 0.0f,
    };
    for(int i = 0; i< 5184; i++)
    {
        this->vertices[i] = arr[i];
    }
}

// 颜色变换数组
void MainWindow::initTransformForOpenGL()
{
    this->openGLCubeColor = {{"blue", {0.0f, 0.0f, 1.0f}},
                             {"green",{ 0.0f, 1.0f, 0.0f}},
                             {"yellow", {1.0f, 1.0f, 0.0f}},
                             {"white",{1.0f, 1.0f, 1.0f}},
                             {"orange",{1.0f, 0.5f, 0.0f}},
                             {"red",{1.0f, -0.33f, -0.33f}}
                            };
    this->tranIndex = {
        {"blue",{4,3,5,7,1,6,0,8,2}},
        {"green",{4,5,3,7,1,8,2,6,0}},
        {"yellow",{4,3,5,1,7,0,6,2,8}},
        {"white",{4,5,3,7,1,6,0,8,2}},
        {"orange",{4,7,1,3,5,6,8,0,2}},
        {"red",{4,7,1,5,3,8,6,2,0}}
    };
}

// 修改map中的數據使其與渲染的順序一致
void MainWindow::drawingCubeColor()
{
    this->cubeForGL = this->ruckCube;
    // 修改map中的數據使其與渲染的順序一致
    for(int i = 0; i < 6; i++)
    {
        string curFace = this->COLORS[i];
        vector<int> trans = this->tranIndex[curFace];
        for(int j = 0; j < 9; j++)
        {
            this->cubeForGL[curFace][j] = this->ruckCube[curFace][trans[j]];
        }
    }

    int arrIndex = 0;
    // 六面
    for(int i = 0; i < 6; i++)
    {
        string curFace = this->COLORS[i];
        vector<string> faceColors = this->cubeForGL[curFace];
        // 九個方塊
        for(int c = 0; c < 9; c++)
        {
            string curColor = faceColors[c];
            vector<float> glColor = this->openGLCubeColor[curColor];
            // 一個方塊需要四次渲染
            for(int j = 0; j < 4; j++)
            {
                // 一次渲染需要三個數字
                for(int k = 0; k < 3; k++)
                {
                    this->vertices[arrIndex+3+k] = glColor[k];
                }
                arrIndex += 6;
            }
        }
    }
}

// 展示3D魔方
void MainWindow::on_constructButton_clicked()
{
    if(this->ruckCube.size() == 6)
    {
        CubieCube cforccb(this->ruckCube);
        if(!cforccb.checkValid())
        {
            QMessageBox::information(this, "提示", "识别出的魔方非法，展现原始魔方。");
            initVertices();
        }else{
            this->ccb = cforccb;
            drawingCubeColor();
            // 将魔方置为非还原状态
            this->reduction = false;
        }
    }else{
        initVertices();
    }
    opengl_showCube();
}

void MainWindow::opengl_showCube()
{
    // 将原先的动态内存释放
    bdc = make_shared<BuildCube>(nullptr, this->vertices);

    connect(this, &MainWindow::sendStrToCube, bdc.get(), &BuildCube::rotateCube);
    connect(bdc.get(), &BuildCube::sendMoveLabel, this, &MainWindow::showMoveLabel);
    connect(bdc.get(), &BuildCube::spinOver, this,&MainWindow::spinOverDeal);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(bdc.get());

    // 删除旧的布局（如果有）
    if (ui->openGLWidget->layout() != nullptr)
    {
        QLayoutItem* item;
        while ((item = ui->openGLWidget->layout()->takeAt(0)) != nullptr)
        {
            if (item->widget() != nullptr)
            {
                item->widget()->deleteLater(); // 使用 deleteLater 而不是 delete
            }
            delete item;
        }
        delete ui->openGLWidget->layout();
    }
    ui->openGLWidget->setLayout(layout);

    ui->openGLWidget->show();
    bdc->setFocus();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    emit ExitWin();
}

void MainWindow::on_pushButton_4_clicked()
{
    if(this->ruckCube.size() == 6)
    {
        for(auto rc: this->ruckCube)
        {
            string center = rc.first;
            vector<string> infos = rc.second;
            cout << "{ \"" << center << "\", { ";
            for(auto inf: infos)
            {
               cout << "\""<< inf <<"\", ";
            }
            cout <<"}}, " <<endl;
        }
    }
}

void MainWindow::on_solveButton_clicked()
{
    QString inputInfo = ui->inputLabel->text();
    bool gotAns = false;
    // 如果非还原状态：直接求解
    if(this->reduction == false)
    {
        if(inputInfo.size() != 0)
        {
            QMessageBox::information(this, "提示", "需要对打乱公式求解请先还原魔方，否则请清空打乱公式");
        }else{
            this->answerLatex = solver->getSolveLatex(this->ccb);
            gotAns = true;
        }
    }
    else
    {
        // 获取打乱公式
        std::istringstream iss(inputInfo.toStdString());
        vector<string> latex;
        string str;
        bool check = true;
        while(iss >> str)
        {
            if(moveMap.find(str) == moveMap.end())
            {
                // 校验输入内容是否合法
                QMessageBox::information(this,"提示", "输入不规范，请重试。");
                check = false;
                break;
            }
            latex.push_back(str);
        }

        if(check)
        {
            this->ccb = CubieCube(latex);
            // 获取求解公式
            this->answerLatex = solver->getSolveLatex(this->ccb);
            gotAns = true;
            qmtx.lock();
            // 通过latex进行旋转
            emit spinCube(latex);
            this->reduction = false;
            ui->infoLabel->setText("正在\n打乱\n魔方\n请稍后\n");
            qmtx.unlock();
        }
    }

    if(gotAns)
    {
        // 旋转完再给出公式
        QString ans;
        for(auto a: answerLatex)
        {
            ans.append(QString::fromStdString(a));
            ans.append(" ");
        }
        // 展示结果
        ui->ansLabel->setText(ans);
    }
}

// 关闭展示
void MainWindow::on_closeShowButton_clicked()
{
    ui->openGLWidget->hide();
}

void MainWindow::on_pushButton_U2_clicked()
{
    emit sendStrToCube("U2");
}

void MainWindow::on_pushButton_L1_clicked()
{
    emit sendStrToCube("L'");
}

void MainWindow::on_pushButton_F_clicked()
{
    emit sendStrToCube("F");
}

// 进行动画旋转旋转
void MainWindow::on_startSolveButton_clicked()
{
    if(this->reduction == false){
        qmtx.lock();
        vector<string> &ans = this->answerLatex;
        emit spinCube(ans);
        qmtx.unlock();
        this->reduction = true;             // 代表魔方已还原
    }else
    {
        QMessageBox::information(this,"提示","魔方已回到还原状态。");
    }

}

void MainWindow::showMoveLabel(QString m)
{
    ui->moveLabel->setText(m);
}

void MainWindow::spinCubeAction(vector<string> moves)
{
    for(auto s: moves)
    {
        cout << s;
        QString qstr = QString::fromStdString(s);
        emit sendStrToCube(qstr);
        QThread::msleep(10); // 添加50毫秒的延迟
        // 如果包含2就转两次
        if(qstr.contains("2"))
        {
            cout << s;
            emit sendStrToCube(qstr);
            QThread::msleep(10); // 添加50毫秒的延迟
        }
    }
}


void MainWindow::on_restoreButton_clicked()
{
    initVertices();
    opengl_showCube();
    this->reduction = true;           // 代表魔方已还原
    ui->infoLabel->clear();
    ui->moveLabel->clear();
}

void MainWindow::spinOverDeal()
{
    ui->infoLabel->clear();
    ui->moveLabel->clear();
}
