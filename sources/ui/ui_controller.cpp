#include "ui/ui_controller.h"
#include "ui/wallet/walletcontroller.h"

#include <QImage>
#include <QDebug>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QImageReader>
#include <QTimer>
#include "managers/account_controller.h"
#include "asyncfuture.h" // temp
#include <QtConcurrent>  // temp

UiController::UiController(QObject *parent)
    : QObject(parent)
{
    newsPage = new NewsModel();
    newsPage->setUiController(this);
    searchModel = new SearchModel();
    eventModel = new EventsModel();
    contractsModel = new ContractsModel();
    contactsModel = new ContactsModel();
    chatListModel = new ChatListModel(this);
    chatModel = new ChatModel();
    portfolioModel = new PortfolioModel();
    wallet = new WalletController();
    notifyModel = new NotifyModel();
    welcomePage = new WelcomePage();
    uiResolver = new UIResolver();
    contactsModel->setUi(this);

    connectSignals();
}

void UiController::startThreads()
{
    ThreadPool::addThread(uiResolver);
}

NewsModel *UiController::getFeed()
{
    return this->newsPage;
}

WalletController *UiController::getWallet()

{
    return this->wallet;
}

SearchModel *UiController::getSearch()
{
    return this->searchModel;
}

EventsModel *UiController::getEvent()
{
    return eventModel;
}

WelcomePage *UiController::getWelcomePage() const
{
    return welcomePage;
}

QString UiController::myId()
{
    return currentActorId.toActorId();
}

void UiController::connectSignals()
{
    // TODO: move to UC
    connect(portfolioModel, &PortfolioModel::save, this, &UiController::addPortfolio);

    // registration
    connect(welcomePage, &WelcomePage::logInStarted, this, &UiController::logIn);
    connect(welcomePage, &WelcomePage::autoLogInStarted, this, &UiController::autoLogIn);
    connect(this, &UiController::authEnded, wallet, &WalletController::updateWallet);

    // profile
    // connect(this, &UiController::saveProfile, this, &UiController::profileChanged);
    connect(this, &UiController::sendProfile, [this]() {
        profileChanged(currentProfile);
        saveProfile(currentProfile.list());
    });
    connect(uiResolver, &UIResolver::loadPost, newsPage, &NewsModel::loadPost);
    connect(uiResolver, &UIResolver::loadEvent, eventModel, &EventsModel::loadEvent);
    connect(this, &UiController::profileUpdated, contactsModel, &ContactsModel::addProfile);
    connect(this, &UiController::profileUpdated, this, &UiController::profileRe);

    // chat
    connect(chatListModel, &ChatListModel::sendMessage, this, &UiController::sendMessage);
    connect(this, &UiController::chatListReceived, chatListModel, &ChatListModel::chatListReceived);
    connect(this, &UiController::removeChat, chatListModel, &ChatListModel::removeChat);
    connect(this, &UiController::chatReceived, chatModel, &ChatModel::chatReceived);
}

void UiController::addAvatar(QString imageFile, bool temp, const int x, const int y, const int size)
{
    const int avaSize = 200;
    qDebug() << "addingAvatar" << currentActorId.toActorId() << QUrl(imageFile).toLocalFile() << x << y
             << size;

    QImageReader reader(QUrl(imageFile).toLocalFile());
    reader.setAutoTransform(true);
    QImage originalImage = reader.read();
    QSize originalSize = originalImage.size();

    QImage image = originalImage.copy(x, y, size, size);
    originalImage = QImage();
    image = image.scaled(avaSize, avaSize);
    QImage out(image.width(), image.height(), QImage::Format_ARGB32);
    out.fill(Qt::transparent);
    QPainter painter(&out);

    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setStyle(Qt::NoPen);
    painter.setPen(pen);
    QBrush brush(image);

    painter.setBrush(brush);

    qreal radius = avaSize / 2;
    painter.drawRoundedRect(QRectF(0, 0, avaSize, avaSize), radius, radius);

    QPixmap pxDst(originalSize);
    pxDst.fill(Qt::transparent);
    QPainter painter2(&pxDst);
    // qt_blurImage(&painter2, out, 8, true, false);

    QString miniPath = "data/avatar_temp";

    if (!out.save(miniPath, "WEBP"))
        out.save(miniPath, "PNG");

    emit send(DfsStruct::DfsSave::File, temp ? imageFile : QUrl(imageFile).toLocalFile(), "",
              DfsStruct::Image);
    emit send(DfsStruct::DfsSave::File, miniPath, "", DfsStruct::Type::Image);

    QStringList avatars = currentProfile.avatar();
    avatars.insert(0, Utils::calcKeccakForFile(temp ? imageFile : QUrl(imageFile).toLocalFile()));
    currentProfile.setAvatar(avatars);
    QString miniAvatar(Utils::calcKeccakForFile(miniPath));
    currentProfile.setMiniAvatar(miniAvatar);
    QTimer::singleShot(1000, [this]() { this->updateAvatarImage(); });

    emit sendProfile();
}

