#pragma once
#include <QObject>
#include "Loader/MainLoader.h"
#include "Entity/StorageOrderDepth.h"

#include <vector>

QT_BEGIN_NAMESPACE
class QTcpServer;
class QTcpSocket;
QT_END_NAMESPACE

const quint16 defaultPort = 54771;

using MainLoader = Loader::MainLoader;
using Storage    = Entity::StorageOrderDepth;

class Server : public QObject
{
	Q_OBJECT

public:
	explicit Server(QObject *parent = nullptr);
	~Server() = default;

signals:
	void printMessage(const QString &msg);

public slots:
	void start();
	void stop();

private slots:
	void newConnect();
	void socketDisconnect();
	void readData();
	void sendDepthOrder(const Entity::DepthOrder &depthOrder, QTcpSocket *client, const QString &index);
	void sendAllClient(const Entity::DepthOrder&, const QString &index);
	void sendStorage(QTcpSocket *client);
	void sendStorageLite(QTcpSocket *client);

private:
	QTcpServer *server = nullptr;
	MainLoader  loader;
	Storage     storage;
	
	std::vector<QTcpSocket *> clients;
};
