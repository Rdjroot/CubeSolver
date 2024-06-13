#include "cubedetect.h"
#include "ui_cubedetect.h"

cubeDetect::cubeDetect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cubeDetect),
    camera_t(new CameraThread()),
    mtx(new QMutex())
{
    ui->setupUi(this);
    // 设置窗口大小
    this->setFixedSize(800,600);
    // 固定摄像画幅大小
    ui->label->setFixedSize(640,480);
    this->setWindowIcon(QIcon(":/Resource/cubeIcon.png"));

    this->setWindowTitle("Rubik's Cube recognition");

    // 注册 Mat 类型
    qRegisterMetaType<cv::Mat>("cv::Mat");

    // 获取摄像机画面并展示
    connect(camera_t, &CameraThread::frameReady, this, &cubeDetect::dealImgInfo);
    connect(camera_t, &CameraThread::framePause, this, &cubeDetect::pauseImg);
    // 获取摄像头失败
    connect(camera_t, &CameraThread::cameraFaild, this,&cubeDetect::getCameraFailed);

    // 画面暂停还是继续分析
    connect(this, &cubeDetect::changeFlag, camera_t, &CameraThread::runOrPause);

    // 画面中的魔方面识别框左上角和右下角位置
    this->Frame = { {90,90},{450, 450} };

    // 设置九个识别点位
    this->ninePoints = { {{130, 130},{170,170}},{{250,130},{290,170}},{{370,130},{410,170}},
                         {{130, 250},{170,290}},{{250,250},{290,290}},{{370,250},{410,290}},
                         {{130,370}, {170,410}},{{250,370},{290,410}},{{370,370},{410,410}}
                       };
    // 六个颜色的HSV值
    this->colorsHSV = { {63, 179,0, 148,108, 255},  // white
                        {18, 51,56, 179,91, 255},	   // yellow
                        { 98, 136,109, 255,35, 113},		// blue
                        { 0, 0,177, 255,61, 129},		// red
                        { 58, 86,85, 255,29, 255},		// green
                        {  2, 29,105, 199,72, 255}   // orange
                      };
    // 标记颜色
    this->myColorValues = { {255, 255, 255},		// white
                            {0, 255, 255},			// yellow
                            {255, 0, 0},			// blue
                            {0, 0, 255},			// red
                            {0, 255, 0},			// green
                            {0, 165, 255},			// orange
                          };

    this->colorMap = { {0, "white"},{1,"yellow"},{2,"blue"} ,
                       {3,"red"},{4,"green"},{5,"orange"} };
}

cubeDetect::~cubeDetect()
{
//    cout<< "delete this "<<endl;
    if(camera_t && camera_t->isRunning())
    {
        camera_t->stop();
    }
    delete camera_t;        // 去掉线程
    delete mtx;
    delete ui;
}

void cubeDetect::closeEvent(QCloseEvent *event)
{
    emit cbdExitWin();
}



void cubeDetect::on_saveButton_clicked()
{
    // 判断是否获取到了数据
    if(this->cache.size() == 9)
    {
//        string center = this->cache[4];
//        this->sixFaces.insert({ center, cache });
        emit sendData(this->cache);
    }else{
        qDebug() << "[Warning]send info faild, the info size is :" << this->cache.size();
    }
}

void cubeDetect::on_startButton_clicked()
{
    if(!camera_t->isRunning())
    {
        // 启动线程
        camera_t->start();
    }
    // 输出标志位是互斥量
    mtx->lock();
    emit changeFlag(true);
    mtx->unlock();

}

void cubeDetect::on_pauseButton_clicked()
{
    if(camera_t->isRunning())
    {
        mtx->lock();
        // 稍微暂停一下
        camera_t->requestInterruption();
        emit changeFlag(false);
        mtx->unlock();
    }
}

// 暂停的画面展示
void cubeDetect::pauseImg(cv::Mat img)
{
     cv::circle(myImg, cv::Point(400, 50), 20, cv::Scalar(0, 0, 255),cv::FILLED);
     MatToQImage();
}

void cubeDetect::getCameraFailed()
{
    QMessageBox::information(this, "警告", "无法获取系统摄像头，请检查！");
}

void cubeDetect::dealImgInfo(cv::Mat img)
{
    if(img.empty())
    {
        qDebug() <<"receive img is empty!";
        return;
    }

    this->myImg = img;

    // 描绘识别框
    cv::rectangle(this->myImg, Frame[0], Frame[1], cv::Scalar(255, 255,255), 2);
    cv::Mat divCube,divHSV;
    vector<string> temp;

    // 魔方一面的九个色块识别
    for (int i = 0; i < 9; i++)
    {
        // 裁剪色块
        cv::Rect roi(ninePoints[i][0],ninePoints[i][1]);
        divCube = this->myImg(roi);
        cv::cvtColor(divCube, divHSV, cv::COLOR_BGR2HSV);

        // 定义的六种颜色将会被识别
        for (int j = 0; j < colorsHSV.size(); j++) {
            // 设置阈值
            cv::Scalar lower(colorsHSV[j][0], colorsHSV[j][2], colorsHSV[j][4]);
            cv::Scalar upper(colorsHSV[j][1], colorsHSV[j][3], colorsHSV[j][5]);
            cv::Mat mask;

            // 匹配颜色，将会是白色块，其余是黑色
            inRange(divHSV, lower, upper, mask);

            // 在原图中勾勒获取识别出的颜色
            string color = getContours(mask, j, i);

            // 成功识别
            if (!color.empty()) {
                temp.push_back(color);
                break;
            }
        }
    }

    // 如果识别出的是9个色块,存放在cache里
    if(temp.size() == 9)
        this->cache = temp;
    MatToQImage();
}

// 获取当前色块颜色
string cubeDetect::getContours(cv::Mat imgDil, const int colorInt, const int pos)
{
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;

    // 根据二值图勾勒轮廓
    cv::findContours(imgDil, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 如果有轮廓，代表颜色对上了
    if (!contours.empty())
    {
        // 描绘对应的颜色识别框，表示当前识别出来是什么颜色
        cv::rectangle(this->myImg, ninePoints[pos][0], ninePoints[pos][1], cv::Scalar(myColorValues[colorInt]), 2);
        return colorMap[colorInt];
    }
    return "";
}

// Mat 转QImgae 类型在label展示摄像头画面
void cubeDetect::MatToQImage()
{
    cv::Mat rgb;
    QImage img;
    try{
        if(this->myImg.channels() == 3)
        {
            cv::cvtColor(this->myImg, rgb, CV_BGR2RGB);
            img = QImage((const uchar*)(rgb.data), rgb.cols, rgb.rows, rgb.cols * rgb.channels(), QImage::Format_RGB888);
        }
        else
        {
            img = QImage((const uchar*)(this->myImg.data),this->myImg.cols, this->myImg.rows, this->myImg.cols*this->myImg.channels(),QImage::Format_Indexed8 );
        }
    }catch (const cv::Exception &e) {
        qDebug() << "OpenCV exception caught:" << e.what();
    } catch (const std::exception &e) {
        qDebug() << "Standard exception caught:" << e.what();
    } catch (...) {
        qDebug() << "Unknown exception caught";
    }

    ui->label->setPixmap(QPixmap::fromImage(img));
}

