#include "ui/model/chatmodel.h"
ChatModel::ChatModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "messId", "type", "userId", "text", "date", "data", "replyId" });
}

QString ChatModel::chatId() const
{
    return m_chatId;
}

void ChatModel::setChatId(QString chatId)
{
    if (m_chatId == chatId)
        return;

    m_chatId = chatId;
    emit chatIdChanged(m_chatId);
}

void ChatModel::chatReceived(QByteArray chatId, QList<UIMessage> messages)
{
    if (chatId != m_chatId)
        return;

    if (messages.length() > 0)
        clear();

    for (auto &&message : messages)
    {
        addMessage(message);
    }

    setChatId(chatId);
}

void ChatModel::messageReceived(QByteArray chatId, UIMessage message)
{
    if (this->m_chatId == chatId)
    {
        addMessage(message);
    }
}

void ChatModel::addMessage(const UIMessage &message)
{
    QVariantMap prevMessage = get(1);

    auto json = QJsonDocument::fromJson(message.message.toUtf8());
    QString type = json["type"].toString();

    QDateTime prevDate = prevMessage["date"].toDateTime();
    if (prevDate.daysTo(message.date) >= 1 || type == "first")
        insert(0, { { "type", "date" }, { "date", message.date } });

    insert(0,
           { { "type", type },
             { "messId", message.messId },
             { "userId", message.userId },
             { "text", json["message"].toString() },
             { "date", message.date },
             { "data", json["data"].toString() },
             { "replyId", json["reply"].toString() } });
}

QVariantMap ChatModel::searchMessage(const QString &messageId)
{
    QList<QVariantMap> &messages = list();

    for (int i = 0; i != messages.length(); i++)
    {
        if (messages[i]["messId"].toString() == messageId)
        {
            QVariantMap map = messages[i];
            map["index"] = i;
            return map;
        }
    }

    return QVariantMap();
}
