#include "ui/model/newsmodel.h"
#include "ui/ui_controller.h"

#include <QImageReader>
#include <QJsonDocument>

NewsModel::NewsModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "userId", "text", "date", "dateModify", "attach", "fileName", "alphabet", "opened",
                    "locale", "imageSize", "type", "postId", "liked", "likes", "comments", "latitude",
                    "longitude", "eventName", "saved" });
    insert(0, { { "type", "end" } });
}

NewsModel::~NewsModel()
{
    if (m_userId != "-100")
        qDebug() << "gg wp," << m_userId;
}

void NewsModel::loadPosts(QStringList subs)
{
    Q_UNUSED(subs)

    if (!count())
        insert(0, { { "type", "end" } });

    auto postFiles = CardManager::getAll(DfsStruct::Type::Post);
    auto eventsFiles = CardManager::getAll(DfsStruct::Type::Event);

    for (const auto &postFile : postFiles)
        loadPost(QString::fromStdString(postFile), PostType::Post);
    for (const auto &eventFile : eventsFiles)
        loadPost(QString::fromStdString(eventFile), PostType::Event);
}

void NewsModel::loadUserPosts()
{
    auto posts = CardManager::getFilesByType(m_userId.toStdString(), DfsStruct::Type::Post);

    if (!posts.size())
        return; // continue;

    for (const auto &postFile : posts)
    {
        auto it = std::find(files.begin(), files.end(), postFile);
        if (it != files.end())
            continue;

        loadPost(QString::fromStdString(postFile));
        files.push_back(postFile);
    }
}

void NewsModel::loadLikesPosts(QString dbName)
{
    QString dbFile = "data/" + m_userId + "/private/" + dbName;
    if (!QFile::exists(dbFile))
        return;
    DBConnector db(dbFile.toStdString());
    auto likes = db.select("SELECT * FROM Posts");

    for (auto &el : likes)
    {
        std::string fileName =
            CardManager::buildPathForFile(el["user"], el["post"], DfsStruct::Type::Post, false);
        loadPost(QString::fromStdString(fileName));
    }
}

void NewsModel::loadPost(const QString &fileName, int type)
{
    auto it = std::find(files.begin(), files.end(), fileName.toStdString());
    if (it != files.end())
        return;

    QVariantMap post = this->post(fileName.mid(5, 20), CardManager::cutPath(fileName));
    post["type"] = type == PostType::Event ? "event" : "post";
    setNewsCount(m_newsCount + 1);

    int error = post["error"].toInt();
    if (error > 0)
    {
        qDebug() << "Post load error" << fileName << error;

        QString errorStr;
        switch (error)
        {
        case 1:
            errorStr = "NoFile";
            break;
        case 2:
            errorStr = "DBErr";
            break;
        case 3:
            errorStr = "DBAtt";
            break;
        case 4:
            errorStr = "DBAttL";
            break;
        case 5:
            errorStr = "ImErr";
            break;
        }

        QString cut = CardManager::cutPath(fileName);
        setErrors(m_errors + fileName.mid(5, 20) + "|" + cut.left(3) + "..." + cut.right(3) + "|" + errorStr
                  + "<br>");
        return;
    }

    QList<QVariantMap> &posts = list();
    qint64 currentPostDate = post["date"].toLongLong();
    int size = posts.length();

    for (int i = 0; i != size; i++)
    {
        qint64 postDate = get(i)["date"].toLongLong();

        if (postDate <= currentPostDate)
        {
            insert(i, post);
            break;
        }
    }

    files.push_back(fileName.toStdString());
}

