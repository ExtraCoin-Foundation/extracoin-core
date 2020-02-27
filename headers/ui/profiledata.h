#ifndef PROFILEDATA_H
#define PROFILEDATA_H

#include <QObject>
#include <QList>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "utils/utils.h"
#include "utils/bignumber.h"
#include "enc/crypt_manager.h"

class WelcomePage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isReg READ isReg WRITE setIsReg NOTIFY isRegChanged)
    Q_PROPERTY(QByteArray email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QByteArray password READ password WRITE setPassword NOTIFY passwordChanged)

private:
    bool status;
    BigNumber currentActorId;

    bool passwordReapeat(QList<QString> data);
    bool emailRule(QList<QString> data);

    bool m_isReg;
    QByteArray m_email;
    QByteArray m_password;
    QByteArray m_profileData;
    QByteArray m_userHash;

public:
    WelcomePage(QObject* parent = nullptr);
    ~WelcomePage();

    Q_INVOKABLE int passwordRule(QString data);
    Q_INVOKABLE bool emailCheck(QString data);
    Q_INVOKABLE QString hash() const;
    Q_INVOKABLE void newUserHash(QByteArray hash);
    Q_INVOKABLE void autoLogIn();

    bool isReg() const;
    QByteArray email() const;
    QByteArray password() const;
    QByteArray getHash() const;

    QByteArray getChanges() const;
    QByteArray serializeUserData() const;

    Q_INVOKABLE void startReg(QByteArray email, QByteArray password);

signals:
    void regStarted(QByteArray hash, const bool account);
    // to userControler
    void logInStarted();
    //
    void autoLogInStarted();

    void isRegChanged(bool isReg);
    void emailChanged(QByteArray email);
    void passwordChanged(QByteArray password);

public slots:
    void setIsReg(bool isReg);
    void setEmail(QByteArray email);
    void setPassword(QByteArray password);
};

#endif // PROFILEDATA_H
