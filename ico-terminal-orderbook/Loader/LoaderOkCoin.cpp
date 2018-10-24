#include "LoaderOkCoin.h"

using json = nlohmann::json;
using namespace Entity;

Loader::LoaderOkCoin::LoaderOkCoin(QObject * parent) : AbstractLoader(parent)
{
	urlStr = "https://www.okcoin.com/";
	exchange = "OKCoin";

	limit = 200;

	rateLimit = 60;
	rateLimitTime = 60 * 1000;

	forAllRateLimit = true;
}

void Loader::LoaderOkCoin::load()
{
	if (!pairs.size()) return;

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr + "api/v1/depth.do");
		url.setQuery("symbol=" + pair.toLower() + "&size=" + QString::number(limit));

		request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
		request.setUrl(url);

		manager->get(request);
	}
}

void Loader::LoaderOkCoin::initPair()
{
	pairs.push_back("btc_usd");
	pairs.push_back("ltc_usd");
	pairs.push_back("eth_usd");
	pairs.push_back("etc_usd");
}

void Loader::LoaderOkCoin::loaded(QNetworkReply *reply)
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
	QRegExp rx("\\w+\\?symbol=(\\w+)");
	if (rx.indexIn(url.toString()) == -1) return;

	auto pair = rx.cap(1);

	try
	{
		auto jsonData = json::parse(data.begin(), data.end());

		// check error code
		// example: {"error_code":1007}

		if (jsonData.find("error_code") != jsonData.end())
		{

			const auto code = jsonData["error_code"].get<uint>();
			emit printMessage(exchange + " has error with code " + QString::number(code));

			return;
		}

		DepthOrder depthOrder;

		

		for (auto el = jsonData["bids"].begin(); el != jsonData["bids"].end(); ++el)
		{
			const Order order(
				(*el)[0].get<double>(),
				(*el)[1].get<double>()
			);

			depthOrder.addBid(order);
		}

		for (auto el = jsonData["asks"].rbegin(); el != jsonData["asks"].rend(); ++el)
		{
			const Order order(
				(*el)[0].get<double>(),
				(*el)[1].get<double>()
			);

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
