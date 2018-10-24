#pragma once
#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE
class QCommandLineParser;
class QString;
QT_END_NAMESPACE

#ifdef WIN32

#include <windows.h>

#else

#include <termios.h>
#include <unistd.h>

#endif

QT_USE_NAMESPACE

struct ConnectInfo
{
	QString host;
	QString dataBase;
	QString user;
	QString password;
};

void SetStdinEcho(bool enable = true);
void ParserInit(QCommandLineParser &parser);
QString GetPassword(const QCommandLineParser &parser);
ConnectInfo GetConnectInfo(const QCoreApplication &app);