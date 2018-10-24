#include "StorageOrderDepth.h"
#include <stdexcept>

namespace Entity
{


StorageOrderDepth::StorageOrderDepth(QObject *parent) : QObject(parent) {}

DepthOrder StorageOrderDepth::getDepthOrder(const QString & index) const
{	
	DepthOrder result;	

	for (const auto &pair : storage)
	{
		if (index.compare(pair.first, Qt::CaseInsensitive)) continue;

		result = pair.second;
	}

	return result;
}

MapIndexDepthOrder StorageOrderDepth::getStorage() const
{
	return storage;
}

MapIndexDepthOrder StorageOrderDepth::getStorageLite() const
{
	decltype(storage) lite;

	for (const auto &el : storage)
	{
		lite[el.first] = el.second.getLimit(1);
	}

	return lite;
}

void StorageOrderDepth::dataRecieved(const DepthOrder &depthOrder, const QString index)
{
	storage[index] = depthOrder;
}


}