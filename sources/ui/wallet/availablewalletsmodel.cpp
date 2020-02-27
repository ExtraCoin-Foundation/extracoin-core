#include "ui/wallet/availablewalletsmodel.h"

AvailableWalletsModel::AvailableWalletsModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "wallet" });
}

void AvailableWalletsModel::getWalletList()
{
    // qDebug() << "getWalletList";
    emit getWalletListFromActors();
}

void AvailableWalletsModel::setWalletList(QStringList *walList)
{
    clear();
    // qDebug() << "AvailableWalletsModel::setWalletList: count = " << walList->size();

    while (!walList->isEmpty())
    {
        QVariantMap walletListMap;
        walletListMap["wallet"] = walList->takeFirst();
        append(walletListMap);
    }
}
