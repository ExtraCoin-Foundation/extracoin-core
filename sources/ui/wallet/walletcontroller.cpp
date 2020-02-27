#include "ui/wallet/walletcontroller.h"

//#include "network/packages/entities/entity_message.h"

QByteArrayList WalletController::getCurrentWallets() const
{
    return currentWallets;
}

void WalletController::setCurrentWallets(const QByteArrayList &value)
{
    currentWallets.clear();
    currentWallets = value;
    // static int i = 1;
    // qDebug() << "setCurrentWallets call counter:" << i++;
}

QStringList WalletController::getAllActor(const QString &ignoring) const
{
    QStringList list;

    QDir actorsDir("blockchain/index/actors");
    const auto sections = actorsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const auto &section : sections)
    {
        auto actors = QDir("blockchain/index/actors/" + section).entryList(QDir::Files);
        actors.removeOne(ignoring);
        list << actors;
    }

    return list;
}

WalletController::WalletController(QObject *parent)
    : QObject(parent)
{
    walletListModel = new WalletListModel();
    availableListModel = new AvailableWalletsModel();
    recentActivitiesModel = new RecentActivitiesModel(BigNumber(1));
}

void WalletController::createWallet()
{
    return;
    qDebug() << "create Wallet ui";
    emit createWalletToNode();
}

void WalletController::updateWallet(bool status)
{
    return;
    if (status)
    {
        qDebug() << "update Wallet ui";
        emit updateWalletToNode();
    }
    else
        qDebug() << "not correct userId";
}

void WalletController::changeWalletId(QByteArray walletId)
{
    if (walletId.isEmpty())
    {
        return;
    }

    qDebug() << "changeWalletId" << walletId;
    setCurrentWalletId(walletId);
    emit changeWalletData(BigNumber(walletId));
}

void WalletController::sendNewTx(QByteArray receiver, QByteArray amount, QByteArray token)
{
    if (token.isEmpty())
        token = "0";

    BigNumber realBigNumber = toRealBigNumber(amount);
    qDebug() << "WALLET CONTROLLER: " << receiver << " " << amount << realBigNumber
             << toRealNumber(realBigNumber) << "| token:" << token;

    emit sendNewTransaction(BigNumber(receiver), realBigNumber, token);
}

void WalletController::sendCoinRequest(QByteArray amount, QByteArray plsr)
{
    emit sendCoinRequestFromUi(toRealBigNumber(amount).toByteArray() + " " + plsr,
                               Messages::ChainMessage::coinRequest);
}

QString WalletController::currentWallet()
{
    return currentWalletId;
}

BigNumber WalletController::getCurrentWalletId() const
{
    return currentWalletId;
}

void WalletController::setCurrentWalletId(const QByteArray &value)
{
    currentWalletId = value;
    recentActivitiesModel->setCurrentWalletId(value);
}

QByteArray WalletController::getCurrentWalletBalance() const
{
    return currentWalletBalance.toByteArray(10);
}

bool WalletController::currentBalanceLessThanAmount(QByteArray amount)
{
    return currentWalletBalance < toRealBigNumber(amount);
}

QString WalletController::getCurrentBalance()
{

    return Transaction::amountToVisible(currentWalletBalance);
}

void WalletController::setCurrentWalletBalance(const BigNumber &value)
{
    currentWalletBalance = value;
}

QByteArray WalletController::getCurrentToken() const
{
    return currentToken;
}

WalletListModel *WalletController::getWalletListModel() const
{
    return walletListModel;
}

void WalletController::updateWalletListModel(QList<QByteArray> *value)
{
    walletListModel->setWalletList(*value);
}

AvailableWalletsModel *WalletController::getAvailableListModel() const
{
    return availableListModel;
}

void WalletController::updateAvailableListModel(QStringList *value)
{
    availableListModel->setWalletList(value);
}

RecentActivitiesModel *WalletController::getRecentActivitiesModel() const
{
    return recentActivitiesModel;
}

void WalletController::updateRecentActivitiesModel(QList<Transaction> *value)
{
    recentActivitiesModel->setRecentActivitiesTransaction(value);
}

void WalletController::changeToken(const QByteArray &token)
{
    currentToken = token;
    updateWalletToNode();
}

// void WalletController::receiveTokenList(QStringList tokens)
// {
// }

