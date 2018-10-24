#pragma once

#include <QObject>

#include <memory>
#include <vector>

namespace Entity
{
class StorageOrderDepth;
class DepthOrder;
}

namespace Loader
{

class AbstractLoader;

class MainLoader : public QObject
{
	Q_OBJECT

public:
	MainLoader(QObject *parent = nullptr);
	~MainLoader();

	void start();
	void stop();
	void setStorageDepth(Entity::StorageOrderDepth *);

signals:
	void printMessage(const QString &msg);
	// index: [exchange]_[pair_currency_1][pair_curryncy_2]; Exmpl: wex_btcusd
	void dataRecieved(const Entity::DepthOrder&, const QString &index);

private:
	std::vector<std::unique_ptr<AbstractLoader> > loaders;
	Entity::StorageOrderDepth *storageOrderDepth = nullptr;
};


}