bool UiController::networkStatus() const
{
    return m_networkStatus;
}

void UiController::generateImagesForPostsEvents(QVariantMap &map)
{
    auto files = map["images"].toList();
    QVariantList edited;

    QStringList iSize;

    for (const QVariant &file : files)
    {
        auto fileMap = file.toMap();
        QString image = fileMap["file"].toString();
        QString type = fileMap["type"].toString();
        QString originalImage = QUrl(image).toLocalFile();
        fileMap["file"] = Utils::calcKeccakForFile(originalImage);

        if (type == "video")
        {
            originalImage = originalImage.replace("file://", "");
            fileMap["file"] = Utils::calcKeccakForFile(originalImage);
            edited << fileMap;
            iSize << QString("%1 500 500").arg(QFileInfo(originalImage).size());
            emit send(DfsStruct::DfsSave::File, originalImage, "", DfsStruct::Video);
            continue;
        }

        QFile fileImage(originalImage);
        if (fileImage.open(QFile::ReadOnly))
        {
            QByteArray read = fileImage.read(3);
            fileImage.close();

            if (read == "GIF")
                fileMap["type"] = "gif";
        }

        QImageReader imageReader(originalImage);
        imageReader.setAutoTransform(true);

        if (imageReader.canRead())
        {
            auto size = imageReader.size();
            auto trans = imageReader.transformation();
            bool isRotate90 = trans == QImageIOHandler::TransformationRotate90
                || trans == QImageIOHandler::TransformationMirrorAndRotate90
                || trans == QImageIOHandler::TransformationFlipAndRotate90;
            QString toISize = !isRotate90 ? "%3 %1 %2" : "%3 %2 %1";
            iSize << toISize.arg(size.width()).arg(size.height()).arg(QFileInfo(originalImage).size());
        }

        edited << fileMap;
        emit send(DfsStruct::DfsSave::File, originalImage, "", DfsStruct::Image);
    }

    map["images"] = edited;
    map["isize"] = iSize;
}

NotifyModel *UiController::getNotifyModel() const
{
    return notifyModel;
}

void UiController::setSubscribeController(SubscribeController *value)
{
    subscribeController = value;
}

double UiController::appWidth() const
{
    return m_appWidth;
}

ContactsModel *UiController::getContactsModel() const
{
    return contactsModel;
}

int UiController::networkSockets() const
{
    return m_networkSockets;
}

ChatModel *UiController::getChatModel() const
{
    return chatModel;
}

ChatListModel *UiController::getChatListModel() const
{
    return chatListModel;
}

UIResolver *UiController::getUiResolver() const
{
    return uiResolver;
}

ContractsModel *UiController::getContractsModel() const
{
    return contractsModel;
}

PortfolioModel *UiController::getPortfolioModel() const
{
    return portfolioModel;
}

