#include "ui/wallet/recentactivitiesmodel.h"
#include "ui/wallet/walletcontroller.h"

RecentActivitiesModel::RecentActivitiesModel(BigNumber currentWalletId, AbstractModel *parent)
    : AbstractModel(parent)
    , currentWalletId(currentWalletId)
{
    setModelRoles({ "sendTime", "memberId", "amount", "date" });
}

void RecentActivitiesModel::getRecentActivitiesTransaction(QByteArray last, QByteArray first)
{
    qDebug() << "getRecentActivitiesTransaction";
    emit getTransactionList(last, first);
}

void RecentActivitiesModel::setCurrentWalletId(BigNumber id)
{
    currentWalletId = id;
    getTransactionList("0", "10");
    // qDebug() << "RECENTATIVITIESMODEL: setCurrentWalletId" << id;
}

void RecentActivitiesModel::setRecentActivitiesTransaction(QList<Transaction> *recentTransactionList)
{
    clear();
    // qDebug() << "setRecentActivitiesTransaction" << recentTransactionList->length();
    for (const auto &transaction : *recentTransactionList)
    {
        QVariantMap recentActivitiesMap;
        recentActivitiesMap["sendTime"] = 0;
        // qDebug() << transaction.toString();
        // qDebug() << transaction.getSender() << " " << currentWalletId;
        if (transaction.getSender() == currentWalletId)
        {
            // qDebug() << "RECENTATIVITIESMODEL: outcome";
            recentActivitiesMap["memberId"] = QString(transaction.getReceiver().toByteArray());
            recentActivitiesMap["amount"] =
                WalletController::toRealNumber(transaction.getAmount() * BigNumber(-1));
            recentActivitiesMap["date"] = transaction.getDate() * 1000;
        }
        else
        {
            // qDebug() << "RECENTATIVITIESMODEL: income" << currentWalletId;
            recentActivitiesMap["memberId"] = QString(transaction.getSender().toByteArray());
            recentActivitiesMap["amount"] = WalletController::toRealNumber(transaction.getAmount());
            recentActivitiesMap["date"] = transaction.getDate() * 1000;
        }
        append(recentActivitiesMap);
    }

    modelUpdated();
}

BigNumber RecentActivitiesModel::getCurrentWalletId() const
{
    return currentWalletId;
}
