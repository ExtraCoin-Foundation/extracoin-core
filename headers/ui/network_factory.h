#ifndef NETWORK_FACTORY_H
#define NETWORK_FACTORY_H

#include <QObject>
#include <QQmlNetworkAccessManagerFactory>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>

class UiNetworkFactory : public QQmlNetworkAccessManagerFactory
{
public:
    UiNetworkFactory()
    {
    }

    QNetworkAccessManager* create(QObject* parent)
    {

        QNetworkAccessManager* manager = new QNetworkAccessManager(parent);
        QNetworkDiskCache* cache = new QNetworkDiskCache(manager);
        cache->setCacheDirectory("imagecache");
        cache->setMaximumCacheSize(cache->maximumCacheSize() * 6);
        manager->setCache(cache);
        return manager;
    }
};

#endif // NETWORK_FACTORY_H