void UiController::addPost(QVariantMap post)
{
    qDebug() << "UiController::addPost";
    generateImagesForPostsEvents(post);

    /*
    QString text = post["text"].toString();
    QString newText = text;

    // TODO: better html
    int toDel = newText.indexOf("<p");
    newText = newText.remove(0, toDel);
    toDel = newText.indexOf(">") + 1;
    newText = newText.remove(0, toDel);
    int end = QString("</p></body></html>").length();
    // if (newdText.right(end) == "</p></body></html>")
    newText.chop(end);
    newText.replace("<!--StartFragment-->", "");
    newText.replace("<!--EndFragment-->", "");
    if (newText == "<br />")
        newText = "";

    post["text"] = newText;
    */

    DBRow postDbCProp { { "version", "2" },
                        { "sender", currentActorId.toActorId().toStdString() },
                        { "dateCreate", post["date"].toByteArray().toStdString() },
                        { "dateModify", post["date"].toByteArray().toStdString() },
                        { "latitude", "-1000" },
                        { "longitude", "-1000" },
                        { "sign", "TODO" } };
    DBRow postDbText { { "locale", "en" },
                       { "text", post["text"].toByteArray().toStdString() },
                       { "sign", "TODO" } };

    QString tmpName = QString("tmp/post_%1").arg(QRandomGenerator::global()->bounded(999999));
    DBConnector db(tmpName.toStdString());
    bool pT = db.createTable(Config::DataStorage::postPropertiesTableCreation),
         cT = db.createTable(Config::DataStorage::commentsTableCreation),
         lT = db.createTable(Config::DataStorage::likesTableCreation),
         ptT = db.createTable(Config::DataStorage::textTableCreation),
         pA = db.createTable(Config::DataStorage::attachTableCreation),
         pM = db.createTable(Config::DataStorage::usersMarkedTableCreation),
         pI = db.insert(Config::DataStorage::propertiesTableName, postDbCProp),
         pIT = db.insert(Config::DataStorage::textTableName, postDbText);

    QVariantList images = post["images"].toList();
    QStringList isize = post["isize"].toStringList();

    if (images.length() != isize.length())
    {
        qDebug() << "Post image error";
        return;
    }

    for (int i = 0; i != images.length(); i++)
    {
        QVariantMap imagesMap = images[i].toMap();
        QString type = imagesMap["type"].toString();
        if (type.isEmpty())
            type = "image";

        DBRow imRow = { { "attachId", imagesMap["file"].toString().toStdString() },
                        { "type", type.toStdString() },
                        { "date", post["date"].toByteArray().toStdString() },
                        { "data", isize[i].toStdString() },
                        { "sign", "TODO" } };
        if (!db.insert(Config::DataStorage::attachTableName, imRow))
        {
            qDebug() << "Post image db error";
            return;
        }
    }

    if (!pA || !pT || !cT || !lT || !pI | !ptT || !pIT || !pM)
    {
        qDebug() << "Error creation database for post";
        return;
    }

    emit send(DfsStruct::DfsSave::File, tmpName, "", DfsStruct::Post);
    // QTimer::singleShot(2000, [tmpName]() {
    //    QFile::remove(tmpName);
    // }); // temp. TODO: need option "remove original file" for send
}

//    start: start,
//    end: end,
//  "start        TEXT     NOT NULL,"
//  "end          TEXT     NOT NULL,"

void UiController::addEvent(QVariantMap event)
{
    qDebug() << "addEvent: universalSender";
    generateImagesForPostsEvents(event);

    DBRow eventDbCProp { { "version", "3" },
                         { "sender", currentActorId.toActorId().toStdString() },
                         { "dateCreate", event["date"].toByteArray().toStdString() },
                         { "dateModify", event["date"].toByteArray().toStdString() },
                         { "latitude", event["latitude"].toByteArray().toStdString() },
                         { "longitude", event["longitude"].toByteArray().toStdString() },
                         { "eventName", event["name"].toByteArray().toStdString() },
                         { "type", "public" },
                         { "locationName", event["location"].toByteArray().toStdString() },
                         { "scope", event["scope"].toByteArray().toStdString() },
                         { "agreement", event["agreement"].toByteArray().toStdString() },
                         { "salary",
                           Transaction::visibleToAmount(event["salary"].toByteArray()).toStdString() },
                         { "startEpoch", event["startEpoch"].toByteArray().toStdString() },
                         { "endEpoch", event["endEpoch"].toByteArray().toStdString() },
                         { "start", event["start"].toByteArray().toStdString() },
                         { "end", event["end"].toByteArray().toStdString() },
                         { "sign", "TODO" } };
    DBRow eventDbText { { "locale", "en" },
                        { "text", event["desc"].toByteArray().toStdString() },
                        { "sign", "TODO" } };

    QString tmpName = QString("tmp/event_%1").arg(QRandomGenerator::global()->bounded(999999));
    DBConnector db(tmpName.toStdString());
    bool pT = db.createTable(Config::DataStorage::eventPropertiesTableCreation),
         cT = db.createTable(Config::DataStorage::commentsTableCreation),
         lT = db.createTable(Config::DataStorage::likesTableCreation),
         ptT = db.createTable(Config::DataStorage::textTableCreation),
         pA = db.createTable(Config::DataStorage::attachTableCreation),
         pI = db.insert(Config::DataStorage::propertiesTableName, eventDbCProp),
         pIT = db.insert(Config::DataStorage::textTableName, eventDbText),
         eUF = db.createTable(Config::DataStorage::usersFollowersTableCreation),
         eUC = db.createTable(Config::DataStorage::usersConfirmedTableCreation);

    auto images = event["images"].toList();
    QStringList isize = event["isize"].toStringList();

    if (images.length() != isize.length())
    {
        qDebug() << "Event image error";
        return;
    }

    for (int i = 0; i != images.length(); i++)
    {
        QVariantMap imagesMap = images[i].toMap();

        DBRow imRow = { { "attachId", imagesMap["file"].toString().toStdString() },
                        { "type", imagesMap["type"].toString().toStdString() },
                        { "date", event["date"].toByteArray().toStdString() },
                        { "data", isize[i].toStdString() },
                        { "sign", "TODO" } };
        if (!db.insert(Config::DataStorage::attachTableName, imRow))
        {
            qDebug() << "Post image db error";
            return;
        }
    }

    if (!pA || !pT || !cT || !lT || !pI | !ptT || !pIT || !eUF || !eUC)
    {
        qDebug() << "Error creation database for event";
        return;
    }

    emit send(DfsStruct::DfsSave::File, tmpName, "", DfsStruct::Event);
}

