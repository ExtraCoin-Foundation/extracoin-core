#include "ui/profiledata.h"
//#include <QDebug>

WelcomePage::WelcomePage(QObject *parent)
    : QObject(parent)
{
    this->status = false;
    this->currentActorId = BigNumber("-1");
}

WelcomePage::~WelcomePage()
{
}

int WelcomePage::passwordRule(QString password)
{ // TODO: regexp
    int charCount = 0, intCount = 0;
    bool flag = true;
    for (int i = 0; i < password.length(); i++)
    {
        if ((password[i] >= '0') && (password[i] <= '9'))
            intCount++;
        else if ((password[i] >= 'A') && (password[i] <= 'Z'))
            charCount++;
        else if ((password[i] >= 'a') && (password[i] <= 'z'))
            flag = true;
        else
            flag = false;
    }
    if ((flag) && ((intCount > 0) && (charCount > 0)))
    {
        if (password.length() < 6)
            return 5;
        return 0;
    }
    else
    {
        if (!flag)
            return 1;
        else if (charCount < 1)
            return 3;
        else if (intCount < 1)
            return 2;
        else
            return 4;
    }
}

bool WelcomePage::passwordReapeat(QList<QString> data)
{
    if (data[2] != data[3])
        return false;
    else
        return true;
}

bool WelcomePage::emailRule(QList<QString> data)
{
    QString email = data[1];
    return email.count("@") == 1 && email.lastIndexOf("@") < email.lastIndexOf('.');
}

bool WelcomePage::emailCheck(QString data)
{
    return emailRule(QStringList({ "", data }));
}

QString WelcomePage::hash() const
{
    return m_email.isEmpty() && m_password.isEmpty() ? "" : Utils::calcKeccak(m_email + m_password);
}

void WelcomePage::newUserHash(QByteArray hash)
{
    m_userHash = hash;
}

void WelcomePage::autoLogIn()
{
    emit autoLogInStarted();
}

bool WelcomePage::isReg() const
{
    return m_isReg;
}

QByteArray WelcomePage::email() const
{
    return m_email;
}

QByteArray WelcomePage::password() const
{
    return m_password;
}

QByteArray WelcomePage::getHash() const
{
    return m_userHash;
}

QByteArray WelcomePage::serializeUserData() const
{
    QList<QByteArray> data_array = {};
    data_array << m_email << m_password;
    return Serialization::serialize(data_array, Serialization::TX_PAIR_FIELD_SPLITTER);
}

void WelcomePage::startReg(QByteArray email, QByteArray password)
{
    QByteArray hash = Utils::calcKeccak(email + password);
    regStarted(hash, 1);
}

void WelcomePage::setIsReg(bool isReg)
{
    if (m_isReg == isReg)
        return;

    m_isReg = isReg;
    emit isRegChanged(m_isReg);
}

void WelcomePage::setEmail(QByteArray email)
{
    if (m_email == email)
        return;

    m_email = email;
    emit emailChanged(m_email);
}

void WelcomePage::setPassword(QByteArray password)
{
    if (m_password == password)
        return;

    m_password = password;
    emit passwordChanged(m_password);
}
