#pragma once

#include "AbstractLoader.h"

namespace Loader
{


class LoaderBinance : public AbstractLoader
{
public:
	LoaderBinance(QObject *parent = nullptr);

	virtual void load() override;
	virtual void loaded(QNetworkReply *);

protected:
	virtual void initPair() override;
};


}