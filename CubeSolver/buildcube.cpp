#include "buildcube.h"
#include "ui_buildcube.h"

QSet<int> allIndex;
BuildCube::BuildCube(QWidget *parent, float *arr)
    : QOpenGLWidget(parent), xRot(0.0f), yRot(0.0f), zRot(0.0f), m_key(-794)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    setFormat(format);

    this->m_command = "";
    this->commandQueue.clear();
    this->speed = 40;
    // 初始化魔方色块界面
    for(int i = 0; i < 5184; i++)
    {
        this->vertices[i] = arr[i];
    }
    translate = -6.0;
    for(int i=0; i<54; ++i)//初始化全部包含
        allIndex.insert(i);
    ans = allIndex;
    //按面插入
    for(int i=0; i<6; ++i){
        for(int j=i*9; j<i*9+9; ++j){
            setList[i].insert(j);
        }
    }
    //插入棱
    for(int j: {1, 10}){//j分别取1和10
        for(auto i: {LEFT, RIGHT, DOWN, UP}){
            setList[i].insert(j++);
        }
    }
    for(int j: {19, 28}){
        for(auto i: {LEFT, RIGHT, BACK, FRONT}){
            setList[i].insert(j++);
        }
    }
    for(int j: {37, 46}){
        for(auto i: {DOWN, UP, BACK, FRONT}){
            setList[i].insert(j++);
        }
    }
    //角块
    for(int j: {5, 14}){
        for(auto i: {LEFT, RIGHT}){
            for(auto k: {DOWN, UP}){
                setList[i].insert(j);
                setList[k].insert(j++);
            }
        }
    }
    for(int j: {23, 32}){
        for(auto i: {LEFT, RIGHT}){
            for(auto k: {BACK, FRONT}){
                setList[i].insert(j);
                setList[k].insert(j++);
            }
        }
    }
    for(int j: {41, 50}){
        for(auto i: {DOWN, UP}){
            for(auto k: {BACK, FRONT}){
                setList[i].insert(j);
                setList[k].insert(j++);
            }
        }
    }
}

BuildCube::~BuildCube()
{
    makeCurrent();
    VAO.destroy();
    VBO.destroy();
    delete program;
}

void BuildCube::initializeGL()
{
    initializeOpenGLFunctions();

    qInfo() << "BuildCube initializeGL context"
            << QOpenGLContext::currentContext()
            << "format" << context()->format().majorVersion() << context()->format().minorVersion()
            << "size" << size();

    glEnable(GL_DEPTH_TEST);

    program = new QOpenGLShaderProgram;
    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shapes.vert"))
    {
        qCritical() << "Vertex shader load failed" << program->log();
        return;
    }
    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shapes.frag"))
    {
        qCritical() << "Fragment shader load failed" << program->log();
        return;
    }
    if (!program->link())
    {
        qCritical() << "Shader link failed" << program->log();
        return;
    }
    program->bind();

    QMatrix4x4 model;
    model.perspective(45.0f, static_cast<GLfloat>(width()) / static_cast<GLfloat>(qMax(1, height())), 0.1f, 100.0f);
    model.translate(0, 0, translate);
    model.rotate(30, 1.0, 0.0, 0.0);
    model.rotate(-30, 0.0, 1.0, 0.0);
    program->setUniformValue("model", model);

    for (QMatrix4x4 &matrixItem : matrix)
    {
        matrixItem.setToIdentity();
    }

    VAO.create();
    VAO.bind();

    VBO.create();
    VBO.bind();
    VBO.allocate(this->vertices, sizeof(this->vertices));

    GLuint vPosition = program->attributeLocation("vPosition");
    program->enableAttributeArray(vPosition);
    program->setAttributeBuffer(vPosition, GL_FLOAT, 0, 3, 6 * sizeof(GL_FLOAT));

    GLuint vColor = program->attributeLocation("aColor");
    program->enableAttributeArray(vColor);
    program->setAttributeBuffer(vColor, GL_FLOAT, 3 * sizeof(GL_FLOAT), 3, 6 * sizeof(GL_FLOAT));

    VAO.release();
    VBO.release();
}

void BuildCube::paintGL()
{
    if (program == nullptr || !program->isLinked())
    {
        qWarning() << "paintGL skipped because shader program is not ready";
        return;
    }

    static bool firstPaint = true;
    if (firstPaint)
    {
        qInfo() << "BuildCube paintGL first frame";
        firstPaint = false;
    }

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->bind();
    VAO.bind();
    for(int i=0; i<54; i++) {
        program->setUniformValue("matrix", matrix[i]);
        glDrawArrays(GL_TRIANGLE_FAN, i*4, 4);
    }
    VAO.release();
    program->release();
}

