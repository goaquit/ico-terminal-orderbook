#include "LoaderExmo.h"

using json = nlohmann::json;
using namespace Entity;

namespace Loader
{

LoaderExmo::LoaderExmo(QObject *parent)
	: AbstractLoader(parent)
{
    urlStr   = "https://api.exmo.com/v1/order_book";
	exchange = "exmo";

    // 24 pairs
    rateLimit = 100;
    rateLimitTime = 60 * 1000;

	forAllRateLimit = true;
}

LoaderExmo::~LoaderExmo()
{
	pairs.clear();
}

void LoaderExmo::loaded(QNetworkReply *reply)
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

	try
	{
		auto jsonData = json::parse(data.begin(), data.end());

		for (const auto &pair : pairs)
		{
			DepthOrder depthOrder;

			const auto key = pair.toStdString();
			if (jsonData.find(key) == jsonData.end()) continue;

			for (auto el = jsonData[key]["bid"].begin(); el != jsonData[key]["bid"].end(); ++el)
			{
				const double price    = std::stod(el->at(0).get<std::string>());
				const double quantity = std::stod(el->at(1).get<std::string>());

				const Order order(price, quantity);

				depthOrder.addBid(order);
			}

			for (auto el = jsonData[key]["ask"].begin(); el != jsonData[key]["ask"].end(); ++el)
			{
				const double price = std::stod(el->at(0).get<std::string>());
				const double quantity = std::stod(el->at(1).get<std::string>());

				const Order order(price, quantity);

				depthOrder.addAsk(order);
			}


			const double askQuantity = std::stod(jsonData[key]["ask_quantity"].get<std::string>());
			const double bidQuantity = std::stod(jsonData[key]["bid_quantity"].get<std::string>());

			depthOrder.setQuantityAsks(askQuantity);
			depthOrder.setQuantityBids(bidQuantity);

			auto pairIndex = pair;
			const QString index = exchange.toLower() + "_" + pairIndex.replace("_", "").toLower();

			emit dataRecieved(depthOrder, index);
		}

	}
	catch (nlohmann::detail::exception e)
	{
		emit printMessage(exchange + " (json): " + e.what());
	}
}

void LoaderExmo::load()
{
	if (!pairs.size()) return;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
        QUrl url(urlStr);
		url.setQuery("pair=" + pair + "&limit=" + QString::number(limit));

		request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
		request.setUrl(url);

		manager->get(request);
	}
}

void LoaderExmo::initPair()
{
	pairs.push_back("BCH_BTC");
	pairs.push_back("BCH_ETH");
	pairs.push_back("BCH_RUB");
	pairs.push_back("BCH_USD");

	pairs.push_back("BTC_EUR");
    //pairs.push_back("BTC_PLN");
	pairs.push_back("BTC_RUB");
    //pairs.push_back("BTC_UAH");
	pairs.push_back("BTC_USD");
	pairs.push_back("BTC_USDT");

    //pairs.push_back("DASH_BTC");
    //pairs.push_back("DASH_RUB");
    //pairs.push_back("DASH_USD");

    //pairs.push_back("DOGE_BTC");

    //pairs.push_back("ETC_BTC");
    //pairs.push_back("ETC_RUB");
    //pairs.push_back("ETC_USD");

    pairs.push_back("ETH_BTC");
	pairs.push_back("ETH_EUR");
	pairs.push_back("ETH_LTC");
    //pairs.push_back("ETH_PLN");
	pairs.push_back("ETH_RUB");
    //pairs.push_back("ETH_UAH");
	pairs.push_back("ETH_USD");
	pairs.push_back("ETH_USDT");

    //pairs.push_back("KICK_BTC");
    //pairs.push_back("KICK_ETH");

	pairs.push_back("LTC_BTC");
	pairs.push_back("LTC_EUR");
	pairs.push_back("LTC_RUB");
	pairs.push_back("LTC_USD");

    //pairs.push_back("USDT_RUB");
    //pairs.push_back("USDT_USD");

    //pairs.push_back("USD_RUB");

    //pairs.push_back("WAVES_BTC");
    //pairs.push_back("WAVES_RUB");

    //pairs.push_back("XMR_BTC");
    //pairs.push_back("XMR_EUR");
    //pairs.push_back("XMR_USD");

	pairs.push_back("XRP_BTC");
	pairs.push_back("XRP_RUB");
	pairs.push_back("XRP_USD");

	pairs.push_back("ZEC_BTC");
	pairs.push_back("ZEC_EUR");
	pairs.push_back("ZEC_RUB");
	pairs.push_back("ZEC_USD");
}

}
