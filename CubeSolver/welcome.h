#ifndef WELCOME_H
#define WELCOME_H

#include <QWidget>
#include <QMessageBox>
#include "solver.h"
#include "cubiecube.h"
#include "sstream"
#include<vector>
#include "mainwindow.h"

namespace Ui {
class Welcome;
}

class Solver;
class CubieCube;

class Welcome : public QWidget
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome();

private:
    Ui::Welcome *ui;
    Solver* solver;
    MainWindow *mw;

signals:
    void initTables();
    void calAnswer(CubieCube originCube);

private slots:
    void initSolver();
    void on_pushButton_clicked();
    void getResult(CubieCube originCube);
    void on_scanUsing_clicked();
    void mwClose();
};

#endif // WELCOME_H