void BuildCube::resizeGL(int, int){}

/*
void BuildCube::keyPressEvent(QKeyEvent *event)
{
    if(m_key == -794){//防止点击过快
        if(event->modifiers() == Qt::ShiftModifier){
            m_anti = -1;
        }
        m_key = event->key();
        //确定操作对象
        if(m_key == Qt::Key_X || m_key == Qt::Key_Y || m_key == Qt::Key_Z){
            ans = allIndex;
        }else if(m_key == Qt::Key_R){
            //得出结果集，确认哪些面块需要旋转。
            ans = setList[RIGHT];
        }else if(m_key == Qt::Key_F){
            ans = setList[FRONT];
        }else if(m_key == Qt::Key_B){
            ans = setList[BACK];
        }else if(m_key == Qt::Key_L){
            ans = setList[LEFT];
        }else if(m_key == Qt::Key_U){
            ans = setList[UP];
        }else if(m_key == Qt::Key_D){
            ans = setList[DOWN];
        }else{
            m_key = -794;
            return;//无关的键屏蔽
        }
        startTimer(10);
    }
    QOpenGLWidget::keyPressEvent(event);
}


void BuildCube::timerEvent(QTimerEvent *event)
{
    static int m_count = 0;
    m_count++;
    switch (m_key) {
    case Qt::Key_X: case Qt::Key_R:
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            // m_anti 控制方向
            tmp.rotate(-10*m_anti, 1.0f, 0.0f, 0.0f);
            matrix[x] = tmp * matrix[x];
        }
        break;
    case Qt::Key_L:
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            tmp.rotate(10*m_anti, 1.0f, 0.0f, 0.0f);
            matrix[x] = tmp * matrix[x];
        }
        break;
    case Qt::Key_Y: case Qt::Key_U:
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            tmp.rotate(-10*m_anti, 0.0f, 1.0f, 0.0f);
            matrix[x] = tmp * matrix[x];
        }
        break;
    case Qt::Key_D:
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            tmp.rotate(10*m_anti, 0.0f, 1.0f, 0.0f);
            matrix[x] = tmp * matrix[x];
        }
        break;
    case Qt::Key_Z: case Qt::Key_F:
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            tmp.rotate(-10*m_anti, 0.0f, 0.0f, 1.0f);
            matrix[x] = tmp * matrix[x];
        }
        break;
    case Qt::Key_B:
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            tmp.rotate(10*m_anti, 0.0f, 0.0f, 1.0f);
            matrix[x] = tmp * matrix[x];
        }
        break;
    default:
        break;
    }
    update();
    if(m_count == 9){//结束
        m_count = 0;
        QSet<int> tmp;
        int *que;
        switch (m_key) {
        case Qt::Key_X:case Qt::Key_Y:case Qt::Key_Z:
            if(m_key == Qt::Key_X){
                tmp = setList[FRONT];
                que = new int[4]{FRONT, DOWN, BACK, UP};
            }else if(m_key == Qt::Key_Y){
                tmp = setList[FRONT];
                que = new int[4]{FRONT, RIGHT, BACK, LEFT};
            }else if(m_key == Qt::Key_Z){
                tmp = setList[UP];
                que = new int[4]{UP, LEFT, DOWN, RIGHT};
            }
            if(m_anti == -1){
                qSwap(que[1], que[3]);
            }
            for(int i=0; i<3; ++i){
                setList[que[i]] = setList[que[i+1]];
            }
            setList[que[3]] = tmp;
            break;
        default:
            if(m_key == Qt::Key_R){
                tmp=ans & setList[FRONT];
                que = new int[4]{FRONT, DOWN, BACK, UP};
            }else if(m_key == Qt::Key_L){
                tmp=ans & setList[FRONT];
                que = new int[4]{FRONT, UP, BACK, DOWN};
            }else if(m_key == Qt::Key_U){
                tmp=ans & setList[FRONT];
                que = new int[4]{FRONT, RIGHT, BACK, LEFT};
            }else if(m_key == Qt::Key_D){
                tmp=ans & setList[FRONT];
                que = new int[4]{FRONT, LEFT, BACK, RIGHT};
            }else if(m_key == Qt::Key_F){
                tmp=ans & setList[UP];
                que = new int[4]{UP, LEFT, DOWN, RIGHT};
            }else if(m_key == Qt::Key_B){
                tmp=ans & setList[UP];
                que = new int[4]{UP, RIGHT, DOWN, LEFT};
            }
            if(m_anti == -1){
                qSwap(que[1], que[3]);
            }
            for(int i=0; i<4; ++i){
                setList[que[i]].subtract(ans);
                if(i<3)
                    setList[que[i]].unite(ans & setList[que[i+1]]);
                else
                    setList[que[3]].unite(tmp);
            }
            break;
        }
        delete[] que;
        m_key = -794;
        m_anti = 1;
        killTimer(event->timerId());
    }
}

*/

