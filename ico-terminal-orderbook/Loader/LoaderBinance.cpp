#include "LoaderBinance.h"

using json = nlohmann::json;

using namespace Entity;

namespace Loader
{


LoaderBinance::LoaderBinance(QObject * parent) : AbstractLoader(parent)
{
	urlStr = "https://api.binance.com";
	exchange = "Binance";
	limit = 1000;

	rateLimit = 60;
	rateLimitTime = 60 * 1000;

	forAllRateLimit = true;
}


void LoaderBinance::load()
{
	if (!pairs.size()) return;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr + "/api/v1/depth");
		url.setQuery("symbol=" + pair.toUpper() + "&limit=" + QString::number(limit));

		request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
		request.setUrl(url);

		manager->get(request);
	}
}

void LoaderBinance::loaded(QNetworkReply *reply)
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
	const auto str = url.toString();
	const auto query = url.query();
	QRegExp rx("symbol=(\\w+)");
	if (rx.indexIn(url.toString()) == -1) return;

	auto pair = rx.cap(1);

	try
	{
		DepthOrder depthOrder;

		auto jsonData = json::parse(data.begin(), data.end());

		for (auto el = jsonData["bids"].begin(); el != jsonData["bids"].end(); ++el)
		{
			const Order order(
				std::stod((*el)[0].get<std::string>()),
				std::stod((*el)[1].get<std::string>())
			);

			depthOrder.addBid(order);
		}

		for (auto el = jsonData["asks"].begin(); el != jsonData["asks"].end(); ++el)
		{
			const Order order(
				std::stod((*el)[0].get<std::string>()),
				std::stod((*el)[1].get<std::string>())
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

void LoaderBinance::initPair()
{
	pairs.push_back("ETHUSDT");
	pairs.push_back("BTCUSDT");
	pairs.push_back("BNBUSDT");
	pairs.push_back("NEOUSDT");
	pairs.push_back("LTCUSDT");
	pairs.push_back("ZECETH");
	pairs.push_back("XRPETH");
	pairs.push_back("ADAETH");
	pairs.push_back("LTCETH");
	pairs.push_back("XLMETH");
	pairs.push_back("BNBETH");
	pairs.push_back("XMRETH");
	pairs.push_back("ETCETH");
	pairs.push_back("DASHETH");
	pairs.push_back("NEOETH");
	pairs.push_back("EOSETH");
	pairs.push_back("TRXETH");
	pairs.push_back("ETCBTC");
	pairs.push_back("ZECBTC");
	pairs.push_back("LTCBTC");
	pairs.push_back("DASHBTC");
	pairs.push_back("XLMBTC");
	pairs.push_back("XMRBTC");
	pairs.push_back("EOSBTC");
	pairs.push_back("ADABTC");
	pairs.push_back("NEOBTC");
	pairs.push_back("BNBBTC");
	pairs.push_back("XRPBTC");
	pairs.push_back("NANOBTC");
	pairs.push_back("ETHBTC");
	pairs.push_back("TRXBTC");
}

}