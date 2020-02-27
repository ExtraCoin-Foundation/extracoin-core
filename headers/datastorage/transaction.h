#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QByteArray>
#include <QDateTime>
#include "datastorage/actor.h"
#include "enc/sign_interface.h"
#include "utils/bignumber.h"
#include "utils/utils.h"

class Transaction : public QObject
{
    Q_OBJECT

public:
    // Construct empty transaction
    Transaction(QObject *parent = nullptr);

    // Deserialize already created transaction
    Transaction(const QByteArray &serialized, QObject *parent = nullptr);

    // Construct transaction
    Transaction(const BigNumber &sender, const BigNumber &receiver, const BigNumber &amount,
                QObject *parent = nullptr);

    // Construct transaction with data
    Transaction(const BigNumber &sender, const BigNumber &receiver, const BigNumber &amount,
                const QByteArray &data, QObject *parent = nullptr);

    Transaction(const Transaction &other, QObject *parent = nullptr);

private:
    BigNumber sender;
    BigNumber receiver;
    BigNumber amount; // coin amount
    long long date;
    QByteArray data;     // additional payload field
    BigNumber token;     // token contract address
    BigNumber prevBlock; // last block id at the moment of tx creation
    int gas;             // security and reward param
    int hop;             // number of the nodes, through which the transaction will pass before
                         // aprovement
    QByteArray hash;     // hash from all fields
    BigNumber approver;  // address of the transaction approver.
    QByteArray digSig;

private:
    /**
     * Calculates hash of this block and writes hash to "hash" variable.
     * Uses keccak.
     */
    void calcHash();

public:
    /**
     * @brief Concatenates all fields that are used for digSig calculation
     * Override in subclasses
     * @return digSig data
     */
    QByteArray getDataForHash() const;
    QByteArray getDataForDigSig() const;

public:
    // digital signature
    void sign(const Actor<KeyPrivate> &actor);
    bool verify(const Actor<KeyPublic> &actor) const;

public:
    void setSenderBalance(BigNumber balance);
    void setReceiverBalance(BigNumber balance);
    void setPrevBlock(const BigNumber &value);
    void setGas(int gas);
    void setHop(int hop);
    void decrementHop();

public:
    int getGas() const;
    int getHop() const;
    BigNumber getSender() const;
    BigNumber getReceiver() const;
    BigNumber getAmount() const;
    BigNumber getPrevBlock() const;
    BigNumber getSenderBalance() const;
    BigNumber getReceiverBalance() const;
    QByteArray getData() const;
    QByteArray getHash() const;
    BigNumber getToken() const;
    BigNumber getApprover() const;
    QByteArray getDigSig() const;
    bool isEmpty() const;
    bool operator==(const Transaction &transaction) const;
    bool operator!=(const Transaction &transaction) const;
    void operator=(const Transaction &transaction);

public:
    QByteArray serialize() const;
    QString toString() const;

    long long getDate() const;
    void setDate(long long value);

    void setToken(const BigNumber &value);

    void setData(const QByteArray &value);

signals:
    void ProveMe();
    void Approved();
    void NotApproved();

public:
    /**
     * @brief 1.1 -> 1.1 * 10e18 in BigNumber
     * @param amount
     */
    static BigNumber visibleToAmount(QByteArray amount);

    /**
     * @brief 1 * 10e18 from BigNumber to number -> 1
     * @param number
     */
    static QString amountToVisible(BigNumber number);
};

inline bool operator<(const Transaction &l, const Transaction &r)
{
    if (l.getPrevBlock() < r.getPrevBlock())
    {
        return true;
    }
    else if (l.getSender() < r.getSender())
    {
        return true;
    }
    else if (l.getReceiver() < r.getReceiver())
    {
        return true;
    }
    else if (l.getData() < r.getData())
    {
        return true;
    }
    else if (l.getAmount() < r.getAmount())
    {
        return true;
    }
    return false;
}
#endif // TRANSACTION_H