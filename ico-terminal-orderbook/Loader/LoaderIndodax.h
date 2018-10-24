#pragma once
#include "AbstractLoader.h"

namespace Loader
{


class LoaderIndodax : public AbstractLoader
{
public:
	LoaderIndodax(QObject *parent = nullptr);

	virtual void load() override;
	virtual void loaded(QNetworkReply *);

protected:
	virtual void initPair() override;
};

}