void BuildCube::rotateCube(QString mv)
{
    // 命令入队
    commandQueue.enqueue(mv);
}

void BuildCube::execCommand(int speed)
{
    m_command.clear();
    this->speed = speed;
    processNextCommand();
}

void BuildCube::clearCommend()
{
    this->commandQueue.clear();
    this->m_command.clear();
}

void BuildCube::processNextCommand()
{
    if(commandQueue.isEmpty())
        return;

    m_command = commandQueue.dequeue();
    QChar pm = '\'';

    // 查看旋转方式是否为逆时针
    if(m_command.indexOf(pm) != -1)
    {
        m_anti = -1;
    }else
    {
        m_anti = 1;
    }

    if(m_command.contains("R"))
    {
        ans = setList[RIGHT];
    }else if(m_command.contains("F"))
    {
        ans = setList[FRONT];
    }else if(m_command.contains("B"))
    {
        ans = setList[BACK];
    }else if(m_command.contains("L"))
    {
        ans = setList[LEFT];
    }else if(m_command.contains("U"))
    {
        ans = setList[UP];
    }else if(m_command.contains("D"))
    {
        ans = setList[DOWN];
    }else{
        m_command.clear();
        processNextCommand();
        return;         // 屏蔽无关字符
    }
    startTimer(this->speed);
}

void BuildCube::timerEvent(QTimerEvent *event)
{
    QThread::msleep(this->speed);

    static int m_count = 0;
    m_count++;
    if(m_command.contains("R"))
    {
        emit sendMoveLabel(m_command);
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            // m_anti 控制方向
            tmp.rotate(-10*m_anti, 1.0f, 0.0f, 0.0f);
            matrix[x] = tmp * matrix[x];
        }
    }else if(m_command.contains("L"))
    {
        emit sendMoveLabel(m_command);
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            tmp.rotate(10*m_anti, 1.0f, 0.0f, 0.0f);
            matrix[x] = tmp * matrix[x];
        }
    }else if(m_command.contains("U"))
    {
        emit sendMoveLabel(m_command);
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            tmp.rotate(-10*m_anti, 0.0f, 1.0f, 0.0f);
            matrix[x] = tmp * matrix[x];
        }
    }else if(m_command.contains("D"))
    {
        emit sendMoveLabel(m_command);
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            tmp.rotate(10*m_anti, 0.0f, 1.0f, 0.0f);
            matrix[x] = tmp * matrix[x];
        }
    }else if(m_command.contains("F"))
    {
        emit sendMoveLabel(m_command);
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            tmp.rotate(-10*m_anti, 0.0f, 0.0f, 1.0f);
            matrix[x] = tmp * matrix[x];
        }

    }else if(m_command.contains("B"))
    {
        emit sendMoveLabel(m_command);
        for(auto x: qAsConst(ans)){
            QMatrix4x4 tmp;
            tmp.rotate(10*m_anti, 0.0f, 0.0f, 1.0f);
            matrix[x] = tmp * matrix[x];
        }
    }
    update();
    if(m_count == 9){//结束
        m_count = 0;
        QSet<int> tmp;
        int *que;
        if( m_command.contains("R")){
            tmp=ans & setList[FRONT];
            que = new int[4]{FRONT, DOWN, BACK, UP};
        }else if(m_command.contains("L")){
            tmp=ans & setList[FRONT];
            que = new int[4]{FRONT, UP, BACK, DOWN};
        }else if(m_command.contains("U")){
            tmp=ans & setList[FRONT];
            que = new int[4]{FRONT, RIGHT, BACK, LEFT};
        }else if(m_command.contains("D")){
            tmp=ans & setList[FRONT];
            que = new int[4]{FRONT, LEFT, BACK, RIGHT};
        }else if(m_command.contains("F")){
            tmp=ans & setList[UP];
            que = new int[4]{UP, LEFT, DOWN, RIGHT};
        }else if(m_command.contains("B")){
            tmp=ans & setList[UP];
            que = new int[4]{UP, RIGHT, DOWN, LEFT};
        }
        if(m_anti == -1){
            qSwap(que[1], que[3]);
        }
        for(int i=0; i<4; ++i){
            setList[que[i]].subtract(ans);
            if(i<3)
                setList[que[i]].unite(ans & setList[que[i+1]]);
            else
                setList[que[3]].unite(tmp);
        }
        delete[] que;
        m_command.clear();
        m_anti = 1;
        killTimer(event->timerId());
        processNextCommand();
    }
    if(commandQueue.empty())
        emit spinOver();
}


