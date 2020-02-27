#ifndef EXTRACOINCLIENT_H
#define EXTRACOINCLIENT_H

#include <QApplication>
#include <QDebug>

#include "managers/node_manager.h"
#include "managers/logs_manager.h"
#include "ui/etutils.h"
#include "ui/translater.h"
#include "ui/notificationclient.h"
#include "ui/model/gallerymodel.h"

#ifdef Q_OS_IOS
#include "ui/iosutils.h"
#endif

class ExtraCoinClient
{
public:
    explicit ExtraCoinClient(QApplication *application);
    ~ExtraCoinClient();

private:
    QQmlApplicationEngine engine;
    bool isWiped;

    QApplication *app;
    NodeManager *nodeManager;
    NotificationClient *notificationClient;
    Translater *translater;
    GalleryModel *galleryModel;
    EtUtils *etUtils;
};

#endif // EXTRACOINCLIENT_H