QVariantMap NewsModel::post(QString userId, QString file)
{
    bool isPost = true;
    QString fileName = QString::fromStdString(CardManager::buildPathForFile(
        userId.toStdString(), file.toStdString(), DfsStruct::Type::Post, false));
    if (!QFile::exists(fileName))
    {
        isPost = false;
        fileName = QString::fromStdString(CardManager::buildPathForFile(
            userId.toStdString(), file.toStdString(), DfsStruct::Type::Event, false));
    }

    if (!QFile::exists(fileName) || !QFile::exists(fileName + ".stored"))
        return { { "error", 1 } };

    DBConnector db(fileName.toStdString());

    auto mapPostCfg = db.selectAll(Config::DataStorage::propertiesTableName, 1);
    auto mapPostText = db.selectAll(Config::DataStorage::textTableName);
    auto mapAttach = db.selectAll(Config::DataStorage::attachTableName);
    auto mapLiked =
        db.select("SELECT liker FROM Likes WHERE liker = '" + uiController->myId().toStdString() + "'");
    int likedCount = db.count(Config::DataStorage::likesTableName);
    int commentsCount = db.count(Config::DataStorage::commentsTableName);
    db.close();

    bool marked = false;
    QString savedDbPath = QString("data/%1/private/saved").arg(uiController->myId());
    if (QFile::exists(savedDbPath))
    {
        DBConnector savedDb(savedDbPath.toStdString());
        auto mapSaved = db.select(
            "SELECT * FROM "
            + (isPost ? Config::DataStorage::savedPostsTableName : Config::DataStorage::savedEventsTableName)
            + " WHERE " + (isPost ? "post" : "event") + " = '" + file.toStdString() + "'");
        marked = mapSaved.size() != 0;
    }

    if (mapPostCfg.empty() || mapPostText.empty() || mapAttach.empty())
    {
        qDebug() << "Error load post" << fileName << "from database";
        return { { "error", 2 } };
    }

    QVariantMap post;
    post["saved"] = marked;
    post["error"] = 0;
    post["userId"] = userId;
    post["postId"] = file;
    post["date"] = QString::fromStdString(mapPostCfg[0]["dateCreate"]).toLongLong();
    post["dateModify"] = QString::fromStdString(mapPostCfg[0]["dateModify"]).toLongLong();
    post["liked"] = !mapLiked.empty();
    post["likes"] = likedCount;
    post["comments"] = commentsCount;
    post["latitude"] = QString::fromStdString(mapPostCfg[0]["latitude"]).toInt();
    post["longitude"] = QString::fromStdString(mapPostCfg[0]["longitude"]).toInt();
    post["eventName"] = QString::fromStdString(mapPostCfg[0]["eventName"]);

    QVariantMap text;
    for (auto textLang : mapPostText)
        text[QString::fromStdString(textLang["locale"])] = QString::fromStdString(textLang["text"]);
    post["text"] = text;

    QVariantList attachList;
    for (auto map : mapAttach)
    {
        QVariantMap attach;
        QString filePath = CardManager::buildPathForFile(userId.toStdString(), map["attachId"],
                                                         DfsStruct::Type::Image, false)
                               .c_str();
        QString type = QString::fromStdString(map["type"]);
        attach["attachId"] = QString::fromStdString(map["attachId"]);
        attach["type"] = type;
        attach["date"] = QString::fromStdString(map["date"]).toLongLong();

#ifndef QT_DEBUG
        if (type == "image" || type == "gif")
        {
            if (!QFile::exists(filePath))
            {
                qDebug() << "Cant load image" << filePath << "for post" << userId << file;
                return { { "error", 5 } };
            }
        }
#endif

        QString data = QString::fromStdString(map["data"]);
        QStringList datas = data.split(" ");

        if (datas.length() != 3)
        {
            qDebug() << "Attach load error";
            return { { "error", 3 } };
        }

        double width = datas[1].toDouble();
        double height = datas[2].toDouble();
        double widthWindow = uiWidth > 0 ? uiWidth : uiController->appWidth();
        double w = widthWindow / width;

        attach["postHeight"] = height * w;
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

void NewsModel::clear()
{
    files.clear();
    AbstractModel::clear();
    insert(0, { { "type", "end" } });
}

QString NewsModel::userId() const
{
    return m_userId;
}

void NewsModel::setUserId(const QString &userId)
{
    if (m_userId == userId)
        return;

    qDebug() << "New userId" << userId;
    m_userId = userId;
    emit userIdChanged(m_userId);
}

void NewsModel::setUiController(UiController *value)
{
    uiController = value;
}

void NewsModel::setWidth(double uiWidth)
{ // TODO
    this->uiWidth = uiWidth;
}
