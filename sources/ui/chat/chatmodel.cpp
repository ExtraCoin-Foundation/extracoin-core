#include "ui/chat/chatmodel.h"

ChatModel::ChatModel(QObject *parent)
    : QObject(parent)
{
    //    this->chat = new Chat();
    //    this->chatList.append(Chat());
}
Chat ChatModel::getChat()
{
    //    return this->chatList.last();
    return Chat();
}
QList<QVariantMap> ChatModel::getLastDialogs()
{
    return QList<QVariantMap>();
}

void ChatModel::openChat(int chatId)
{
    this->chatList.at(chatId);
}
void ChatModel::loadChats()
{
    emit requestChatsList();
}
void ChatModel::recieveChatsList(QList<QByteArray> serialized)
{
    while (!serialized.isEmpty())
    {
        this->chatList.append(Chat(serialized.takeFirst()));
    }
}
