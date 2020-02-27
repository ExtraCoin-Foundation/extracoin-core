#ifndef MESSAGES_H
#define MESSAGES_H

#include <QString>
#include "utils/bignumber.h"
#include "ui/chat/members.h"
#include <QDateTime>
#include <QObject>

class ChatMessages
{
private:
    QString message;
    Members actorId;
    QByteArray time;

public:
    ChatMessages();
    ChatMessages(const QByteArray &serialized);
    ChatMessages(const QString &message, const Members &actorId);
    QVariantMap toVariantMap() const;
    QString getMessage() const;
    BigNumber getActorId() const;
    QString getTime() const;
    QByteArray serialized() const;
};

#endif // MESSAGE_H
