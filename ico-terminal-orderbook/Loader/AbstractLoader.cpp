#include "AbstractLoader.h"
#include "LoaderCexIo.h"
#include "LoaderWex.h"
#include "LoaderExmo.h"
#include "LoaderBithumb.h"
#include "LoaderBitfinex.h"
#include "LoaderBinance.h"
#include "LoaderHuobi.h"
#include "LoaderIndodax.h"
#include "LoaderOkCoin.h"
#include "LoaderBitFlyer.h"

#include <QTimer>

static const uint defaultInterval = 10000; // Default timer interval: 3 sec

inline uint rateInterval(uint limit, uint time, uint count)
{
	return (time * count) / limit;
}

namespace Loader
{

AbstractLoader::AbstractLoader(QObject *parent)
	: QObject(parent)
{
	
}

void AbstractLoader::connectStorage(Entity::StorageOrderDepth *storage)
{
	if (!storage) return;

	QObject::connect(this, &AbstractLoader::dataRecieved, storage, &Entity::StorageOrderDepth::dataRecieved);
}

void AbstractLoader::run()
{
	emit stop();

	if (!manager)
	{
		manager = new QNetworkAccessManager;

		connect(this,    &QObject::destroyed,              manager, &QObject::deleteLater);
		connect(manager, &QNetworkAccessManager::finished, this,    &AbstractLoader::loaded);
	}

	initPair();

	const auto interval = (forAllRateLimit) ? rateInterval(rateLimit, rateLimitTime, static_cast<uint>(pairs.size())) : defaultInterval;

	auto timer = new QTimer(this);

	timer->setInterval(interval);

	connect(timer, &QTimer::timeout,      this,  &AbstractLoader::load);
	connect(this,  &AbstractLoader::stop, timer, &QObject::deleteLater);

	timer->start();

	load();
}

AbstractLoader * AbstractLoader::Generate(Type type, QObject * parent)
{
	AbstractLoader * result = nullptr;

	switch (type)
	{
		case Binance:	result = new LoaderBinance(parent);	break;
		case Bitfinex:	result = new LoaderBitfinex(parent);break;
		case bitFlyer:	result = new LoaderBitFlyer(parent);break;
		case Bithumb:	result = new LoaderBithumb(parent);	break;
		case CexIo: 	result = new LoaderCexIo(parent);	break;
		case Exmo:		result = new LoaderExmo(parent);	break;
		case Indodax:	result = new LoaderIndodax(parent);	break;
		case OKCoin:	result = new LoaderOkCoin(parent);	break;
		case Huobi:		result = new LoaderHuobi(parent);	break;
		case Wex:		result = new LoaderWex(parent);		break;
	}

	return result;
}


}
