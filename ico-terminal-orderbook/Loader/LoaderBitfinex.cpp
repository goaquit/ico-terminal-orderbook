#include "LoaderBitfinex.h"

using json = nlohmann::json;

using namespace Entity;

namespace Loader
{

LoaderBitfinex::LoaderBitfinex(QObject * parent) : AbstractLoader(parent)
{
	urlStr = "https://api.bitfinex.com/v1/book/";
	exchange = "Bitfinex";
	rateLimit = 20;
	rateLimitTime = 60 * 1000;

	forAllRateLimit = true;
}

void LoaderBitfinex::load()
{
	if (!pairs.size()) return;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr + pair);
		url.setQuery("limit_asks=" + QString::number(limit)+"&limit_bids=" + QString::number(limit));

		request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
		request.setUrl(url);

		manager->get(request);
	}
}

void LoaderBitfinex::initPair()
{
	pairs.push_back("BTCUSD");
	pairs.push_back("ETHBTC");
	pairs.push_back("ETHUSD");
	pairs.push_back("ZECUSD");
	pairs.push_back("ZECBTC");
	pairs.push_back("LTCUSD");
	pairs.push_back("ETCUSD");
	pairs.push_back("ETCBTC");
	pairs.push_back("RRTUSD");
	pairs.push_back("RRTBTC");
	pairs.push_back("LTCBTC");
}

void LoaderBitfinex::loaded(QNetworkReply *reply)
{
	if (reply->error() != QNetworkReply::NoError)
	{
		emit printMessage(exchange + " (network): " + reply->errorString());

		reply->deleteLater();

		return;
	}

	const auto url = reply->url();
	const auto byteData = reply->readAll();
	const std::string data(byteData.constData(), byteData.length());
	reply->deleteLater();

	// find pair key
	QRegExp rx("\\w+/book/(\\w+)");
	if (rx.indexIn(url.toString()) == -1) return;

	auto pair = rx.cap(1);

	try
	{
		DepthOrder depthOrder;

		auto jsonData = json::parse(data.begin(), data.end());

		for (auto el = jsonData["bids"].begin(); el != jsonData["bids"].end(); ++el)
		{
			const Order order(
				std::stod((*el)["price"].get<std::string>()), 
				std::stod((*el)["amount"].get<std::string>())
			);

			depthOrder.addBid(order);
		}

		for (auto el = jsonData["asks"].begin(); el != jsonData["asks"].end(); ++el)
		{
			const Order order(
				std::stod((*el)["price"].get<std::string>()),
				std::stod((*el)["amount"].get<std::string>())
			);

			depthOrder.addAsk(order);
		}


		const QString index = exchange.toLower() + "_" + pair.toLower();

		emit dataRecieved(depthOrder, index);
	}
	catch (nlohmann::detail::exception e)
	{
		emit printMessage(exchange + " (json): " + e.what());
	}
}

}