void UiController::addComment(QByteArray postId, QString body)
{
    QVariantMap sub = { { "userId", 2 }, { "date", 22222 }, { "text", "2text text text" } };
    QVariantMap comment = { { "userId", 1 },
                            { "date", 11111 },
                            { "text", "text text text" },
                            { "sub", QVariantList { sub, sub, sub } } };

    qDebug() << QJsonDocument::fromVariant(comment).toJson(QJsonDocument::Compact);

    QByteArray serialized =
        Serialization::serialize({ "POSTES", postId + "c", body.toUtf8(), this->currentActorId.toActorId() },
                                 Serialization::INFORMATION_SEPARATOR_ONE);
}

QVariantList UiController::tokens()
{
    qDebug() << "UiController::tokens()";
    QVariantList list;
    list << QVariantMap { { "tokenId", "0" }, { "name", "ExtraCoin Coin" }, { "color", "#000000" } };

    QFile file("blockchain/.tokens");
    if (!file.open(QFile::ReadOnly))
        return list;

    QByteArrayList listData = Serialization::universalDeserialize(file.readAll(), 4);
    file.close();

    for (int i = 0; i < listData.length(); i += 3)
    {
        if (i >= listData.length())
            return list;

        QVariantMap map;
        map["tokenId"] = QString(listData[i]);
        map["name"] = QString(listData[i + 1]);
        map["color"] = QString(listData[i + 2]);
        list << map;
    }

    return list;
}

QVariantMap UiController::token(const QString &id)
{
    if (id.isEmpty())
        return QVariantMap();
    if (id == "0")
        return QVariantMap { { "tokenId", "0" }, { "name", "ExtraCoin Coin" }, { "color", "#000000" } };

    PublicProfile profile = accController->getActorIndex()->getActor(id.toLatin1()).profile();
    auto list = profile.getListProfile();
    if (list.length() < 6)
        return QVariantMap();
    if (list[0] != "6")
        return QVariantMap();

    QVariantMap map;
    map["tokenId"] = QString(list.at(2));
    map["name"] = QString(list.at(3));
    map["color"] = QString(list.at(6));

    return map;
}

void UiController::logOut()
{
    currentProfile = Profile();
    wallet->getWalletListModel()->clearModel();

    emit logout();
}

bool UiController::serverError() const
{
    return m_serverError;
}

void UiController::addPortfolio(QString image)
{
    emit send(DfsStruct::DfsSave::File, image, "", DfsStruct::Image);

    QStringList portfolio = currentProfile.portfolio();
    portfolio << Utils::calcKeccakForFile(image);
    currentProfile.setPortfolio(portfolio);
    emit sendProfile();
}

void UiController::setNetworkStatus(bool networkStatus)
{
    if (m_networkStatus == networkStatus)
        return;

    m_networkStatus = networkStatus;
    emit networkStatusChanged(m_networkStatus);
}

void UiController::setProfile(Profile profile)
{
    if (currentProfile == profile)
        return;

    currentProfile = profile;
    emit profileChanged(currentProfile);
}

void UiController::setServerError(bool serverError)
{
    if (m_serverError == serverError)
        return;

    m_serverError = serverError;
    emit serverErrorChanged(m_serverError);
}

void UiController::setNetworkSockets(int networkSockets)
{
    if (m_networkSockets == networkSockets)
        return;

    m_networkSockets = networkSockets;
    emit networkSocketsChanged(m_networkSockets);
}

void UiController::setAppWidth(double appWidth)
{
    if (qFuzzyCompare(m_appWidth, appWidth))
        return;

    m_appWidth = appWidth;
    emit appWidthChanged(m_appWidth);
}

void UiController::setIsApp(bool isApp)
{
    if (m_isApp == isApp)
        return;

    qDebug() << "Is App?" << isApp;
    m_isApp = isApp;
    emit isAppChanged(m_isApp);
}

