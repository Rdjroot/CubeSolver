#include "camerathread.h"

CameraThread::CameraThread()
    :QThread(),
      running(false),
      flag(true)
{

}

CameraThread::~CameraThread()
{
    running = false;
    wait();             // 等待所有线程任务结束
    if(cap.isOpened())
    {
        cap.release(); // 释放摄像头资源
    }
//    qDebug() <<"thread over!!";
}

// 停止线程运作
void CameraThread::stop()
{
    running = false;
    requestInterruption();
}

// 打开摄像头，获取画面
// 根据标志位向cubedetect传递信号和画面信息
void CameraThread::run()
{
//    qDebug() << "run start!" ;
    cap.open(1);            // 开启的摄像头序列号
    if(!cap.isOpened())
    {
        // 如果获取不到1，尝试获取0
        cap.open(0);
        if(!cap.isOpened()){
            emit cameraFaild();
            return;
        }
    }

    running = true;

    /**
    * 为了让展示画面流畅
    * 指定视频编码格式为CV_CAP_PROP_FOURCC
    * M J P G 表示Motion JPEG编码格式
    * 将每一帧视频压缩为一个JPEG图像
    */
    cap.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
    cv::Mat frame;
    while(running)
    {
        cap.read(frame);

        if(frame.empty())
        {
            continue;
            qDebug() <<" get frame is failed";
        }

        if(flag){
            // 分析画面
            emit frameReady(frame);
        } else
        {
            // 暂停画面
            emit framePause(frame);
        }

        // 线程休眠15毫秒
        QThread::msleep(15);
    }
//    qDebug() << "cap is release";
    cap.release();
}

void CameraThread::runOrPause(const bool f)
{
    // 修改输出画面标志位
    this->flag = f;
}