BigNumber WalletController::toRealBigNumber(QByteArray amount)
{
    amount += amount.indexOf(".") == -1 ? "." : "";
    QByteArrayList amountList = amount.split('.');
    int secondLength = amountList[1].length();

    amount += QString("0").repeated(18 - secondLength).toLatin1();
    amount.replace(".", "");

    return BigNumber(amount, 10);
}

QByteArray WalletController::toRealNumber(BigNumber number)
{
    // qDebug() << number;
    if (number == 0)
        return "0";

    QByteArray numberArr = number.toByteArray(10);
    // qDebug() << numberArr;
    QString second = numberArr.right(18); //
    second = QString("0").repeated(18 - second.length()).toLatin1() + second;
    second = second.remove(QRegExp("[0]*$"));
    QByteArray first = numberArr.left(numberArr.length() - 18);

    // qDebug() << "---->" << first << second;

    QString numberDec =
        (first.isEmpty() ? "0" : first) + (second == "0" || second.isEmpty() ? "" : "." + second);

    return numberDec.toLatin1();
}

// QList<Transaction> getTransactionsList(BigNumber actorId)
//{
//    // read block from file
//    qDebug() << "signal has been connected";
//    QFile fileTransaction("path.dat");
//    QList<Transaction> returnList;
//    fileTransaction.open(QIODevice::ReadOnly | QIODevice::Text);

//    if (!fileTransaction.isOpen())
//        qDebug() << "file with transaction has not been open";

//    while (!fileTransaction.atEnd())
//    {
//        QByteArray transaction = fileTransaction.readLine();
//        //        qDebug() << "transaction from file:" << transaction;
//        Transaction tr(transaction);
//        //        qDebug() << "Object Transaction class" << tr.toString()
//        //                 << "tr.sender: " << tr.getSender().toString();

//        if ((tr.getSender() == actorId) || (tr.getReceiver() == actorId))
//            returnList.append(tr);
//    }

//    return returnList;
//}

// Wallet::Wallet(BigNumber actorId, QObject *parent)
//    : QObject(parent)
//{
//    this->actorId = actorId;
//    // User usr;
//    // qDebug() << "new actor: ";
//    // connect(this, &Wallet::getTransactionList, &usr, &User::getTransactionsList);
//    emit getTransactionList(actorId);
//    qDebug() << "signals has been emit: ";
//    // this->lastTransaction = User::getTransactionsList(this->actorId);
//    // qDebug() << "actorId : " << actorId;
//    this->lastTransaction = getTransactionsList(actorId);
//    // for (int i = 0; i < lastTransaction.count(); i++)
//    //     qDebug() << lastTransaction.at(i).toString();
//}

// QString Wallet::getActor(Transaction tr)
//{
//    return tr.getSender().toString();
//}

// QString Wallet::getAmout(Transaction tr)
//{
//    return tr.getAmount().toString();
//}

// QString Wallet::getBalance(Transaction tr)
//{
//    return tr.getSenderBalance().toString();
//}

// QString Wallet::getReciver(Transaction tr)
//{
//    return tr.getReceiver().toString();
//}

// Wallet::~Wallet()
//{
//}

// void Wallet::newTransaction(QString actor, QString amount)
//{
//    qDebug() << "[WalletBackend]"
//             << "New transaction for actor" << actor << "with amount" << amount;
//    emit sendNewTransaction(BigNumber(actor.toInt()), BigNumber(amount.toUtf8().toInt()));
//}

// void Wallet::setActorsState()
//{
//    qDebug() << "[WalletBackend] setActorsState";
//    emit takeAllMyWallet();
//}

// QString Wallet::setTransactionParametrs(const QString &flag, int counter)
//{
//    currentTransaction = this->lastTransaction.at(counter);
//    if (flag == "actor")
//        return getActor(currentTransaction);
//    else if (flag == "balance")
//        return getBalance(currentTransaction);
//    else if (flag == "amount")
//        return getAmout(currentTransaction);
//    else if (flag == "reciver")
//        return getReciver(currentTransaction);
//    else
//        return flag;
//}

// void Wallet::setActorsList(QMap<QByteArray, QByteArray> map)
//{
//    qDebug() << "[WalletBackend]" << map;
//    this->stateActors = map;
//}

// void Wallet::actorsListSet(QList<QByteArray> actorsList)
//{
//    qDebug() << "[WalletBackend]" << actorsList;
//    this->actorsList = actorsList;
//}
