#pragma once

#include "AbstractLoader.h"

namespace Loader
{


class LoaderHuobi : public AbstractLoader
{
public:
	LoaderHuobi(QObject *parent = nullptr);

	// Inherited via AbstractLoader
	virtual void load() override;
	virtual void loaded(QNetworkReply *);

protected:
	virtual void initPair() override;
};

}