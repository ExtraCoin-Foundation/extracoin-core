#ifndef WALLET_H
#define WALLET_H

#include <QObject>
#include "utils/bignumber.h"
#include "headers/network/packages/service/message_types.h"
//#include "ui/wallet/wa

//=====================models=========================
#include "ui/wallet/walletlistmodel.h"
#include "ui/wallet/availablewalletsmodel.h"
#include "ui/wallet/recentactivitiesmodel.h"
//====================================================

class WalletController : public QObject
{
    Q_OBJECT
private:
    QByteArray currentWalletId;
    BigNumber currentWalletBalance;
    QByteArray currentToken;
    QByteArrayList currentWallets;

    WalletListModel *walletListModel;
    AvailableWalletsModel *availableListModel;
    RecentActivitiesModel *recentActivitiesModel;

public:
    WalletController(QObject *parent = nullptr);

    Q_INVOKABLE void createWallet();
    Q_INVOKABLE void updateWallet(bool status);
    Q_INVOKABLE void changeWalletId(QByteArray walletId);
    Q_INVOKABLE void sendNewTx(QByteArray receiver, QByteArray amount, QByteArray token);
    Q_INVOKABLE void sendCoinRequest(QByteArray amount, QByteArray plsr);
    Q_INVOKABLE QString currentWallet();

    BigNumber getCurrentWalletId() const;
    void setCurrentWalletId(const QByteArray &value);

    Q_INVOKABLE QByteArray getCurrentWalletBalance() const;
    Q_INVOKABLE bool currentBalanceLessThanAmount(QByteArray amount);
    Q_INVOKABLE QString getCurrentBalance();
    void setCurrentWalletBalance(const BigNumber &value);

    QByteArray getCurrentToken() const;

    WalletListModel *getWalletListModel() const;
    void updateWalletListModel(QList<QByteArray> *value);

    AvailableWalletsModel *getAvailableListModel() const;
    void updateAvailableListModel(QStringList *value);

    RecentActivitiesModel *getRecentActivitiesModel() const;
    void updateRecentActivitiesModel(QList<Transaction> *value);

    Q_INVOKABLE void changeToken(const QByteArray &token);

public slots:
    // void addWalletToModel()

signals:
    void createWalletToNode();
    void updateWalletToNode();
    void changeWalletData(BigNumber walletId);
    void sendNewTransaction(BigNumber receiver, BigNumber amount, BigNumber token);
    void sendCoinRequestFromUi(const QByteArray &data, const unsigned int &msgType,
                               const SocketPair &resolver = SocketPair());
    void addNewWallet();
    void walletsUpdated();

public:
    static BigNumber toRealBigNumber(QByteArray amount); // 1.1 -> 1.1 * 10e18 in BigNumber
    static QByteArray toRealNumber(BigNumber number);    // 1 * 10e18 from BigNumber to number -> 1
    QByteArrayList getCurrentWallets() const;
    void setCurrentWallets(const QByteArrayList &value);

    QStringList getAllActor(const QString &ignoring) const;
};

#endif // WALLET_H
