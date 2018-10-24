#include "LoaderBithumb.h"

using json = nlohmann::json;
using namespace Entity;

namespace Loader
{


LoaderBithumb::LoaderBithumb(QObject *parent) : AbstractLoader(parent)
{
	urlStr = "https://api.bithumb.com/public/orderbook/";
	exchange = "Bithumb";
}


void LoaderBithumb::load()
{
	if (!pairs.size()) return;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr + pair);
		url.setQuery("count=" + QString::number(limit));

		request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
		request.setUrl(url);

		manager->get(request);
	}
}

void LoaderBithumb::initPair()
{
	pairs.push_back("BTC");
	pairs.push_back("ETH");
	pairs.push_back("DASH");
	pairs.push_back("LTC");
	pairs.push_back("ETC");
	pairs.push_back("XRP");
	pairs.push_back("BCH");
	pairs.push_back("XMR");
	pairs.push_back("ZEC");
}

void LoaderBithumb::loaded(QNetworkReply *reply)
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

		if (jsonData["status"].is_number() && jsonData["status"].get<uint>() > 0)
		{
			emit printMessage(exchange + " has error");

			return;
		}

		for (const auto &pair : pairs)
		{
			DepthOrder depthOrder;

			const auto key = pair.toStdString();

			if (key.compare(jsonData["data"]["order_currency"].get<std::string>())) continue;

			for (auto el = jsonData["data"]["bids"].begin(); el != jsonData["data"]["bids"].end(); ++el)
			{
				const double price    = std::stod( (*el)["price"].get<std::string>());
				const double quantity = std::stod((*el)["quantity"].get<std::string>());

				const Order order(price, quantity);

				depthOrder.addBid(order);
			}

			for (auto el = jsonData["data"]["asks"].begin(); el != jsonData["data"]["asks"].end(); ++el)
			{
				const double price    = std::stod((*el)["price"].get<std::string>());
				const double quantity = std::stod((*el)["quantity"].get<std::string>());

				const Order order(price, quantity);

				depthOrder.addAsk(order);
			}

			auto pairIndex = pair + QString::fromStdString(jsonData["data"]["payment_currency"].get<std::string>());
			const QString index = exchange.toLower() + "_" + pairIndex.toLower();

			emit dataRecieved(depthOrder, index);
		}

	}
	catch (nlohmann::detail::exception e)
	{
		emit printMessage(exchange + " (json): " + e.what());
	}
}


}