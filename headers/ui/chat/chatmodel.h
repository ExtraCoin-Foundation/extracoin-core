#ifndef CHATMODEL_H
#define CHATMODEL_H
#include "ui/model/abstractmodel.h"
#include "ui/chat/chat.h"

class ChatModel : public QObject
{
    Q_OBJECT
private:
    //    Chat *chat;
    QList<Chat> chatList;
    QString avatarPath;
    QString lastMessage;
    QString chatName;
    void loadChats();

public:
    ChatModel(QObject *parent = nullptr);
    Chat getChat();
    Q_INVOKABLE QList<QVariantMap> getLastDialogs();
    Q_INVOKABLE void openChat(int chatId);
signals:
    void requestChatsList();
public slots:
    void recieveChatsList(QList<QByteArray> serialized);
};

#endif // CHATMODEL_H
