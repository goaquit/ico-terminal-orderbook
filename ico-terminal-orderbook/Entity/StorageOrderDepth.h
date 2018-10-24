#pragma once

#include <QObject>
#include "DepthOrder.h"
#include <map>
#include <QString>
#include <QMutex>

namespace Entity
{

using MapIndexDepthOrder = std::map<QString, DepthOrder>;

class StorageOrderDepth : public QObject
{
	Q_OBJECT

public:
	StorageOrderDepth(QObject *parent = nullptr);
	~StorageOrderDepth() = default;

	DepthOrder getDepthOrder(const QString & index) const;

	MapIndexDepthOrder getStorage() const;
	MapIndexDepthOrder getStorageLite() const;

public slots:
	void dataRecieved(const DepthOrder&, const QString index);

private:
	MapIndexDepthOrder storage;
};


}