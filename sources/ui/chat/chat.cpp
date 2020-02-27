#include "ui/chat/chat.h"

Chat::Chat(QObject *parent)
    : QObject(parent)
{
}

Chat::Chat(const Chat &temp, QObject *parent)
    : Chat(parent)
{
    this->name = temp.name;
    this->chatKey = temp.chatKey;
    this->actorIdList = temp.actorIdList;
    this->fileName = temp.fileName;
    this->messageList = temp.messageList;
}

Chat::Chat(const QByteArray serialized, QObject *parent)
    : Chat(parent)
{
    QList<QByteArray> deserialized = Serialization::deserialize(serialized, Serialization::TX_FIELD_SPLITTER);
    this->fileName = deserialized.at(0);
    QList<QByteArray> list = Serialization::deserialize(deserialized.at(2), "_");
    while (!list.isEmpty())
        this->actorIdList.append(new Members(BigNumber(list.takeFirst())));
    this->name = deserialized.at(1);
    generateChatKey();
    chatLoader();
    // append to this list info ()
    // maybe activete slots here to dfs
    // and only after combinate Chat Model
    //
}

Chat::Chat(const QList<BigNumber> actorId, QObject *parent)
    : Chat(parent)
{
    QList<QByteArray> list;
    for (auto &&el : actorId)
    {

        this->actorIdList.append(new Members(el));
        list.append(el.toByteArray());
    }
    this->name = QString(Serialization::serialize(list, "_"));
    this->fileName = this->name;
}

Chat Chat::operator=(const Chat &temp)
{
    this->name = temp.name;
    this->chatKey = temp.chatKey;
    this->fileName = temp.fileName;
    this->actorIdList = temp.actorIdList;
    this->messageList = temp.messageList;
    return *this;
}

Chat::~Chat()
{
}

void Chat::generateChatKey()
{
    QList<QByteArray> keys;
    actorIdList.takeFirst()->getPubKey();
    for (auto &&el : actorIdList)
        keys.append(el->getPubKey());
    this->chatKey = "0";
    //
}

void Chat::chatLoader()
{
    emit loadedMessages(this->messageList.last());
}

void Chat::chatControls(QString mess, QString actorId)
{
    this->messageList.append(new ChatMessages(
        Serialization::serialize({ actorId.toUtf8(), mess.toUtf8() }, Serialization::TX_FIELD_SPLITTER)));
    QByteArray serializedMessage = this->messageList.last()->serialized();
    //    KeyPublic key(this->chatKey);
    // KeyPublic key(this->actorIdList[0]->getPubKey());
    //    emit sendMessage(this->messageList.last());
}

void Chat::showLastMessageList(int count)
{
    qDebug() << "----------------------------------\n-------------------------void "
                "Chat::showLastMessageList(int count)-----------------------\n"
                "-------------------------------------------------";
    emit requestMessageList(count, "this->fileName");
    //
    //    QFile file("..//DATA/1/SERVICE/chat_" + Serialization::serialize({ "1", "2" },
    //    "_")
    //               + ".dat");
    //    file.open(QIODevice::ReadOnly);
    //    QList<QByteArray> list = Serialization::deserialize2(
    //        file.readAll(), "\n" /*Serialization::INFORMATION_SEPARATOR_ONE*/);
    //    QList<ChatMessages> messageList;
    //    for (int i = list.length() - (count - 1) * 10 - 1; i < 0; i--)
    //        messageList.append(ChatMessages(list.at(list.length() - 1 - i)));
    //    qDebug() << "message list: " << list;
    //
    //    QList<QVariantMap> list;
    //    for (auto &&el : this->messageList)
    //        list.append(el.toVariantMap());
    //    emit showMessageList(list);
}

bool Chat::sendMessages(QString mess, QString actorId)
{
    //    ChatMessages *message = new ChatMessages(mess, BigNumber(actorId.toUtf8()));
    //    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << actorId;
    //    //    KeyPublic key(this->chatKey);
    //    //
    //    KeyPublic key(""), key1("");
    //    if (this->actorIdList.count() == 2)
    //    {
    //        if (this->actorIdList[0]->getActorId().toString() == actorId)
    //        {
    //            key = KeyPublic(this->actorIdList[0]->getPubKey());
    //            key1 = KeyPublic(this->actorIdList[1]->getPubKey());
    //        }
    //        else
    //        {
    //            key1 = KeyPublic(this->actorIdList[1]->getPubKey());
    //            key = KeyPublic(this->actorIdList[1]->getPubKey());
    //        }
    //    }
    //    qDebug() << key.getPublicKey();
    //    QList<QByteArray> actors;
    //    for (auto &&el : this->actorIdList)
    //        actors.append(el->getActorId().toActorId());
    //    QFile file("..//DATA/1/SERVICE/chat_"
    //               + Serialization::serialize(/*actors*/ { "1", "2" }, "_") + ".dat");
    //    file.open(QIODevice::WriteOnly | QIODevice::Append);
    //    file.write(message->serialized() + "\n");
    //    file.flush();
    //    file.close();
    //    this->messageList.append(message);
    //    emit sendMessage(key.encrypt(message->serialized()), "1_2");
    //    emit saveMessageDFS(key1.encrypt(message->serialized()), "1_2");
    return true;
}

void Chat::recieveMessageList(QList<ChatMessages> messageList)
{
    QList<QVariantMap> list;
    while (!messageList.isEmpty())
    {
        this->messageList.append(new ChatMessages(messageList.takeFirst()));
        list.append(this->messageList.last()->toVariantMap());
    }
    emit showMessageList(list);
    qDebug() << "result: - " << list;
}

void Chat::recieveMessage(ChatMessages mess)
{
    this->messageList.append(&mess);
    emit showMessage(this->messageList.last()->toVariantMap());
}
