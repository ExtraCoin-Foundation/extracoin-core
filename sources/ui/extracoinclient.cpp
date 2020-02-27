#include <QStandardPaths>
#include <QQmlContext>
#include <QQuickStyle>
#include <utility>

#include "metatypes.h"
#include "ui/extracoinclient.h"
#include "managers/logs_manager.h"
#include "ui/permrequest.h"
#include "ui/model/portfoliomodel.h"
#include "ui/documenthandler.h"
#include "statusbar.h"
#include "ui/model/calendarmodel.h"
#include "ui/network_factory.h"
#include <utility>

#include <QLockFile>
#include <QZXing>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
#endif

// #include "preconfig.h"

#ifdef Q_OS_ANDROID
QObject* pQmlRootObject = NULL;

void AndroidVirtualKeyboardStateChanged(JNIEnv* env, jobject thiz, jint VirtualKeyboardHeight)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)

    if (pQmlRootObject != NULL)
    {
        QMetaObject::invokeMethod(pQmlRootObject, "androidVirtualKeyboardStateChanged", Qt::AutoConnection,
                                  Q_ARG(QVariant, VirtualKeyboardHeight));
    }
}

void InitializeForAndroid()
{
    JNINativeMethod methods[] = { { "VirtualKeyboardStateChanged", "(I)V",
                                    reinterpret_cast<void*>(AndroidVirtualKeyboardStateChanged) } };

    QAndroidJniObject javaClass("com/excfoundation/extracoin/VirtualKeyboardListener");
    QAndroidJniEnvironment env;

    jclass objectClass = env->GetObjectClass(javaClass.object<jobject>());

    env->RegisterNatives(objectClass, methods, sizeof(methods) / sizeof(methods[0]));
    env->DeleteLocalRef(objectClass);

    QAndroidJniObject::callStaticMethod<void>("com/excfoundation/extracoin/VirtualKeyboardListener",
                                              "InstallKeyboardListener");
}
#endif

ExtraCoinClient::ExtraCoinClient(QApplication* application)
{
    //#ifdef Q_OS_ANDROID
    //    qputenv("QT_QPA_NO_TEXT_HANDLES", "1");
    //#endif

    // QLoggingCategory::setFilterRules("*.debug=false\n"
    //                                  "qml=false\n"
    //                                  "driver.usb.debug=true");

    registerMetaTypes();
    app = application;
    app->setApplicationName("ExtraCoin");
    app->setOrganizationName("ExtraCoinFoundation");
    // app->setApplicationVersion(QString("%1.%2").arg(EXTRACOIN_VERSION, COMPILE_DATE));

    QQuickStyle::setStyle("Material");

    bool is64bit = QSysInfo::currentCpuArchitecture().contains(QLatin1String("64"));
    bool iPhoneSpecialLayout = false;

    qmlRegisterType<PermRequest>("ExtraCoin", 1, 0, "AndroidPermission");
    qmlRegisterType<NewsModel>("ExtraCoin", 1, 0, "EtNewsModel");
    qmlRegisterType<EventsModel>("ExtraCoin", 1, 0, "EtEventsModel");
    qmlRegisterType<ContractsModel>("ExtraCoin", 1, 0, "EtContractsModel");
    qmlRegisterType<CalendarModel>("ExtraCoin", 1, 0, "EtCalendarModel");
    qmlRegisterType<ChatModel>("ExtraCoin", 1, 0, "EtChatModel");
    qmlRegisterType<NewsModel>("ExtraCoin", 1, 0, "NewsModel");
    qmlRegisterType<EventsModel>("ExtraCoin", 1, 0, "EventsModel");
    qmlRegisterType<StatusBar>("ExtraCoin", 1, 0, "StatusBar");
    qmlRegisterType<DocumentHandler>("ExtraCoin", 1, 0, "DocumentHandler");

    QString currentPath = QDir::currentPath();

#ifdef Q_OS_IOS
    currentPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).value(0);

    iPhoneSpecialLayout = iOSUtils::iPhoneLess();
#endif

#ifdef Q_OS_ANDROID
#ifdef QT_DEBUG
    QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    dir.cdUp();
    currentPath = dir.path();
#endif
#endif

    currentPath += "/extracoin-data";
    QDir().mkdir(currentPath);
    QDir::setCurrent(currentPath);

#ifndef Q_OS_ANDROID
#ifndef Q_OS_IOS
    static QLockFile lockFile(".extracoin.lock");
    if (!lockFile.tryLock(100))
    {
        qDebug() << "ExtraCoin Client already running in directory" << QDir::currentPath();
        std::exit(0);
    }
#endif
#endif

    LogsManager::etHandler();
    LogsManager::offFile();
    // LogsManager::setFilesFilter({ "" });
    // LogsManager::setAntiFilter(true);

    PermRequest perm;
    perm.setPermissions({ "android.permission.CAMERA", "android.permission.WRITE_EXTERNAL_STORAGE",
                          "android.permission.VIBRATE", "android.permission.RECORD_AUDIO",
                          "android.permission.ACCESS_COARSE_LOCATION",
                          "android.permission.ACCESS_FINE_LOCATION" });

    engine.setNetworkAccessManagerFactory(new UiNetworkFactory());
    translater = new Translater(engine);
    etUtils = new EtUtils();

    engine.rootContext()->setContextProperty("currentPath", QUrl::fromLocalFile(currentPath).toString());
    engine.rootContext()->setContextProperty("dataPath",
                                             QUrl::fromLocalFile(currentPath + "/data").toString());
    engine.rootContext()->setContextProperty("is64bit", is64bit);
    engine.rootContext()->setContextProperty("iPhoneSpecialLayout", iPhoneSpecialLayout);
    // engine.rootContext()->setContextProperty("gitCommit", GIT_COMMIT);
    // engine.rootContext()->setContextProperty("gitBranch", GIT_BRANCH);
    // engine.rootContext()->setContextProperty("gitCommitCore", GIT_COMMIT_CORE);
    // engine.rootContext()->setContextProperty("gitBranchCore", GIT_BRANCH_CORE);
    engine.rootContext()->setContextProperty("translater", translater);
    engine.rootContext()->setContextProperty("qtVersion", QString(qVersion()));
    engine.rootContext()->setContextProperty("etUtils", etUtils);
    engine.rootContext()->setContextProperty("networkBuild", Network::build);

    QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                qFatal("[ExtraCoin] QML Error");
        },
        Qt::QueuedConnection);
    engine.load(url);

