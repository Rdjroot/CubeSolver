#include "mainwindow.h"
#include "welcome.h"
#include <opencv2/core.hpp>
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QStandardPaths>
#include <QTextStream>

namespace {
QFile *g_logFile = nullptr;
QMutex g_logMutex;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    Q_UNUSED(context);

    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString level;
    switch (type)
    {
    case QtDebugMsg:
        level = "DEBUG";
        break;
    case QtInfoMsg:
        level = "INFO";
        break;
    case QtWarningMsg:
        level = "WARN";
        break;
    case QtCriticalMsg:
        level = "ERROR";
        break;
    case QtFatalMsg:
        level = "FATAL";
        break;
    }

    const QString line = QString("[%1] [%2] %3").arg(timestamp, level, message);

    {
        QMutexLocker locker(&g_logMutex);
        fprintf(stderr, "%s\n", line.toLocal8Bit().constData());
        fflush(stderr);

        if (g_logFile != nullptr && g_logFile->isOpen())
        {
            QTextStream stream(g_logFile);
            stream << line << Qt::endl;
            g_logFile->flush();
        }
    }

    if (type == QtFatalMsg)
    {
        abort();
    }
}

void initLogging()
{
    QDir baseDir(QStringLiteral(CUBESOLVER_PROJECT_ROOT));
    QDir projectLogDir(baseDir.filePath("Documents/logs"));
    if (!projectLogDir.exists())
    {
        projectLogDir.mkpath(".");
    }

    const QString logFilePath = projectLogDir.filePath(
        QString("CubeSolver_%1.log").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")));

    g_logFile = new QFile(logFilePath);
    if (!g_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        delete g_logFile;
        g_logFile = nullptr;
        fprintf(stderr, "Failed to open log file: %s\n", logFilePath.toLocal8Bit().constData());
    }

    qInstallMessageHandler(messageHandler);
    qInfo() << "Logging initialized at" << (g_logFile ? g_logFile->fileName() : QStringLiteral("stderr only"));
}
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    initLogging();

    qRegisterMetaType<cv::Mat>("cv::Mat");

    Welcome w;
    w.setWindowTitle("CubeSolver");
    w.show();
    return a.exec();
}
