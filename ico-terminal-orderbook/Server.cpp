#include "Server.h"
#include <QTcpServer>
#include <QTcpSocket>

#include <QDataStream>

Server::Server(QObject *parent)
	: QObject(parent)
{
	qRegisterMetaType<Entity::DepthOrder>("Entity::DepthOrder");
}

void Server::start()
{
	if (!server)
	{
		server = new QTcpServer;

		connect(server, &QTcpServer::newConnection, this, &Server::newConnect);
	}

	if (!server->isListening())
	{
		server->listen(QHostAddress::Any, defaultPort);
		emit printMessage("Server starting. Using port: " + QString::number(defaultPort));
	
		loader.setStorageDepth(&storage);
		loader.start();

		connect(&loader, &MainLoader::dataRecieved, this, &Server::sendAllClient);
	}
}

void Server::stop()
{
	loader.stop();

	if(server)	server->close();
}

void Server::newConnect()
{
	auto client = server->nextPendingConnection();
	clients.push_back(client);

	connect(client, &QTcpSocket::readyRead,    this, &Server::readData);
	connect(client, &QTcpSocket::disconnected, this, &Server::socketDisconnect);

	emit printMessage("Client " + client->localAddress().toString() + " connected");
}

void Server::socketDisconnect()
{
	auto client = qobject_cast<QTcpSocket *>(sender());

	if (!client) return;

	auto el = std::find(clients.begin(), clients.end(), client);

	if (el == clients.end()) return;

	clients.erase(el);

	client->deleteLater();

	emit printMessage("Client " + client->localAddress().toString() + " disconnected");
}

void Server::readData()
{
	auto client = qobject_cast<QTcpSocket *>(sender());
	if (!client) return;

	while (!client->atEnd())
	{
		QString inputRequest(client->readAll());

		// split request
		auto listRequest = inputRequest.split("#", QString::SkipEmptyParts);

		for (auto &request : listRequest)
		{
			// type request:
			// request:****;arg:**;
			// request:
			//    book - request one book order, arg: index (wex_btcusd)
			//    storage - request all book order, arg: -
			//	  storage_lite - request all book with only first ask/bid order book
			// exmpl: request:book;arg:wex_ltcrur;
			//		  request:storage;arg:;

			// find pair key

			QRegExp rx("request:(\\w+);arg:(\\S*);");
			if (rx.indexIn(request) == -1)
			{
				emit printMessage("ERROR: Data request incorrect " + request);
				return;
			}

			const auto method = rx.cap(1);
			

			if (!method.compare("book", Qt::CaseInsensitive))
			{
				const auto index = rx.cap(2).trimmed().toLower();

				if (!index.size())
				{
					emit printMessage("Index not found in request: " + method);
					return;
				}

				const auto depthOrder = storage.getDepthOrder(index);

				if (depthOrder.asks().size() && depthOrder.bids().size())
				{
					sendDepthOrder(depthOrder, client, index);
				}
				else
				{
					emit printMessage("INFO: Order book not found " + request);
					return;
				}

			}
			else if (!method.compare("storage", Qt::CaseInsensitive))
			{
				sendStorage(client);
			} 
			else if (!method.compare("storage_lite", Qt::CaseInsensitive))
			{
				sendStorageLite(client);
			}

			

		}
	}

}

void Server::sendDepthOrder(const Entity::DepthOrder &depthOrder, QTcpSocket * client, const QString & index)
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream << depthOrder.getLimit(100) << index;

	client->write(data);
}

void Server::sendAllClient(const Entity::DepthOrder& depthOrder, const QString &index)
{
	QByteArray data;
	QDataStream stream(&data,QIODevice::ReadWrite);

	stream << depthOrder.getLimit(100) << index;

	for (auto client : clients)
	{
		client->write(data);
	}
}

void Server::sendStorage(QTcpSocket * client)
{
	for (const auto &el : storage.getStorage())
	{
		sendDepthOrder(el.second, client, el.first);
	}
}

void Server::sendStorageLite(QTcpSocket * client)
{
	for (const auto &el : storage.getStorageLite())
	{
		sendDepthOrder(el.second, client, el.first);
	}
}
