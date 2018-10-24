#pragma once

#include "AbstractLoader.h"

namespace Loader
{

class LoaderExmo : public AbstractLoader
{
public:
	LoaderExmo(QObject *parent = nullptr);
	~LoaderExmo();

	void loaded(QNetworkReply *);
	void load();

protected:
	void initPair();
};

}