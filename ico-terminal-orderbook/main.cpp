#include <QtCore/QCoreApplication>
#include <QTextCodec>
#include "mainFunc.h"
#include "AppController.h"

#define APP_VERSION "1.0"
#define APP_NAME	"OrderBook server"


int main(int argc, char *argv[])
{
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("CP866"));

	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName(APP_NAME);
	QCoreApplication::setApplicationVersion(APP_VERSION);

	const auto connectInfo = GetConnectInfo(app);

	AppController controller;
	controller.initDataBase(connectInfo.host, connectInfo.dataBase, connectInfo.user, connectInfo.password);
	controller.start();

	return app.exec();
}
