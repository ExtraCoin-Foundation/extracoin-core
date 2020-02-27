#ifndef WALLETLISTMODEL_H
#define WALLETLISTMODEL_H

#include "ui/model/abstractmodel.h"

#include <QDebug>

class WalletListModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit WalletListModel(AbstractModel *parrent = nullptr);
    Q_INVOKABLE void getWalletList();
    Q_INVOKABLE void changeWallet(QByteArray walletId);

    void clearModel();

signals:
    void getWalletListFromAccount();
    void changeWalletIdInAccountController(QByteArray);
    void setWalletList(QList<QByteArray> walList);
    //    void updateTransactionListInModel();

public slots:
    void setWalletListSlot(QList<QByteArray> walList);
};

#endif // WALLETLISTMODEL_H