void UiController::initUserFirstFiles()
{
    //
    std::string currentId = currentActorId.toStdString();

    DBConnector dbc(
        (DfsStruct::ROOT_FOOLDER_NAME + "/" + currentActorId.toActorId() + "/" + DfsStruct::ACTOR_CARD_FILE)
            .toStdString());
    dbc.createTable(Config::DataStorage::cardTableCreation);
    dbc.createTable(Config::DataStorage::cardDeletedTableCreation);

    QDir().mkpath(QString("data/%1/services").arg(currentId.c_str()));

    DBConnector dbSubscribe("data/" + currentId + "/services/subscribe");
    dbSubscribe.createTable(Config::DataStorage::tableMySubscribeCreation);
    dbSubscribe.close();

    DBConnector dbFollower("data/" + currentId + "/services/follower");
    dbFollower.createTable(Config::DataStorage::tableFollowerCreation);
    dbFollower.close();

    DBConnector dbChatInvite("data/" + currentId + "/services/chatinvite");
    dbChatInvite.createTable(Config::DataStorage::chatInviteCreation);
    dbChatInvite.close();

    DBConnector dbMyLikes("data/" + currentId + "/private/likes");
    dbMyLikes.createTable(Config::DataStorage::savedPostsTableCreation);
    dbMyLikes.createTable(Config::DataStorage::savedEventsTableCreation);
    dbMyLikes.close();

    DBConnector dbMySaved("data/" + currentId + "/private/saved");
    dbMySaved.createTable(Config::DataStorage::savedPostsTableCreation);
    dbMySaved.createTable(Config::DataStorage::savedEventsTableCreation);
    dbMySaved.close();

    DBConnector dbChats("data/" + currentId + "/private/chats");
    dbChats.createTable(Config::DataStorage::chatIdStorage);
    dbChats.close();

    DBConnector dbNotify("data/" + currentId + "/private/notifications");
    dbNotify.createTable(Config::DataStorage::notificationTableCreation);

    send(DfsStruct::DfsSave::Static, "likes", "", DfsStruct::Private);
    send(DfsStruct::DfsSave::Static, "saved", "", DfsStruct::Private);
    send(DfsStruct::DfsSave::Static, "chats", "", DfsStruct::Private);
    send(DfsStruct::DfsSave::Static, "notifications", "", DfsStruct::Private);
    send(DfsStruct::DfsSave::Static, "chatinvite", "", DfsStruct::Service);
    send(DfsStruct::DfsSave::Static, "subscribe", "", DfsStruct::Service);
    send(DfsStruct::DfsSave::Static, "follower", "", DfsStruct::Service);
}

void UiController::setDfs(Dfs *value)
{
    dfs = value;
}

bool UiController::isApp() const
{
    return m_isApp;
}

void UiController::addPostThread(QVariantMap post)
{
    auto future = QtConcurrent::run(this, &UiController::addPost, post);

    AsyncFuture::observe(future).subscribe([&]() { qDebug() << "Post added"; });
}

void UiController::importData(QString file)
{
    qDebug() << file;
    QFile fileImport(file);
    qDebug() << fileImport.open(QFile::ReadOnly);
    QByteArrayList list = Serialization::universalDeserialize(qUncompress(fileImport.readAll()));
    qDebug() << list;
    fileImport.close();

    // TODO: this & bool return
    //  if (list.length() < 4 || list.length() % 2 != 0 || list[0] != "extracoin_import")
    //     return;

    for (int i = 1; i != list.length(); i += 2)
    {
        QString filePath = list[i];

        if (filePath.left(5) == "file:")
            filePath.remove(0, 5);
        else
            return; // false

        qDebug() << filePath;
        QDir().mkpath(QFileInfo(filePath).path());
        QFile file(filePath);
        qDebug() << file.open(QFile::WriteOnly);
        qDebug() << file.write(list[i + 1]);
        file.close();
    }
}

void UiController::exportData()
{
    // TODO: bool return
    QString file = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/"
        + currentActorId.toByteArray() + "_" + QString::number(QDateTime::currentMSecsSinceEpoch())
        + ".extracoin";
    qDebug() << file;

    QString privateProfile = "keystore/profile/" + currentActorId.toActorId() + ".private";

    QByteArrayList res = { "extracoin_import" };
    QFile privateFile(privateProfile);
    privateFile.open(QFile::ReadOnly);
    res << "file:" + privateProfile.toLatin1() << privateFile.readAll();
    privateFile.close();

    QByteArrayList keys = wallet->getCurrentWallets();

    for (const QByteArray &key : keys)
    {
        QString keyFileStr = "keystore/personal/" + key + ".key";
        QFile keyFile(keyFileStr);
        keyFile.open(QFile::ReadOnly);
        res << "file:" + keyFileStr.toLatin1() << keyFile.readAll();
        keyFile.close();

        QString actorStr = "blockchain/index/actors/" + key.right(2) + "/" + key;
        QFile actorFile(actorStr);
        actorFile.open(QFile::ReadOnly);
        res << "file:" + actorStr.toLatin1() << actorFile.readAll();
        actorFile.close();

        QString blockStr = "blockchain/index/blocks/0/0";
        QFile blockFile(actorStr);
        blockFile.open(QFile::ReadOnly);
        res << "file:" + actorStr.toLatin1() << blockFile.readAll();
        blockFile.close();
    }

    QFile fileRes(file);
    fileRes.open(fileRes.WriteOnly);
    fileRes.write(qCompress(Serialization::universalSerialize(res, 8), 9));
    fileRes.close();
}

