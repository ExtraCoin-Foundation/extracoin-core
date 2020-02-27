#include "dfs/managers/headers/card_manager.h"
#include <QMutex>
#include <QUrl>

std::vector<std::string> CardManager::getFilesByType(const std::string &userId, DfsStruct::Type type)
{
    DBConnector dbConnect;

    if (!QFile::exists(pathToRoot(userId).c_str()))
        return {};
    if (!dbConnect.open(pathToRoot(userId)))
    {
        qDebug() << "[Error][Card_Manager][getFilesByType]";
        return {};
    }

    std::string query =
        "SELECT id FROM " + Config::DataStorage::cardTableName + " WHERE type=" + std::to_string(type) + ';';
    std::vector<DBRow> data = dbConnect.select(query);

    std::vector<std::string> listData;

    for (DBRow &temp : data)
        listData.push_back(CardManager::buildPathForFile(userId, temp["id"], type, false));

    return listData;
}

QStringList CardManager::getAllFiles(const QByteArray &userId)
{
    //    QFile card(dfsStruct::ROOT_FOOLDER_NAME + '/' + userId + '/' + dfsStruct::ACTOR_CARD_FILE);
    //    if (!card.exists())
    //        return {};
    //    card.open(QIODevice::ReadOnly);
    //    QList<QByteArray> list =
    //        Serialization::deserialize(card.readAll(), Serialization::DFS_ROOT_CARD_FILE_DELIMITER);
    //    if (list.isEmpty())
    //        return QStringList();

    //    for (const QByteArray &el : list)

    //        result << Serialization::deserialize(el, Serialization::DFS_CARD_FILE_SECTION_DELIMETR).at(2);
    QString path(DfsStruct::ROOT_FOOLDER_NAME + '/' + userId + '/');
    DBConnector dbConnect;
    QStringList listData;
    if (!dbConnect.open(path.toStdString() + DfsStruct::ACTOR_CARD_FILE.toStdString()))
    {
        qDebug() << "[Error][Card_Manager][getAllFiles]";
        return QStringList();
    }
    QByteArray query = "SELECT id, type FROM " + QByteArray(Config::DataStorage::cardTableName.c_str());

    std::vector<DBRow> data = dbConnect.select(query.toStdString());
    for (DBRow &temp : data)
    {
        std::string path = CardManager::buildPathForFile(userId.toStdString(), temp["id"],
                                                         DfsStruct::Type(std::stoi(temp["type"])), false);
        listData.append(QByteArray::fromStdString(path));
    }

    return listData;
}

DfsStruct::Type CardManager::getTypeByName(const QString &fullPath)
{
    QString userId = fullPath.mid(5, 20);
    bool hasSection = false;
    // int fromType = fullPath.indexOf("/", 26);
    int from = fullPath.indexOf("/", 27) + 1;
    // int fromSection = fullPath.indexOf("/", from) + 1;
    hasSection = fullPath[from + 2] == "/";
    // qDebug() << fullPath << fullPath[from + 2] << hasSection << fullPath.mid(hasSection ? fromSection :
    // from);
    QString type = fullPath.mid(26);
    type = type.left(type.indexOf("/"));
    // qDebug() << type;

    return DfsStruct::toDfsType(type.toLatin1());
}

std::string CardManager::pathToRoot(std::string userId)
{
    return DfsStruct::ROOT_FOOLDER_NAME.toStdString() + '/' + userId + '/'
        + DfsStruct::ACTOR_CARD_FILE.toStdString();
}

std::vector<std::string> CardManager::getAll(DfsStruct::Type type)
{
    std::vector<std::string> all;

    const QStringList allUserIds =
        QDir(DfsStruct::ROOT_FOOLDER_NAME).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto &userId : allUserIds)
    {
        std::vector<std::string> files = getFilesByType(userId.toStdString(), type);
        for (const std::string &file : files)
            all.push_back(file);
    }

    return all;
}

std::string CardManager::buildPathForFile(const std::string &userId, const std::string &file,
                                          DfsStruct::Type type, bool localFormat)
{
    if (file.empty())
        return "";

    const std::string currentPath =
        (localFormat ? QUrl::fromLocalFile(QDir::currentPath()).toString().toStdString() + "/" : "")
        + DfsStruct::ROOT_FOOLDER_NAME.toStdString() + "/" + userId;
    std::string section = QByteArray::fromStdString(file).right(2).toStdString() + "/";
    if (int(type) > 100)
    {
        type = DfsStruct::Type(static_cast<int>(type) - 100);
        section = "";
    }
    std::string typeName = DfsStruct::toString(type).toStdString();
    std::string path = currentPath + "/" + typeName + "/" + section + file;

    return path;
}

std::vector<std::string> CardManager::buildPathForFiles(const std::string &userId,
                                                        const std::vector<std::string> &files,
                                                        DfsStruct::Type type, bool localFormat)
{
    std::vector<std::string> result;

    for (const std::string &file : files)
    {
        result.push_back(buildPathForFile(userId, file, type, localFormat));
    }

    return result;
}

QString CardManager::cutPath(QString fullPath)
{
    QString userId = fullPath.mid(5, 20);
    bool hasSection = false;
    // int fromType = fullPath.indexOf("/", 26);
    int from = fullPath.indexOf("/", 27) + 1;
    int fromSection = fullPath.indexOf("/", from) + 1;
    hasSection = fullPath[from + 2] == "/";
    // qDebug() << fullPath << fullPath[from + 2] << hasSection << fullPath.mid(hasSection ? fromSection :
    // from);
    QString type = fullPath.mid(26);
    type = type.left(type.indexOf("/"));
    // qDebug() << type;

    return fullPath.mid(hasSection ? fromSection : from);
}

CardManager::CardManager()
{
}