#ifdef Q_OS_ANDROID
    pQmlRootObject = engine.rootObjects().at(0);
    InitializeForAndroid();
#endif

    bool networkStatus = etUtils->networkActive();
    // qDebug() << "krya krya" << networkStatus;
#ifdef Q_OS_WIN
    networkStatus = true;
#endif

    bool isPrProDirEmpty = !QDir("keystore/profile").exists() || QDir("keystore/profile").isEmpty();
    if (!perm.checkPermissions() || isPrProDirEmpty || !networkStatus)
    {
        engine.rootObjects()
            .first()
            ->findChild<QObject*>("appLoader")
            ->setProperty("source", "qrc:/WelcomeMain.qml");
        app->exec();
    }

    if (!perm.checkPermissions())
        std::exit(0);

#ifdef Q_OS_ANDROID
#ifdef QT_DEBUG
    QDir().mkdir(currentPath);
    QDir::setCurrent(currentPath);
#endif
#endif

    // exit if close on welcome page
    bool fakeExit = engine.rootObjects().first()->property("fakeExit").toBool();

    if (!fakeExit && isPrProDirEmpty)
    {
        qInfo() << "Exit from App";
        std::exit(0);
    }

    LogsManager::debugLogs = true;
#ifdef QT_RELEASE
    LogsManager::debugLogs = false;
#endif
    LogsManager::onFile();

    isWiped = EtUtils::autoWipe(66); // increment for wipe data

    // after backend
    nodeManager = new NodeManager;
    notificationClient = new NotificationClient;
    nodeManager->setNotificationClient(notificationClient);

    UiController* uiController = nodeManager->getUiController();
    WelcomePage* uiSlotToRegistration = uiController->getWelcomePage();
    NewsModel* feedModel = uiController->getFeed();
    SearchModel* searchModel = uiController->getSearch();
    EventsModel* eventsModel = uiController->getEvent();
    ContractsModel* contractsModel = uiController->getContractsModel();
    WalletListModel* walletModel = uiController->getWallet()->getWalletListModel();
    NotifyModel* notifyModel = uiController->getNotifyModel();
    AvailableWalletsModel* availableWalletsModel = uiController->getWallet()->getAvailableListModel();
    RecentActivitiesModel* recentActivitiesModel = uiController->getWallet()->getRecentActivitiesModel();
    uiController->setAccController(nodeManager->getAccController());

    auto portfolioModel = uiController->getPortfolioModel();
    galleryModel = new GalleryModel;
    // Message* message = uiController->getMessage();

    QZXing::registerQMLTypes();
    QZXing::registerQMLImageProvider(engine);

    engine.rootContext()->setContextProperty("isWiped", isWiped);
    engine.rootContext()->setContextProperty("signBackend", uiSlotToRegistration);
    engine.rootContext()->setContextProperty("uiController", uiController);
    engine.rootContext()->setContextProperty("notificationClient", notificationClient);
    engine.rootContext()->setContextProperty("feedModel", feedModel);
    engine.rootContext()->setContextProperty("searchModel", searchModel);
    engine.rootContext()->setContextProperty("galleryModel", galleryModel);
    engine.rootContext()->setContextProperty("portfolioModel", portfolioModel);
    engine.rootContext()->setContextProperty("chatListModel", uiController->getChatListModel());
    engine.rootContext()->setContextProperty("messageModel", uiController->getChatModel());
    engine.rootContext()->setContextProperty("contactsModel", uiController->getContactsModel());
    engine.rootContext()->setContextProperty("walletModel", walletModel);
    engine.rootContext()->setContextProperty("availableWalletsModel", availableWalletsModel);
    engine.rootContext()->setContextProperty("eventsModel", eventsModel);
    engine.rootContext()->setContextProperty("walletBackend", uiController->getWallet());
    engine.rootContext()->setContextProperty("recentActivitiesModel", recentActivitiesModel);
    engine.rootContext()->setContextProperty("logsModel", &LogsManager::logs);
    engine.rootContext()->setContextProperty("contractsModel", contractsModel);
    engine.rootContext()->setContextProperty("notifyModel", notifyModel);
    engine.rootContext()->setContextProperty("serverIpDefault", Network::serverIp);
    engine.rootObjects().first()->setProperty("backendStarted", true);

#ifdef Q_OS_ANDROID
    QAndroidJniObject::callStaticMethod<void>("com/excfoundation/extracoin/MyService", "startMyService",
                                              "(Landroid/content/Context;)V",
                                              QtAndroid::androidActivity().object());
#endif
}

ExtraCoinClient::~ExtraCoinClient()
{
    galleryModel->deleteLater();
    translater->deleteLater();
    notificationClient->deleteLater();
    etUtils->deleteLater();
    nodeManager->deleteLater();
}
