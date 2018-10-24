#include "LoaderIndodax.h"

using json = nlohmann::json;
using namespace Entity;

Loader::LoaderIndodax::LoaderIndodax(QObject * parent) : AbstractLoader(parent)
{
	urlStr = "https://indodax.com/api/";
	exchange = "Indodax";

	rateLimit = 60;
	rateLimitTime = 60 * 1000;

	forAllRateLimit = true;
}

void Loader::LoaderIndodax::load()
{
	if (!pairs.size()) return;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr + pair.toLower() + "/depth");

		request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
		request.setUrl(url);

		manager->get(request);
	}
}

void Loader::LoaderIndodax::initPair()
{
	pairs.push_back("ignis_idr");
	pairs.push_back("ada_idr");
	pairs.push_back("str_idr");
	pairs.push_back("btc_idr");
	pairs.push_back("nxt_idr");
	pairs.push_back("bcd_idr");
	pairs.push_back("ten_idr");
	pairs.push_back("bch_idr");
	pairs.push_back("xrp_idr");
	pairs.push_back("waves_idr");
	pairs.push_back("eth_idr");
	pairs.push_back("btg_idr");
	pairs.push_back("str_btc");
	pairs.push_back("etc_idr");
	pairs.push_back("doge_btc");
	pairs.push_back("bts_btc");
	pairs.push_back("nxt_btc");
	pairs.push_back("xrp_btc");
	pairs.push_back("ltc_idr");
	pairs.push_back("xzc_idr");
	pairs.push_back("nem_btc");
	pairs.push_back("eth_btc");
	pairs.push_back("drk_btc");
	pairs.push_back("ltc_btc");
}

void Loader::LoaderIndodax::loaded(QNetworkReply *reply)
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
	QRegExp rx("api/(\\w+)/depth");
	if (rx.indexIn(url.toString()) == -1) return;

	auto pair = rx.cap(1);

	try
	{
		DepthOrder depthOrder;

		auto jsonData = json::parse(data.begin(), data.end());

		for (auto el = jsonData["buy"].begin(); el != jsonData["buy"].end(); ++el)
		{
			const auto priceEl = (*el)[0];

			const auto price = (priceEl.is_string()) ? std::stod(priceEl.get<std::string>()) :
				priceEl.get<double>();

			const auto volumeEl = (*el)[1];
			const auto volume = (volumeEl.is_string()) ? std::stod(volumeEl.get<std::string>()) :
				volumeEl.get<double>();

			const Order order(price,volume);

			depthOrder.addBid(order);
		}

		for (auto el = jsonData["sell"].begin(); el != jsonData["sell"].end(); ++el)
		{
			const auto priceEl = (*el)[0];

			const auto price = (priceEl.is_string()) ? std::stod(priceEl.get<std::string>()) :
				priceEl.get<double>();

			const auto volumeEl = (*el)[1];
			const auto volume = (volumeEl.is_string()) ? std::stod(volumeEl.get<std::string>()) :
				volumeEl.get<double>();

			const Order order(price, volume);

			depthOrder.addAsk(order);
		}


		const QString index = exchange.toLower() + "_" + pair.replace("_", "").toLower();

		emit dataRecieved(depthOrder, index);
	}
	catch (nlohmann::detail::exception e)
	{
		emit printMessage(exchange + " (json): " + e.what());
	}
}
