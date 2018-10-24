#include "LoaderHuobi.h"

using json = nlohmann::json;
using namespace Entity;

Loader::LoaderHuobi::LoaderHuobi(QObject * parent) : AbstractLoader(parent)
{
	urlStr = "https://api.huobi.pro";
	exchange = "Huobi";

	rateLimit = 50;
	rateLimitTime = 10 * 1000;

	forAllRateLimit = true;
}

void Loader::LoaderHuobi::load()
{
	if (!pairs.size()) return;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr + "/market/depth");
		url.setQuery("symbol=" + pair.toLower() + "&type=step0");

		request.setUrl(url);

		manager->get(request);
	}
}

void Loader::LoaderHuobi::initPair()
{
	pairs.push_back("btcusdt");
	pairs.push_back("bchusdt");
	pairs.push_back("ethusdt");
	pairs.push_back("etcusdt");
	pairs.push_back("ltcusdt");
	pairs.push_back("eosusdt");
	pairs.push_back("xrpusdt");
	pairs.push_back("omgusdt");
	pairs.push_back("dashusdt");
	pairs.push_back("zecusdt");
	pairs.push_back("steemusdt");
	pairs.push_back("adausdt");
	pairs.push_back("bchbtc");
	pairs.push_back("ethbtc");
	pairs.push_back("ltcbtc");
	pairs.push_back("etcbtc");
	pairs.push_back("eosbtc");
	pairs.push_back("omgbtc");
	pairs.push_back("xrpbtc");
	pairs.push_back("dashbtc");
	pairs.push_back("zecbtc");
	pairs.push_back("adabtc");
	pairs.push_back("steembtc");
	pairs.push_back("eoseth");
	pairs.push_back("omgeth");
	pairs.push_back("adaeth");
	pairs.push_back("steemeth");
}

void Loader::LoaderHuobi::loaded(QNetworkReply *reply)
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
	QRegExp rx("\\w+\\?symbol=(\\w+)");

	const auto requestUrl = url.toString();

	if (rx.indexIn(requestUrl) == -1) return;

	auto pair = rx.cap(1);

	try
	{
		DepthOrder depthOrder;

		const auto jsonData = json::parse(data.begin(), data.end());

		if (jsonData.find("status") == jsonData.end()) return;

		const auto status = (jsonData["status"].get<std::string>() == "ok") ? true : false;

		if (!status)
		{
			emit printMessage(exchange + " has error");
			return;
		}


		for (auto el = jsonData["tick"]["bids"].begin(); el != jsonData["tick"]["bids"].end(); ++el)
		{
			const Order order(
				(*el)[0].get<double>(),
				(*el)[1].get<double>()
			);

			depthOrder.addBid(order);
		}

		for (auto el = jsonData["tick"]["asks"].begin(); el != jsonData["tick"]["asks"].end(); ++el)
		{
			const Order order(
				(*el)[0].get<double>(),
				(*el)[1].get<double>()
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
