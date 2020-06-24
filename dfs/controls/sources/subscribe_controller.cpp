#include "dfs/controls/headers/subscribe_controller.h"
#include "managers/node_manager.h"

SubscribeController::SubscribeController(QObject *parent)
    : QObject(parent)
{
}

SubscribeController::SubscribeController(const SubscribeController &)
{
}

SubscribeController::~SubscribeController()
{
}

void SubscribeController::editMySubscribe(QByteArray id, bool isRemove)
{
    QByteArray currentId = nodeManager->getIdPrivateProfile();
    sendEditSql(currentId, "subscribe", DfsStruct::Type::Service,
                isRemove ? DfsStruct::ChangeType::Delete : DfsStruct::ChangeType::Insert,
                { Config::DataStorage::subscribeColumnTableName.c_str(), "subscription", id });

    sendEditSql(
        id, "follower", DfsStruct::Type::Service,
        isRemove ? DfsStruct::ChangeType::Delete : DfsStruct::ChangeType::Insert,
        { Config::DataStorage::subscribeFollowerTableName.c_str(), "subscriber", currentId, "sign", "TODO" });
}

bool SubscribeController::checkSubscribe(QByteArray id)
{
    QString path =
        DfsStruct::ROOT_FOOLDER_NAME + "/" + nodeManager->getIdPrivateProfile() + "/services/subscribe";
    DBConnector DB(path.toStdString());
    DB.createTable(Config::DataStorage::tableMySubscribeCreation);
    std::vector<DBRow> res = DB.select("SELECT * FROM " + Config::DataStorage::subscribeColumnTableName
                                       + " WHERE subscription = " + "'" + id.toStdString() + "';");
    return !res.empty();
}

int SubscribeController::checkCountSubscribe(QByteArray id)
{
    QString path = DfsStruct::ROOT_FOOLDER_NAME + "/" + id + "/services/subscribe";
    DBConnector DB(path.toStdString());
    DB.createTable(Config::DataStorage::tableMySubscribeCreation);
    std::vector<DBRow> res =
        DB.select("SELECT COUNT (*) FROM " + Config::DataStorage::subscribeColumnTableName);
    int count = res.empty() ? 0 : std::stoi(res[0]["COUNT (*)"]);
    return count;
}

std::vector<DBRow> SubscribeController::getAllSubscribe(QByteArray id)
{
    QString path = DfsStruct::ROOT_FOOLDER_NAME + "/" + id + "/services/subscriber";
    DBConnector DB(path.toStdString());
    DB.createTable(Config::DataStorage::tableMySubscribeCreation);
    std::vector<DBRow> res = DB.select("SELECT * FROM " + Config::DataStorage::subscribeColumnTableName);
    //    QList<std::string> sub;
    //    for (auto &tmp : res)
    //    {
    //        sub.append(tmp["subscription"]);
    //    }
    return res;
}

void SubscribeController::setNodeManager(NodeManager *value)
{
    nodeManager = value;
}
