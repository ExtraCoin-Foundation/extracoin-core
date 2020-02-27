#ifndef RECENTACTIVITIESMODEL_H
#define RECENTACTIVITIESMODEL_H

#include "ui/model/abstractmodel.h"
#include "datastorage/transaction.h"

class RecentActivitiesModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit RecentActivitiesModel(BigNumber currentWalletId, AbstractModel *parent = nullptr);
    Q_INVOKABLE void getRecentActivitiesTransaction(QByteArray last, QByteArray count);

    void setCurrentWalletId(BigNumber);

    BigNumber getCurrentWalletId() const;

signals:
    void getTransactionList(QByteArray last, QByteArray first);
    void modelUpdated();

public slots:
    void setRecentActivitiesTransaction(QList<Transaction> *recentTransactionList);

private:
    BigNumber currentWalletId;
};

#endif // RECENTACTIVITIESMODEL_H
