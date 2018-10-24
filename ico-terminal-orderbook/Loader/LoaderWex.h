#pragma once

#include "AbstractLoader.h"

namespace Loader
{


class LoaderWex : public AbstractLoader
{
	Q_OBJECT

public:
	LoaderWex(QObject *parent = nullptr);
	~LoaderWex();

	void loaded(QNetworkReply *);
	void load();

protected:
	void initPair();
};


}