#pragma once
#include "AbstractLoader.h"

namespace Loader
{



class LoaderOkCoin : public AbstractLoader
{
public:
	LoaderOkCoin(QObject *parent = nullptr);

	virtual void load() override;
	virtual void loaded(QNetworkReply *);

protected:
	virtual void initPair() override;
};

}