#include "ui/chat/members.h"

Members::Members(QObject *parent)
    : QObject(parent)
{
}

Members::Members(const Members &temp, QObject *parent)
    : Members(parent)
{
    this->name = temp.name;
    this->actorId = temp.actorId;
    this->avatarPath = temp.avatarPath;
    this->pubKey = temp.pubKey;
}

Members::Members(const QByteArray serialized, QObject *parent)
    : Members(parent)
{
    QList<QByteArray> list =
        Serialization::deserialize(serialized, Serialization::TX_FIELD_SPLITTER);
    if (list.length() == 2)
    {
        this->actorId = BigNumber(list.at(0));
        this->pubKey = list.at(1);
    }
    else
    {
        qDebug() << "Incorrect chat members created";
    }
}

Members::Members(const BigNumber actorId, QObject *parent)
    : Members(parent)
{
    this->actorId = actorId;
    emit requestUserData(this->actorId);
}

Members::~Members()
{
}

Members Members::operator=(const Members &actor)
{
    this->actorId = actor.actorId;
    this->name = actor.name;
    this->pubKey = actor.pubKey;
    this->avatarPath = actor.avatarPath;

    return *this;
}

BigNumber Members::getActorId() const
{
    return this->actorId;
}

QString Members::getName() const
{
    return this->name;
}

QByteArray Members::getPubKey() const
{
    return this->pubKey;
}

QString Members::getAvatarPath() const
{
    return this->avatarPath;
}
void Members::recieveUserData(QByteArray pubKey)
{
    this->pubKey = pubKey;
}
