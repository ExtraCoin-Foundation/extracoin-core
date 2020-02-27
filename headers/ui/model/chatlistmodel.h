#ifndef CHATLISTMODEL_H
#define CHATLISTMODEL_H

#include "ui/model/abstractmodel.h"
#include "ui/model/chatmodel.h"
#include "managers/chat.h"

class UiController;

class ChatListModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QString currentChatId READ currentChatId WRITE setCurrentChatId NOTIFY currentChatIdChanged)

public:
    explicit ChatListModel(UiController* ui, AbstractModel* parent = nullptr);
    QString currentChatId() const;

    Q_INVOKABLE void resetUnread(const QString& chatId);

signals:
    void sendMessage(QByteArray chatId, QByteArray message);
    void currentChatIdChanged(QString currentChatId);

public slots:
    void chatListReceived(QList<UIChat> chats);
    void chatAdded(UIChat chat);
    void messageReceived(QByteArray chatId, UIMessage message);
    void removeChat(QByteArray chatId);
    void setCurrentChatId(QString currentChatId);

private:
    UiController* ui;

    QString m_currentChatId;
};

#endif // CHATLISTMODEL_H