QVariantMap UiController::exportAttach(QString fileName)
{
    QVariantMap result;
    fileName.replace("file://", "");

    QImageReader reader(fileName);
    QString format = reader.format().toLower();
    if (format == "jpeg")
        format = "jpg";

    QFile file(fileName);
    QString newFileName = "Image_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + "." + format;
    QString newFilePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QDir().mkpath(newFilePath);
    bool copyRes = file.copy(newFilePath + "/" + newFileName);

    result["name"] = newFileName;
    result["path"] = newFilePath;
    result["ok"] = copyRes;

    return result;
}

void UiController::requestQProfile(const QString &userId, QJSValue callback)
{
    // qDebug() << "requestQProfile";
    QuickProfile qProfileCheck = qProfiles.value(userId);

    if (qProfileCallbacks[userId].isEmpty())
    {
        qProfileCallbacks.insert(userId, { callback });
    }
    else
    {
        qProfileCallbacks[userId] << callback;
    }

    if (qProfileCheck.userId.isEmpty())
    {
        if (!BigNumber::isValid(userId.toLatin1()))
            return;
        qDebug() << "---------------- Start future" << userId;
        auto future = QtConcurrent::run(accController->getActorIndex(), &ActorIndex::getProfile, userId);

        AsyncFuture::observe(future).subscribe([&, future, userId]() {
            qDebug() << userId;
            QByteArrayList profileList = future.result();
            if (profileList.isEmpty())
            {
                auto actorIndex = this->accController->getActorIndex();
                if (actorIndex->getActor(userId.toLatin1()).getAccount() == 0)
                {
                }
                return;
            }

            Profile profile = profileList;
            QStringList avatarList = profile.avatar();
            QString avatar = avatarList.length() ? avatarList[0] : "";

            QuickProfile qProfile { userId, profile.firstName(), profile.lastName(), avatar,
                                    profile.miniAvatar() };
            qProfiles[userId] = qProfile;

            auto list = qProfileCallbacks[userId];
            for (auto &&el : list)
                el.call(QJSValueList { qProfile.firstName + " " + qProfile.lastName, qProfile.miniAvatar,
                                       qProfile.firstName, qProfile.lastName, qProfile.avatar });
            qProfileCallbacks.remove(userId);
        });
    }
    else
    {
        callback.call(QJSValueList { qProfileCheck.firstName + " " + qProfileCheck.lastName,
                                     qProfileCheck.miniAvatar, qProfileCheck.firstName,
                                     qProfileCheck.lastName, qProfileCheck.avatar });
    }
}

bool UiController::isUserMySub(const QString &userId)
{
    return subscribeController->checkSubscribe(userId.toLatin1());
}

int UiController::subsCount(const QString &userId)
{
    if (userId.isEmpty())
        return -1;

    return subscribeController->checkCountSubscribe(userId.toLatin1());
}

QVariantList UiController::allMySubs(const QString &userId)
{
    QVariantList list;
    auto dbRows = subscribeController->getAllSubscribe(userId.toLatin1());

    for (const auto &map : dbRows)
    {
        for (const auto &[column, value] : map)
        {
            list << QString::fromStdString(value);
        }
    }

    return list;
}

QString UiController::dfsImage(const QString &userId, const QString &imageFile)
{
    if (userId.isEmpty() || imageFile.isEmpty())
        return "";

    QString path = QString::fromStdString(
        CardManager::buildPathForFile(userId.toStdString(), imageFile.toStdString(), DfsStruct::Image, true));
    return path;
}

QString UiController::dfsVideo(const QString &userId, const QString &videoFile)
{
    if (userId.isEmpty() || videoFile.isEmpty())
        return "";

    QString path = QString::fromStdString(
        CardManager::buildPathForFile(userId.toStdString(), videoFile.toStdString(), DfsStruct::Video, true));
    return path;
}

Profile UiController::profile() const
{
    return currentProfile;
}

void UiController::setAccController(AccountController *value)
{
    accController = value;
    contactsModel->setActorIndex(accController->getActorIndex());
    contactsModel->setAccController(accController);
}

