#include "InputController.h"

#include <QCoreApplication>
#include <QMutex>

#include <iostream>

InputController::InputController(QObject *parent)
	: QObject(parent)
{
}

InputController::~InputController()
{
}

void InputController::stop()
{
	QMutexLocker lock(&mtx);

	stopping = true;
}

bool InputController::hasStop()
{
	QMutexLocker lock(&mtx);

	return stopping;
}

void InputController::run()
{
	while (!hasStop())
	{
		char option = 0;

		if (!(std::cin >> option))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		switch (option)
		{
			case 'q': 
			{
				emit signalStopApplication();
				stop();
			} break;
		}

		QCoreApplication::processEvents();
	}
}
