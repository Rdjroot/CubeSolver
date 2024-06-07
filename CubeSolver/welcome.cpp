#include "welcome.h"
#include "ui_welcome.h"

Welcome::Welcome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Welcome),
    solver(nullptr),
    mw(nullptr)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/Resource/cubeIcon.png"));
    connect(this,&Welcome::initTables,this,&Welcome::initSolver);
    connect(this,&Welcome::calAnswer,this,&Welcome::getResult);
    emit initTables();
}

Welcome::~Welcome()
{
    if(mw != nullptr)
    {
        mw->close();
        mw->deleteLater();
    }
    solver = nullptr;
    delete ui;
}

// 在后台初始化solver所有的表
void Welcome::initSolver()
{
    this->solver = Solver::getInstance();
}

// 通过打乱公式获取结果
void Welcome::on_pushButton_clicked()
{
    QString inputInfo = ui->latexInput->text();
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
        CubieCube originCube(latex);
        ui->latexOutput->setText("正在计算，请稍等...");
        emit calAnswer(originCube);
    }
}

void Welcome::getResult(CubieCube originCube)
{
    vector<string> results = solver->getSolveLatex(originCube);
    QString res;
    for(auto r: results)
    {
        res += QString::fromStdString(r) + " ";
    }
    ui->latexOutput->clear();
    ui->latexOutput->setText(res);
}



void Welcome::on_scanUsing_clicked()
{
    if(mw == nullptr)
    {
        mw = new MainWindow();
    }
    mw->show();
    this->hide();
    connect(mw,MainWindow::ExitWin,this,Welcome::mwClose);
}

void Welcome::mwClose()
{
    this->show();
}
