#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cubedetect.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , rcb(nullptr)
{
    this->setWindowIcon(QIcon(":/Resource/cubeIcon.png"));

    ui->setupUi(this);
    ui->blue5->setStyleSheet("background-color: blue");
    ui->white5->setStyleSheet("background-color: white");
    ui->orange5->setStyleSheet("background-color: orange");
    ui->red5->setStyleSheet("background-color: red");
    ui->green5->setStyleSheet("background-color: green");
    ui->yellow5->setStyleSheet("background-color: yellow");
}

MainWindow::~MainWindow()
{
    if(rcb !=nullptr)
    {
        rcb->close();
        rcb->deleteLater();
    }
    delete ui;
}

void MainWindow::on_openCameraButton_clicked()
{
    if (rcb == nullptr) {
        rcb = new cubeDetect();
        qDebug() << "cubeDetect object created";
        connect(rcb, &cubeDetect::sendData, this, &MainWindow::get_once_info);
    }
    rcb->show();
    qDebug() << "cubeDetect window shown";
}

void MainWindow::on_closeCameraButton_clicked()
{
    if(rcb != nullptr)
    {
        rcb->close();
        rcb->deleteLater();
        rcb = nullptr;
    }
}

void MainWindow::get_once_info(vector<string> infos)
{
    if(infos.size() < 9)
    {
        qDebug() << " the infos size is: "<< infos.size();
        return;
    }
    this->ruckCube.insert({infos[4], infos});
    QString faceCenter = QString::fromStdString(infos[4]);
    for(int i = 1; i <= 9; i++)
    {
        QString buttonName = faceCenter + QString::number(i);
        QString color = QString::fromStdString(infos[i-1]);
        qDebug() <<"color not found: " << color;
        changeBottonColor(buttonName,color);
    }
}

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


