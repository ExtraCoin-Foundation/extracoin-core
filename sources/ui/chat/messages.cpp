#include "ui/chat/messages.h"

ChatMessages::ChatMessages()
{
}

ChatMessages::ChatMessages(const QByteArray &serialized)
    : ChatMessages()
{
    QList<QByteArray> list = Serialization::deserialize(serialized, Serialization::TX_FIELD_SPLITTER);
    if (list.length() == 3)
    {
        this->time = list.at(2);
        this->message = QString(list.at(1));
        this->actorId = BigNumber(list.at(0));
    }
    else
    {
        qDebug() << "incorect input data to message serialized";
    }
}
ChatMessages::ChatMessages(const QString &message, const Members &actorId)
    : ChatMessages()
{
    this->message = message;
    this->time = QString::number(QDateTime().currentSecsSinceEpoch()).toUtf8();
    this->actorId = actorId;
}

QVariantMap ChatMessages::toVariantMap() const
{
    QVariantMap temp;
    temp["message"] = this->message;
    temp["time"] = this->time;
    temp["actorId"] = QString(this->actorId.getActorId().toActorId());
    return temp;
}

QString ChatMessages::getMessage() const
{
    return this->message;
}

BigNumber ChatMessages::getActorId() const
{
    return this->actorId.getActorId();
}

QString ChatMessages::getTime() const
{
    return QString(this->time);
}

QByteArray ChatMessages::serialized() const
{
    return Serialization::serialize(
        { this->actorId.getActorId().toActorId(), this->message.toUtf8(), this->time },
        Serialization::TX_FIELD_SPLITTER);
}
