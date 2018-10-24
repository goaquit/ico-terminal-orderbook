#include "LoaderBitFlyer.h"

using json = nlohmann::json;
using namespace Entity;

namespace Loader
{

LoaderBitFlyer::LoaderBitFlyer(QObject * parent) : AbstractLoader(parent)
{
	urlStr = "https://api.bitflyer.com";
	exchange = "bitFlyer";

	rateLimit = 60;
	rateLimitTime = 60 * 1000;
}

void LoaderBitFlyer::load()
{
	if (!pairs.size()) return;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr + "/v1/board");
		url.setQuery("product_code=" + pair.toLower());

		request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
		request.setUrl(url);

		manager->get(request);
	}
}

void LoaderBitFlyer::initPair()
{
	pairs.push_back("btc_jpy");
	pairs.push_back("eth_btc");
	pairs.push_back("bch_btc");
	pairs.push_back("btc_usd");
}

void LoaderBitFlyer::loaded(QNetworkReply *reply)
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
	QRegExp rx("product_code=(\\w+)");
	if (rx.indexIn(url.toString()) == -1) return;

	auto pair = rx.cap(1);

	try
	{
		DepthOrder depthOrder;

		auto jsonData = json::parse(data.begin(), data.end());

		for (auto el = jsonData["bids"].begin(); el != jsonData["bids"].end(); ++el)
		{
			const Order order(
				(*el)["price"].get<double>(),
				(*el)["size"].get<double>()
			);

			depthOrder.addBid(order);
		}

		for (auto el = jsonData["asks"].begin(); el != jsonData["asks"].end(); ++el)
		{
			const Order order(
				(*el)["price"].get<double>(),
				(*el)["size"].get<double>()
			);

			depthOrder.addAsk(order);
		}


		const QString index = exchange.toLower() + "_" + pair.replace("_", "").toLower();;

		emit dataRecieved(depthOrder, index);
	}
	catch (nlohmann::detail::exception e)
	{
		emit printMessage(exchange + " (json): " + e.what());
	}
}

}