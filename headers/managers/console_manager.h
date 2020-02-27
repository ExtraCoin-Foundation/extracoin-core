#ifdef EXTRACOIN_CONSOLE
#ifndef READER_H
#define READER_H

#include <QDebug>
#include <QCoreApplication>

#ifdef Q_OS_WIN
// #include <QWinEventNotifier>
#else
#include <QSocketNotifier>
#endif

class NodeManager;
class AccountController;
class NetManager;

class ConsoleManager : public QObject
{
    Q_OBJECT

public:
    explicit ConsoleManager(QObject *parent = nullptr);
    ~ConsoleManager();
    void setNodeManager(NodeManager *value);
    void startInput();

    static QString getSomething(const QString &name);

signals:
    void textReceived(QString message);

public slots:
    void text();
    void commandReceiver(QString command);

private:
#ifdef Q_OS_WIN
//    QWinEventNotifier *m_notifier;
#else
    QSocketNotifier notifier;
#endif

    NodeManager *nodeManager;
    AccountController *accController;
    NetManager *netManager;
};

#endif // READER_H
#endif