void UiController::userRegistrationCompletion(BigNumber userId, bool isUser)
{
    if (!isUser)
        return;

    qDebug() << "userRegistrationCompletion" << userId << isUser;
    currentActorId = accController->getMainActor()->getId();

    QByteArray currentActorIdByte = currentActorId.toActorId();
    currentProfile.setUserId(currentActorIdByte);
    wallet->setCurrentWallets({ currentActorIdByte });

    emit sendProfile();
    emit savePrivateProfile(getWelcomePage()->email(), getWelcomePage()->password(), currentActorIdByte);
    emit ready();
    QTimer::singleShot(200, this, &UiController::initUserFirstFiles);
    QTimer::singleShot(1000, [this, currentActorIdByte]() {
        if (dfs->dfsValidate(currentActorIdByte))
        {
            emit connectToServer();
            emit authEnded(true, 1);
        }
    });
}

void UiController::logIn()
{
    qDebug() << "LOGIN" << welcomePage->email() << welcomePage->password();
    emit loadPrivateProfile(welcomePage->email(), welcomePage->password());
}

void UiController::autoLogIn()
{
    emit loadProfileForAutologin(welcomePage->getHash());
}

void UiController::loginPrivateProfile(QByteArray id, QByteArrayList idList)
{
    currentActorId = BigNumber(id);

    emit requestProfile(currentActorId.toActorId());
    wallet->setCurrentWallets(idList);
    emit initDfs();
    emit ready();
    QTimer::singleShot(500, this, &UiController::connectToServer); // Seva petu4
    emit authEnded(true, 3);
}

void UiController::loadInfo(const QByteArray &info, const QString &value) // pr profile getter
{
}

void UiController::profileRe(QString userId, Profile profile)
{
    QStringList avatarList = profile.avatar();
    QString avatar = avatarList.length() ? avatarList[0] : "";
    QuickProfile qProfile { userId, profile.firstName(), profile.lastName(), avatar, profile.miniAvatar() };
    qProfiles[userId] = qProfile;

    auto list = qProfileCallbacks[userId];
    for (auto &&el : list)
        el.call(QJSValueList { qProfile.firstName + " " + qProfile.lastName, qProfile.miniAvatar,
                               qProfile.firstName, qProfile.lastName, qProfile.avatar });
    qProfileCallbacks.remove(userId);

    if (userId == currentActorId.toActorId())
        setProfile(profile);
}

void UiController::allNotification(QList<notification> ntf)
{
    for (const notification &notify : ntf)
    {
        QVariantMap notifyMap = notifyModel->prepareNotify(notify);
        notifyModel->insert(0, notifyMap);
    }
}

void UiController::newNotification(notification ntf)
{
    QVariantMap notifyMap = notifyModel->prepareNotify(ntf);
    notifyModel->insert(0, notifyMap);
}

void UiController::blockDfs()
{
    QFile file("data/" + currentActorId.toActorId() + "/services/blocked");
    file.open(QFile::WriteOnly);
    file.write(currentActorId.toActorId());
    file.close();

    send(DfsStruct::DfsSave::Static, "blocked", "", DfsStruct::Service);
}

void UiController::dfsChangesEditTest()
{
    sendEdit(currentActorId.toActorId(), "test", DfsStruct::Service,
             QByteArray::number(QRandomGenerator::global()->bounded(123123123)));
}

void UiController::needProfile(const QString &userId)
{
    qDebug() << "needProfile";

    if (profilesCache[userId].userId().isEmpty())
    {
        qDebug() << "---------------- Start future2" << userId;
        auto future = QtConcurrent::run(accController->getActorIndex(), &ActorIndex::getProfile, userId);

        AsyncFuture::observe(future).subscribe([&, future, userId]() {
            qDebug() << userId;
            QByteArrayList profileList = future.result();
            if (profileList.isEmpty())
                return;

            Profile profile = profileList;
            profilesCache[userId] = profile;
            profilePageUpdated(userId, profile);
        });
    }
    else
    {
        profilePageUpdated(userId, profilesCache[userId]);
    }
}

