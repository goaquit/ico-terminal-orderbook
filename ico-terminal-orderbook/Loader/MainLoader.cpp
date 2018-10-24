#include "MainLoader.h"
#include "AbstractLoader.h"
#include "Entity/StorageOrderDepth.h"
#include "Entity/DepthOrder.h"

namespace Loader
{

using StorageOrderDepth = Entity::StorageOrderDepth;
using uniqLoader		= std::unique_ptr<AbstractLoader>;

MainLoader::MainLoader(QObject *parent)
	: QObject(parent)
{
	
}

MainLoader::~MainLoader()
{
	loaders.clear();
}

void MainLoader::start()
{

	loaders.push_back(uniqLoader(AbstractLoader::Generate(AbstractLoader::CexIo    )));
	loaders.push_back(uniqLoader(AbstractLoader::Generate(AbstractLoader::Exmo	   )));
	loaders.push_back(uniqLoader(AbstractLoader::Generate(AbstractLoader::Wex      )));
	loaders.push_back(uniqLoader(AbstractLoader::Generate(AbstractLoader::Bithumb  )));
	loaders.push_back(uniqLoader(AbstractLoader::Generate(AbstractLoader::Bitfinex )));
	loaders.push_back(uniqLoader(AbstractLoader::Generate(AbstractLoader::Binance  )));
	loaders.push_back(uniqLoader(AbstractLoader::Generate(AbstractLoader::Huobi    )));
	loaders.push_back(uniqLoader(AbstractLoader::Generate(AbstractLoader::Indodax  )));
	loaders.push_back(uniqLoader(AbstractLoader::Generate(AbstractLoader::OKCoin   )));
	loaders.push_back(uniqLoader(AbstractLoader::Generate(AbstractLoader::bitFlyer )));

	for (const auto &loader : loaders)
	{
		if (!loader.get()) continue;		
		loader->connectStorage(storageOrderDepth);
		loader->run();

		connect(loader.get(), &AbstractLoader::printMessage, this, &MainLoader::printMessage);
		connect(loader.get(), &AbstractLoader::dataRecieved, this, &MainLoader::dataRecieved);
	}

}

void MainLoader::stop()
{
	loaders.clear();
}

void MainLoader::setStorageDepth(StorageOrderDepth *storage)
{
	storageOrderDepth = storage;
}


}