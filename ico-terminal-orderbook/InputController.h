#pragma once

#include <QObject>
#include <QMutex>

class InputController : public QObject
{
	Q_OBJECT

public:
	InputController(QObject *parent = nullptr);
	~InputController();

signals:
	void signalStopApplication();

public slots:
	void run();
	void stop();

private:
	QMutex mtx;

	bool stopping = false;

	bool hasStop();
};
