#pragma once

#include <QObject>
#include <QMutex>
#include <QSqlDatabase>
#include <QString>
#include <QThread>


class AppController : public QObject
{
	Q_OBJECT

public:
	AppController(QObject *parent = nullptr);
	~AppController();

	bool hasError() const;

	void initDataBase(const QString & host, const QString & dbName, const QString & user, const QString & password);

	void start();

signals:
	void stopAll();
	void started();

public slots:
	void printMessage(const QString &);
	void stop();



private:
	QSqlDatabase database;
	QString errorStr = "";

	QMutex mtx;

	void startInputController();
	QThread threadInputController;

	void startServer();
	QThread threadServer;
};
