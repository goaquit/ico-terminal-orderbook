#include "LoaderCexIo.h"

using json = nlohmann::json;

using namespace Entity;

namespace Loader
{

LoaderCexIo::LoaderCexIo(QObject *parent)
	: AbstractLoader(parent)
{
	urlStr    = "https://cex.io/api/order_book/";
	exchange  = "cexio";
	rateLimit = 500;

	forAllRateLimit = true;
}

LoaderCexIo::~LoaderCexIo()
{
	pairs.clear();
}

void LoaderCexIo::loaded(QNetworkReply *reply)
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
	QRegExp rx("\\w+/order_book/((\\w+)/(\\w+))");
	if (rx.indexIn(url.toString()) == -1) return;

	auto pair = rx.cap(2) + rx.cap(3);

	
	try
	{
		DepthOrder depthOrder;

		auto jsonData = json::parse(data.begin(), data.end());

		for (auto el = jsonData["bids"].begin(); el != jsonData["bids"].end(); ++el)
		{
			const Order order(el->at(0).get<double>(), el->at(1).get<double>());

			depthOrder.addBid(order);
		}

		for (auto el = jsonData["asks"].begin(); el != jsonData["asks"].end(); ++el)
		{
			const Order order(el->at(0).get<double>(), el->at(1).get<double>());

			depthOrder.addAsk(order);
		}


		const QString index = exchange.toLower() + "_" + pair.toLower();

		emit dataRecieved(depthOrder, index);
	}
	catch (nlohmann::detail::exception e)
	{
		qDebug() << e.what();
	}
}

void LoaderCexIo::load()
{
	if (!pairs.size())
	{
		return;
	}

	for (const auto& pair : pairs)
	{
		QNetworkRequest request;
		QUrl url(urlStr + pair + "/");
		url.setQuery(QString("depth=") + QString::number(limit));

		request.setUrl(url);

		manager->get(request);
	}
}

void LoaderCexIo::initPair()
{
	pairs.push_back("BTC/USD");  	  
	pairs.push_back("BTC/EUR");  
	pairs.push_back("BTC/RUB");  
	//pairs.push_back("BTC/GBR"); 

	pairs.push_back("ETH/USD");  
	pairs.push_back("ETH/EUR");  
	pairs.push_back("ETH/BTC");  
							
    pairs.push_back("BCH/USD");
	//pairs.push_back("BCH/EUR");  
	//pairs.push_back("BCH/GBR");  
    pairs.push_back("BCH/BTC");
	//						
	//pairs.push_back("BTG/USD");  
	//pairs.push_back("BTG/EUR");  
	//pairs.push_back("BTG/BTC");  

	//pairs.push_back("DASH/USD");
	//pairs.push_back("DASH/EUR");
	//pairs.push_back("DASH/GBR");
	//pairs.push_back("DASH/BTC");

	//pairs.push_back("XRP/USD");  
	//pairs.push_back("XRP/EUR");  
	//pairs.push_back("XRP/BTC");  

	//pairs.push_back("ZEC/USD");  
	//pairs.push_back("ZEC/EUR");  
	//pairs.push_back("ZEC/GBR");  
	//pairs.push_back("ZEC/BTC");  
}


}
