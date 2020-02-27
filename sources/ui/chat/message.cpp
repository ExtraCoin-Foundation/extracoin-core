#include "ui/chat/message.h"

void Message1::decryptAndShow(QByteArray message, QString rx_id)
{
    Q_UNUSED(message)
    Q_UNUSED(rx_id)
    // emit sendToQML(message, rx_id);
}

void Message1::encryptAndSend(QString message, QString rx_id, QString tx_id)
{
    emit sendToDfsForSaving(message.toUtf8(), rx_id.toUtf8(), tx_id.toUtf8());
}

void Message1::getLastMassage(QString rx_id, QString tx_id)
{
    emit sendToDfsForReading(rx_id.toUtf8(), tx_id.toUtf8());
}

void Message1::receiveMessageFromFile(QStringList message, QString actorId)
{
    qDebug() << "messahe:::::::::::____________________::::::; ----------     ;) " << message << actorId;
    // QString temp = message;
    emit sendToQML(message, actorId);
}
