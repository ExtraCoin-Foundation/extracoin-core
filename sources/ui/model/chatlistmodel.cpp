#include "ui/model/chatlistmodel.h"

#include "ui/ui_controller.h"

ChatListModel::ChatListModel(UiController *ui, AbstractModel *parent)
    : AbstractModel(parent)
{
    this->ui = ui;
    setModelRoles({ "users", "userId", "chatId", "text", "date", "unread", "name", "lastUserId",
                    "lastMessage", "lastType" });
}

void ChatListModel::chatListReceived(QList<UIChat> chats)
{
    qDebug() << "ChatListModel::chatListReceived";

    QString currentId = ui->myId();
    clear();

    for (auto &&chat : chats)
    {
        if (chat.users.length() != 2)
            continue;

        auto messageJson = QJsonDocument::fromJson(chat.lastMessage.message.toUtf8());
        QString type = messageJson["type"].toString();
        type = type.isEmpty() ? "message" : type;
        QString message = messageJson["message"].toString();
        qDebug() << "messageJson" << messageJson;

        QString userId = chat.users[0] == currentId ? chat.users[1] : chat.users[0];
        QVariantMap chatMap = { { "users", chat.users },
                                { "userId", userId },
                                { "chatId", chat.chatId },
                                { "lastUserId", chat.lastMessage.userId },
                                { "lastType", type },
                                { "lastMessage", message },
                                { "date", chat.lastMessage.date },
                                { "unread", 0 } };
        QDateTime currentPostDate = chatMap["date"].toDateTime();

        if (count())
        {
            for (int i = 0; i != count(); i++)
            {
                QDateTime postDate = get(i)["date"].toDateTime();

                if (postDate <= currentPostDate)
                {
                    insert(i, chatMap);
                    break;
                }
                else
                {
                    insert(i + 1, chatMap);
                    break;
                }
            }
        }
        else
        {
            append(chatMap);
        }
    }
}

void ChatListModel::chatAdded(UIChat chat)
{
    if (chat.users.length() != 2)
        return;

    QString userId = chat.users[0] == ui->myId() ? chat.users[1] : chat.users[0];
    if (userId == ui->myId())
        return;

    auto messageJson = QJsonDocument::fromJson(chat.lastMessage.message.toUtf8());
    QString type = messageJson["type"].toString();
    type = type.isEmpty() ? "message" : type;
    QString message = messageJson["message"].toString();

    insert(0,
           { { "users", chat.users },
             { "userId", userId },
             { "chatId", chat.chatId },
             { "lastUserId", chat.lastMessage.userId },
             { "lastType", type },
             { "lastMessage", message },
             { "date", chat.lastMessage.date },
             { "unread", 0 } });
}

void ChatListModel::messageReceived(QByteArray chatId, UIMessage message)
{
    Q_UNUSED(message)
    QList<QVariantMap> &chats = list();

    int i = 0;
    for (auto &&chat : chats)
    {
        QString loopChatId = chat["chatId"].toString();

        if (loopChatId == chatId)
        {
            auto messageJson = QJsonDocument::fromJson(message.message.toUtf8());
            qDebug() << "messageJson" << messageJson;
            QString type = messageJson["type"].toString();
            // type = type.isEmpty() ? "message" : type;
            QString messageText = messageJson["message"].toString();

            set(i, "date", message.date);
            set(i, "lastUserId", message.userId);
            set(i, "lastMessage", messageText);
            set(i, "lastType", type);

            if (m_currentChatId != loopChatId && ui->myId() != message.userId)
            {
                int unread = chat["unread"].toInt() + 1;
                set(i, "unread", unread);
            }
            else
            {
                set(i, "unread", 0);
            }

            move(i, 0, 1);
            break;
        }

        i++;
    }
}

void ChatListModel::removeChat(QByteArray chatId)
{
    QList<QVariantMap> &chats = list();

    int i = 0;
    for (auto &&chat : chats)
    {
        if (chat["chatId"].toString() == chatId)
        {
            remove(i, 1);
            break;
        }

        i++;
    }
}

QString ChatListModel::currentChatId() const
{
    return m_currentChatId;
}

void ChatListModel::resetUnread(const QString &chatId)
{
    QList<QVariantMap> &chats = list();
    int i = 0;

    for (auto &&chat : chats)
    {
        if (chat["chatId"].toString() == chatId)
        {
            set(i, "unread", 0);
            break;
        }

        i++;
    }
}

void ChatListModel::setCurrentChatId(QString currentChatId)
{
    if (m_currentChatId == currentChatId)
        return;

    m_currentChatId = currentChatId;
    emit currentChatIdChanged(m_currentChatId);
}
