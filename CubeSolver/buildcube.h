#ifndef BUILDCUBE_H
#define BUILDCUBE_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QVector>
#include <QVector3D>
#include <QSet>

QT_BEGIN_NAMESPACE
namespace Ui {
class BuildCube;
}
QT_END_NAMESPACE

class QOpenGLShaderProgram;
class QOpenGLTexture;

// 构造魔方3D图像展示
class BuildCube : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    enum Surface{FRONT, BACK, UP, DOWN, LEFT, RIGHT};
    explicit BuildCube(QWidget *parent = nullptr, float *arr = nullptr);
    ~BuildCube();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void keyPressEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;


private:
    Ui::BuildCube *ui;
    float vertices[5184];
    QOpenGLShaderProgram *program;
    QOpenGLBuffer VBO;
    GLfloat translate, xRot, yRot, zRot;
    int m_key;
    QSet<int> setList[6];
    QSet<int> ans; //涉及到变换的Index集合
    QMatrix4x4 matrix[54];
    int m_anti = 1;
};

#endif // BUILDCUBE_H
