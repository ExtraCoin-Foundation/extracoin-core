#include "ui/wallet/walletlistmodel.h"

WalletListModel::WalletListModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "wallet", "balance", "type" });
    connect(this, &WalletListModel::setWalletList, this, &WalletListModel::setWalletListSlot);
    clearModel();
}

void WalletListModel::getWalletList()
{
    qDebug() << "getWalletList";
    emit getWalletListFromAccount();
}

void WalletListModel::changeWallet(QByteArray walletId)
{
    qDebug() << "changeWallet" << walletId;
    emit changeWalletIdInAccountController(walletId);
}

void WalletListModel::clearModel()
{
    clear();

    QVariantMap walletListMap;
    walletListMap["type"] = "new";
    walletListMap["wallet"] = "";
    walletListMap["balance"] = "";
    append(walletListMap);
}

void WalletListModel::setWalletListSlot(QList<QByteArray> walList)
{
    // qDebug() << "walList size " << this->count() << walList;

    int i = 0;
    while (!walList.isEmpty())
    {
        QVariantMap walletListMap;
        walletListMap["type"] = "wallet";
        walletListMap["wallet"] = QString(walList.takeFirst());
        walletListMap["balance"] = QString(walList.takeFirst());

        if (walletListMap["wallet"] == get(i)["wallet"])
        {
            // qDebug() << "update balance = ";
            set(i, "balance", walletListMap["balance"]);
        }
        else
        {
            if (count() == 0)
            {
                append(walletListMap);
            }
            else if (get(count() - 1)["type"] == "new")
            {
                insert(count() - 1, walletListMap);
            }
            else
            {
                append(walletListMap);
            }
        }
        ++i;
    }

    // qDebug() << "walList size " << this->count();
}
