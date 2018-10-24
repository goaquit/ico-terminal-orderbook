#include "AppController.h"
#include "InputController.h"
#include "Server.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QEvent>
#include <QSqlError>

#include <iostream>


AppController::AppController(QObject *parent)
	: QObject(parent), database(QSqlDatabase::addDatabase("QPSQL"))
{
	errorStr = "Initialization not performed";
}

AppController::~AppController()
{
	threadInputController.wait();
	threadServer.wait();

	// disconnect database
	database.close();
	printMessage("Disconnect database");
}

bool AppController::hasError() const
{
	return errorStr.length() > 0;
}

void AppController::printMessage(const QString & value)
{
	auto currentDateTime = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

	std::cout << currentDateTime.toStdString() << " -- " << value.toStdString() << std::endl;

	//TODO: if log file
}

void AppController::initDataBase(const QString & host, const QString & dbName, const QString & user, const QString & password)
{
	database.setHostName(host);
	database.setDatabaseName(dbName);
	database.setUserName(user);
	database.setPassword(password);

	if (!database.open())
	{
		errorStr = database.lastError().databaseText();
		printMessage(errorStr);

		stop();
	}
	else
	{
		errorStr = "";
		printMessage("Database is open");
	}
}

void AppController::start()
{
	if (hasError() || !database.isOpen())
	{
		qDebug() << "Start fail. Program has error: " << errorStr;

		stop();
	}

	startInputController();
	startServer();

	emit started();
}

void AppController::stop()
{
	printMessage("Program closing...");
	emit stopAll();

	// stop all thread		
	threadInputController.quit();
	threadServer.quit();

	qApp->quit();
}

void AppController::startInputController()
{
	InputController * controller = new InputController;
	controller->moveToThread(&threadInputController);

	connect(&threadInputController, &QThread::started,  controller, &InputController::run);
	connect(&threadInputController, &QThread::finished, controller, &InputController::stop);
	connect(&threadInputController, &QThread::finished, controller, &InputController::deleteLater);

	connect(this, &AppController::stopAll, controller, &InputController::stop);

	connect(controller, &InputController::signalStopApplication, this, &AppController::stop);

	threadInputController.start();
}

void AppController::startServer()
{
	auto server = new Server;
	server->moveToThread(&threadServer);

	connect(&threadServer, &QThread::started,  server, &Server::start);
	connect(&threadServer, &QThread::finished, server, &Server::stop);
	connect(&threadServer, &QThread::finished, server, &Server::deleteLater);

	connect(this, &AppController::stopAll, server, &Server::stop);

	connect(server, &Server::printMessage, this, &AppController::printMessage);

	threadServer.start();
}