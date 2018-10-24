#pragma once

#include <QObject>
#include <QString>
#include <vector>

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>


// entity
#include "Entity/DepthOrder.h"
#include "Entity/StorageOrderDepth.h"
#include "Lib/json.hpp"

namespace Loader
{

class AbstractLoader : public QObject
{
	Q_OBJECT

public:
	AbstractLoader(QObject *parent = nullptr);
	~AbstractLoader() = default;

	enum Type : quint8
	{
		Binance,
		Bitfinex,
		bitFlyer,
		Bithumb,
		CexIo,
		Exmo,
		Indodax,
		OKCoin,
		Huobi,
		Wex
	};

	void connectStorage(Entity::StorageOrderDepth *storage = nullptr);

	void run();

	static AbstractLoader * Generate(Type type, QObject *parent = nullptr);

public slots:
	virtual void loaded(QNetworkReply *) = 0;
	virtual void load() = 0;

signals:
	// index: [exchange]_[pair_currency_1][pair_curryncy_2]; Exmpl: wex_btcusd
	void dataRecieved(const Entity::DepthOrder&, const QString &index);
	void printMessage(const QString &msg);
	void stop();

protected:

	QNetworkAccessManager *manager = nullptr;

	QString exchange = "";
	QString urlStr	 = "";
	uint    limit	 = 5000;

	std::vector<QString> pairs; // pair; frequency

	virtual void initPair() = 0; 

	bool forAllRateLimit = false;
	uint rateLimit		 = 600; 
	uint rateLimitTime	 = 1000 * 60 * 10; // 10 min

};


}