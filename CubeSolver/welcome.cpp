#include "welcome.h"
#include "ui_welcome.h"

Welcome::Welcome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);
        this->setWindowIcon(QIcon(":/Resource/cubeIcon.png"));
}

Welcome::~Welcome()
{
    delete ui;
}
