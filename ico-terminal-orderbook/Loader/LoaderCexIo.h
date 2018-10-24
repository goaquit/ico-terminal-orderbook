#pragma once

#include "AbstractLoader.h"

namespace Loader
{


class LoaderCexIo : public AbstractLoader
{
	Q_OBJECT

public:
	LoaderCexIo(QObject *parent = nullptr);
	~LoaderCexIo();

	void loaded(QNetworkReply *);
	void load();

protected:
	void initPair();
};



}