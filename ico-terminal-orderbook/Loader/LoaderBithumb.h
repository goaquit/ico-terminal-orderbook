#pragma once

#include "AbstractLoader.h"

namespace Loader
{


class LoaderBithumb : public AbstractLoader
{
public:
	LoaderBithumb(QObject *parent = nullptr);

	virtual void load() override;
	virtual void loaded(QNetworkReply *);

protected:
	virtual void initPair() override;
};


}