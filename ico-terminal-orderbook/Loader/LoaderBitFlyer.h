#pragma once
#include "AbstractLoader.h"

namespace Loader
{


class LoaderBitFlyer : public AbstractLoader
{
public:
	LoaderBitFlyer(QObject *parent = nullptr);

	virtual void load() override;
	virtual void loaded(QNetworkReply *);

protected:
	virtual void initPair() override;
};

}