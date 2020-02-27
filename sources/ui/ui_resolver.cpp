#include "ui/ui_resolver.h"
#include "dfs/managers/headers/card_manager.h"

UIResolver::UIResolver(QObject *parent)
    : QObject(parent)
{
}

UIResolver::~UIResolver()
{
    disconnect();
}

void UIResolver::process()
{
}

void UIResolver::resolveMsg(const QByteArray &path, const DfsStruct::Type &type, const QByteArray &actorId)
{
    qDebug() << "From DFS. File:" << path << "| type:" << toByteArray(type) << "| actor:" << actorId;

    switch (type)
    {
    case DfsStruct::Type::Service:
        if (path.indexOf("chatinvite") != -1)
            loadChat(path);
        break;
    case DfsStruct::Type::Post:
        // emit loadPost(path); // TODO
        resolvePost(path, actorId);
        break;
    case DfsStruct::Type::Event:
        emit loadEvent(path);
        emit loadPost(path, 1);
        break;
    case DfsStruct::Type::Image:
        emit loadImage(path);
        resolveImage(path, actorId);
        break;
    case DfsStruct::Type::Chat:
        loadChat(path);
        break;
    default:
        break;
    }
}

void UIResolver::resolvePost(const QString &fileName, const QByteArray &actorId)
{
    if (!QFile::exists(fileName))
        return;

    DBConnector db(fileName.toStdString());
    auto mapPostAttach = db.select("SELECT attachId FROM " + Config::DataStorage::attachTableName);
    db.close();

    if (mapPostAttach.empty())
        return;

    QStringList images;

    for (auto &el : mapPostAttach)
    {
        images << el["attachId"].c_str();
    }

    QList<std::pair<QString, bool>> im;

    for (auto &&image : images)
    {
        image = QString::fromStdString(CardManager::buildPathForFile(
            actorId.toStdString(), image.toStdString(), DfsStruct::Image, false));

        im.append({ image, QFile::exists(image) });
    }

    waitingPost[fileName] = im;
    checkPost(fileName);
}

void UIResolver::checkPost(const QString &fileName)
{
    auto images = waitingPost[fileName];

    if (images.isEmpty())
        return;

    bool check = true;
    for (auto &[image, status] : images)
    {
        if (!status)
        {
            status = QFile::exists(image);
            if (!status)
            {
                check = false;
                break;
            }
        }
    }

    if (check)
        emit loadPost(fileName, 0);
}

void UIResolver::resolveImage(const QString &fileName, const QByteArray &actorId)
{
    for (const auto &[post, images] : waitingPost)

    {
        checkPost(post);
        //        for (const auto &image : images)
        //        {
        //        }
    }
    // emit loadPost(path);
}
