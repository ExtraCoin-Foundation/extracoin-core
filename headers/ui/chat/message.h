#ifndef MESSAGE_H
#define MESSAGE_H
#include <QObject>
#include <QDebug>

class Message1 : public QObject
{
    Q_OBJECT

private:
    QString text_message;
    QByteArray key;
    QStringList message_list; // convert to VariantMap in future
    // QVariantMap message_list[actorId] [message]

public:
    void decryptAndShow(QByteArray message, QString rx_id);
    Q_INVOKABLE void encryptAndSend(QString message, QString rx_id, QString tx_id);
    Q_INVOKABLE void getLastMassage(QString rx_id, QString tx_id);

public slots:
    void receiveMessageFromFile(QStringList message, QString actorId);

signals:
    void sendToQML(QStringList message, QString rx_id);
    void sendToDfsForSaving(QByteArray message, QByteArray rx_id, QByteArray tx_id);
    void sendToDfsForReading(QByteArray rx_id, QByteArray tx_id);
};

#endif // MESSAGE_H
