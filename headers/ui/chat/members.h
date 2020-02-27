#ifndef MEMBERS_H
#define MEMBERS_H

#include "utils/bignumber.h"
#include "utils/utils.h"
#include <QByteArray>
#include <QList>
#include <QObject>

class Members : public QObject
{
    Q_OBJECT
private:
    BigNumber actorId;
    QByteArray pubKey;
    QString name;
    QString avatarPath;

public:
    Members(QObject *parent = nullptr);
    Members(const Members &temp, QObject *parent = nullptr);
    Members(const QByteArray serialized, QObject *parent = nullptr);
    Members(const BigNumber actorId, QObject *parent = nullptr);
    ~Members();
    Members operator=(const Members &actor);
    BigNumber getActorId() const;
    QString getName() const;
    QByteArray getPubKey() const;
    QString getAvatarPath() const;
signals:
    void requestUserData(BigNumber actorId);
public slots:
    void recieveUserData(QByteArray pubKey);
};
#endif // MEMBERS_H
