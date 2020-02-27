#ifndef CHATMODEL_H
#define CHATMODEL_H

#include "ui/model/abstractmodel.h"
#include "managers/chat.h"

class ChatModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QString chatId READ chatId WRITE setChatId NOTIFY chatIdChanged)

public:
    explicit ChatModel(AbstractModel* parent = nullptr);
    QString chatId() const;

    Q_INVOKABLE QVariantMap searchMessage(const QString& messageId);

public slots:
    void setChatId(QString chatId);
    void chatReceived(QByteArray chatId, QList<UIMessage> messages);
    void messageReceived(QByteArray chatId, UIMessage message);

signals:
    void chatIdChanged(QString chatId);

private:
    void addMessage(const UIMessage& message);

    QString m_chatId;
};

#endif // CHATMODEL_H
