#ifndef CHAT_H
#define CHAT_H

#include <QObject>
#include <QString>
#include <QList>
#include "ui/chat/members.h"
#include "ui/chat/messages.h"
#include "ui/model/abstractmodel.h"
//#include "enc/ecc/key_public.h"
//#include "enc/ecc/ecc.h"
#include "utils/bignumber.h"

class Chat : public QObject
{
    Q_OBJECT

private:
    QString name;
    QList<Members *> actorIdList;
    QList<ChatMessages *> messageList;
    QString fileName;
    QByteArray chatKey;

public:
    Chat(QObject *parent = nullptr);
    Chat(const Chat &temp, QObject *parent = nullptr);
    Chat(const QByteArray serialized, QObject *parent = nullptr);
    Chat(const QList<BigNumber> actorId, QObject *parent = nullptr);
    Chat operator=(const Chat &temp);
    ~Chat();
    // function get
    QString getName();
    QString getFileName();
    void generateChatKey();
    void chatLoader();
    Q_INVOKABLE void chatControls(QString mess, QString actorId);
    Q_INVOKABLE void showLastMessageList(int count);
    Q_INVOKABLE bool sendMessages(QString mess, QString actorId);
signals:
    void sendMessage(QByteArray mess, QByteArray actor);
    void loadedMessages(ChatMessages *lastMess);
    void showMessage(QVariantMap message);
    void showMessageList(QList<QVariantMap> list);
    void requestMessageList(int count, QString path);
    void saveMessageDFS(QByteArray mess, QByteArray actor);
public slots:
    void recieveMessageList(QList<ChatMessages> messageList);
    void recieveMessage(ChatMessages mess);
};

#endif // CHAT_H