void UiController::like(QString userId, QString postId, QString liker, bool isRemove, bool isPost)
{
    QString filePath = CardManager::buildPathForFile(userId.toStdString(), postId.toStdString(),
                                                     DfsStruct::Type::Post, false)
                           .c_str();
    filePath = filePath.remove(QString("data/%1/posts/").arg(userId));

    qDebug() << "!!!" << filePath << isRemove;
    sendEditSql(userId, filePath, isPost ? DfsStruct::Type::Post : DfsStruct::Type::Event,
                isRemove ? DfsStruct::Delete : DfsStruct::Insert,
                { Config::DataStorage::likesTableName.c_str(), "liker", liker.toLatin1(), "sign", "TODO" });

    if (isPost)
        sendEditSql(currentActorId.toByteArray(), "likes", DfsStruct::Type::Private,
                    isRemove ? DfsStruct::Delete : DfsStruct::Insert,
                    { Config::DataStorage::savedPostsTableName.c_str(), "user", userId.toLatin1(), "post",
                      postId.toLatin1() });
    else
        sendEditSql(currentActorId.toByteArray(), "likes", DfsStruct::Type::Private,
                    isRemove ? DfsStruct::Delete : DfsStruct::Insert,
                    { Config::DataStorage::savedEventsTableName.c_str(), "user", userId.toLatin1(), "event",
                      postId.toLatin1() });
}

void UiController::mark(QString userId, QString postId, bool isRemove, bool isPost)
{
    if (isPost)
        sendEditSql(currentActorId.toByteArray(), "saved", DfsStruct::Type::Private,
                    isRemove ? DfsStruct::Delete : DfsStruct::Insert,
                    { Config::DataStorage::savedPostsTableName.c_str(), "user", userId.toLatin1(), "post",
                      postId.toLatin1() });
    else
        sendEditSql(currentActorId.toByteArray(), "saved", DfsStruct::Type::Private,
                    isRemove ? DfsStruct::Delete : DfsStruct::Insert,
                    { Config::DataStorage::savedEventsTableName.c_str(), "user", userId.toLatin1(), "event",
                      postId.toLatin1() });
}

void UiController::postComment(QString userId, QString postId, QString sender, QString text)
{
    QString filePath = CardManager::buildPathForFile(userId.toStdString(), postId.toStdString(),
                                                     DfsStruct::Type::Post, false)
                           .c_str();
    filePath = filePath.remove(QString("data/%1/posts/").arg(userId));

    sendEditSql(userId, filePath, DfsStruct::Type::Post, DfsStruct::Insert,
                { Config::DataStorage::commentsTableName.c_str(), "message", text.toUtf8(), "sender",
                  sender.toLatin1(), "date", QByteArray::number(QDateTime::currentMSecsSinceEpoch()), "sign",
                  "TODO" }); // sign
}

int UiController::updateLikesTemp(QString userId, QString postId) // TODO: dfs changes signal
{
    QString filePath = CardManager::buildPathForFile(userId.toStdString(), postId.toStdString(),
                                                     DfsStruct::Type::Post, false)
                           .c_str();
    DBConnector db(filePath.toStdString());
    auto mapLikedCount = db.select("SELECT COUNT() FROM Likes");
    if (mapLikedCount.empty())
        return -1;
    if (mapLikedCount[0].empty())
        return -1;
    return QString(mapLikedCount[0]["COUNT()"].c_str()).toInt();
}

QVariantList UiController::loadCommentsTemp(QString userId, QString postId)
{
    QString filePath = CardManager::buildPathForFile(userId.toStdString(), postId.toStdString(),
                                                     DfsStruct::Type::Post, false)
                           .c_str();
    DBConnector db(filePath.toStdString());
    auto mapSql = db.select("SELECT * FROM Comments ORDER BY date");

    QVariantList comments;
    for (auto &el : mapSql)
    {
        QVariantMap map;

        for (auto &[key, value] : el)
            map[QString::fromStdString(key)] = QString::fromStdString(value);

        comments << map;
    }

    return comments;
}

QVariantList UiController::loadLikesTemp(QString userId, QString postId)
{
    QString filePath = CardManager::buildPathForFile(userId.toStdString(), postId.toStdString(),
                                                     DfsStruct::Type::Post, false)
                           .c_str();
    DBConnector db(filePath.toStdString());
    auto mapSql = db.select("SELECT liker FROM Likes");

    QVariantList likes;
    for (auto &el : mapSql)
    {
        likes << QString::fromStdString(el["liker"]);
    }

    return likes;
}

bool UiController::isCompany(QString userId)
{
    if (accController->getActorIndex()->companyId == nullptr)
        return false;

    QByteArray companyId = *accController->getActorIndex()->companyId;

    return companyId == userId;
}

QVariantList UiController::chatOwners()
{
    QVariantList list;

    QString filePath = "data/" + currentActorId.toActorId() + "/private/chats";

    if (!QFile::exists(filePath))
        return {};

    DBConnector DB(filePath.toStdString());
    std::vector<DBRow> chats = DB.select("SELECT * FROM " + Config::DataStorage::chatIdTableName);

    for (DBRow &row : chats)
    {
        QVariantMap map = { { "chatId", QString::fromStdString(row["chatId"]) },
                            { "owner", QString::fromStdString(row["owner"]) } };
        list << map;
    }

    return list;
}
