#pragma once

#include "AbstractLoader.h"

namespace Loader
{


class LoaderBitfinex : public AbstractLoader
{
public:
	LoaderBitfinex(QObject *parent = nullptr);
	virtual void load() override;
	virtual void loaded(QNetworkReply *);

protected:
	virtual void initPair() override;

};


}