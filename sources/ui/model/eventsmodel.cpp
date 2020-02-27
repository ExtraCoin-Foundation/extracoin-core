#include "ui/model/eventsmodel.h"

#include <QDate>
#include <QFile>

#include "datastorage/transaction.h"

EventsModel::EventsModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "name", "text", "location", "attach", "start", "end", "userId", "agreement", "scope",
                    "salary", "alphabet", "eventId", "startEpoch", "endEpoch", "latitude", "longitude" });
}

EventsModel::~EventsModel()
{
    if (m_userId != "-100")
        qDebug() << "gg wp," << m_userId;
}

void EventsModel::loadEvents()
{
    if (m_userId != "-333")
    {
        loadMyEvents();
        return;
    }

    auto events = CardManager::getAll(DfsStruct::Event);

    if (!events.size())
        return;

    for (const auto &event : events)
        loadEvent(QString::fromStdString(event));
}

void EventsModel::loadMyEvents()
{
    auto events = CardManager::getFilesByType(m_userId.toStdString(), DfsStruct::Event);

    for (const auto &event : events)
        loadEvent(QString::fromStdString(event));
}

void EventsModel::clear()
{
    files.clear();
    AbstractModel::clear();
}

QString EventsModel::userId() const
{
    return m_userId;
}

void EventsModel::loadEvent(const QString &fileName)
{
    auto it = std::find(files.begin(), files.end(), fileName.toStdString());
    if (it != files.end())
        return;

    QVariantMap post = this->event(fileName.mid(5, 20), CardManager::cutPath(fileName));
    if (post["error"].toBool())
    {
        qDebug() << "Post load error";
    }

    QList<QVariantMap> &posts = list();
    qint64 currentPostDate = post["startEpoch"].toLongLong();
    int size = posts.length();

    bool added = false;
    for (int i = 0; i != size; i++)
    {
        qint64 postDate = get(i)["startEpoch"].toLongLong();

        if (postDate <= currentPostDate)
        {
            insert(i, post);
            added = true;
            break;
        }
    }

    if (!added)
        append(post);
    files.push_back(fileName.toStdString());
}

QVariantMap EventsModel::event(QString userId, QString file)
{
    QString fileName = QString::fromStdString(CardManager::buildPathForFile(
        userId.toStdString(), file.toStdString(), DfsStruct::Type::Event, false));

    if (!QFile::exists(fileName))
        return { { "error", 1 } };

    DBConnector db(fileName.toStdString());

    auto mapPostCfg = db.selectAll(Config::DataStorage::propertiesTableName, 1);
    auto mapPostText = db.selectAll(Config::DataStorage::textTableName);
    auto mapAttach = db.selectAll(Config::DataStorage::attachTableName);
    auto mapLiked = db.select("SELECT liker FROM Likes WHERE liker = '0'"); // TODO
    int likedCount = db.count(Config::DataStorage::likesTableName);
    int commentsCount = db.count(Config::DataStorage::commentsTableName);
    db.close();

    if (mapPostCfg.empty() || mapPostText.empty() || mapAttach.empty())
    {
        qDebug() << "Error load post" << fileName << "from database";
        return { { "error", 2 } };
    }

    QVariantMap post;
    post["error"] = 0;
    post["userId"] = userId;
    post["eventId"] = file;
    post["name"] = QString::fromStdString(mapPostCfg[0]["eventName"]);
    post["date"] = QString::fromStdString(mapPostCfg[0]["dateCreate"]).toLongLong();
    post["dateModify"] = QString::fromStdString(mapPostCfg[0]["dateModify"]).toLongLong();
    post["startEpoch"] = QString::fromStdString(mapPostCfg[0]["startEpoch"]).toLongLong();
    post["endEpoch"] = QString::fromStdString(mapPostCfg[0]["endEpoch"]).toLongLong();
    BigNumber salary(QByteArray::fromStdString(mapPostCfg[0]["salary"]));
    post["salary"] = QString(Transaction::amountToVisible(salary.toByteArray()));
    post["salaryAmount"] = salary.toByteArray();
    post["liked"] = !mapLiked.empty();
    post["likes"] = likedCount;
    post["comments"] = commentsCount;
    post["latitude"] = QString::fromStdString(mapPostCfg[0]["latitude"]);
    post["longitude"] = QString::fromStdString(mapPostCfg[0]["longitude"]);
    post["scope"] = QString::fromStdString(mapPostCfg[0]["scope"]).toInt();
    post["agreement"] = QString::fromStdString(mapPostCfg[0]["agreement"]).toInt();
    post["location"] = post["latitude"].toString() + " " + post["longitude"].toString();
    post["start"] =
        QJsonDocument::fromJson(QByteArray::fromStdString(mapPostCfg[0]["start"])).toVariant().toMap();
    post["end"] =
        QJsonDocument::fromJson(QByteArray::fromStdString(mapPostCfg[0]["end"])).toVariant().toMap();

    QVariantMap text;
    for (auto textLang : mapPostText)
        text[QString::fromStdString(textLang["locale"])] = QString::fromStdString(textLang["text"]);
    post["text"] = text;

    QVariantList attachList;
    for (auto map : mapAttach)
    {
        QVariantMap attach;
        attach["attachId"] = QString::fromStdString(map["attachId"]);
        attach["type"] = QString::fromStdString(map["type"]);
        attach["date"] = QString::fromStdString(map["date"]).toLongLong();

        QString data = QString::fromStdString(map["data"]);
        QStringList datas = data.split(" ");

        if (datas.length() != 3)
        {
            qDebug() << "Attach load error";
            return { { "error", 3 } };
        }

        double width = datas[1].toDouble();
        double height = datas[2].toDouble();

        attach["postHeight"] = 0;
        attach["size"] = datas[0].toLongLong();
        attach["width"] = width;
        attach["height"] = height;
        attachList << attach;
    }

    if (attachList.isEmpty())
    {
        qDebug() << "attachList error";
        return { { "error", 4 } };
    }

    post["attach"] = attachList;
    QVariantMap first = attachList[0].toMap();
    post["imageSize"] = first["postHeight"].toDouble();

    return post;
}

void EventsModel::setUserId(QString userId)
{
    if (m_userId == userId)
        return;

    m_userId = userId;
    emit userIdChanged(m_userId);

    clear();
    emit userIdChanged(m_userId);
    qDebug() << "New userId" << userId;

    loadMyEvents(); // TODO: need remove after closing tests with local data files
}
