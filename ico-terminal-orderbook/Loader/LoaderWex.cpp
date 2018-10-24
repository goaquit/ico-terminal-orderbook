#include "LoaderWex.h"

using json = nlohmann::json;
using namespace Entity;

namespace Loader
{


LoaderWex::LoaderWex(QObject *parent)
	: AbstractLoader(parent)
{
	urlStr = "https://wex.nz/api/3/depth/";
	exchange = "wex";
}

LoaderWex::~LoaderWex()
{
	pairs.clear();
}

void LoaderWex::loaded(QNetworkReply * reply)
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

			for (auto el = jsonData[key]["bids"].begin(); el != jsonData[key]["bids"].end(); ++el)
			{
				const Order order(el->at(0).get<double>(), el->at(1).get<double>());

				depthOrder.addBid(order);
			}

			for (auto el = jsonData[key]["asks"].begin(); el != jsonData[key]["asks"].end(); ++el)
			{
				const Order order(el->at(0).get<double>(), el->at(1).get<double>());

				depthOrder.addAsk(order);
			}

			// find pair key
			QRegExp rx("(\\w+)_(\\w+)");
			if (rx.indexIn(pair) == -1) return;


			auto pairIndex = rx.cap(1) + ((rx.cap(2) == "rur") ? "rub" : rx.cap(2));

			const QString index = exchange.toLower() + "_" + pairIndex.toLower();

			emit dataRecieved(depthOrder, index);
		}

	}
	catch (nlohmann::detail::exception e)
	{
		emit printMessage(exchange + " (json): " + e.what());
	}
}

void LoaderWex::load()
{
	if (!pairs.size()) return;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr + pair);
		url.setQuery(QString("limit=") + QString::number(limit));

		request.setUrl(url);

		manager->get(request);
	}
}

void LoaderWex::initPair()
{
	pairs.push_back("btc_usd");
	pairs.push_back("btc_rur");
	pairs.push_back("btc_eur");
	
	pairs.push_back("ltc_btc");
	pairs.push_back("ltc_usd");
	pairs.push_back("ltc_rur");
	pairs.push_back("ltc_eur");

	pairs.push_back("nmc_btc");
	pairs.push_back("nmc_usd");

	pairs.push_back("nvc_btc");
	pairs.push_back("nvc_usd");

	pairs.push_back("usd_rur");
	pairs.push_back("eur_usd");
	pairs.push_back("eur_rur");

	pairs.push_back("nvc_btc");
	pairs.push_back("nvc_usd");

	pairs.push_back("eth_btc");
	pairs.push_back("eth_usd");
	pairs.push_back("eth_rur");
	pairs.push_back("eth_eur");
	pairs.push_back("eth_ltc");

	pairs.push_back("dsh_btc");
	pairs.push_back("dsh_usd");
	pairs.push_back("dsh_rur");
	pairs.push_back("dsh_eur");
	pairs.push_back("dsh_ltc");
	pairs.push_back("dsh_eth");
}


}