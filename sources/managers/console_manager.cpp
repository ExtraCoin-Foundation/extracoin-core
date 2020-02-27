#ifdef EXTRACOIN_CONSOLE

#include "managers/console_manager.h"
#include "managers/logs_manager.h"
#include "managers/node_manager.h"

#include <QTextStream>
#include <QProcess>

#ifdef Q_OS_LINUX
#include <unistd.h> // STDIN_FILENO
#endif

#ifdef Q_OS_WINDOWS
// #include <Windows.h>
#endif

ConsoleManager::ConsoleManager(QObject *parent)
    : QObject(parent)
#ifdef Q_OS_LINUX
    , notifier(STDIN_FILENO, QSocketNotifier::Read)
#else
// notifier(0, QSocketNotifier::Read)
#endif
{
}

ConsoleManager::~ConsoleManager()
{
#ifdef Q_OS_WIN
    // m_notifier->deleteLater();
#endif
}

void ConsoleManager::text()
{
    QTextStream qin(stdin);
    QString line = qin.readLine();
    emit textReceived(line);
}

void ConsoleManager::commandReceiver(QString command)
{
    command = command.toLower();
    qDebug() << "text from console:" << command;

    if (nodeManager->listenCoinRequest())
    {
        auto &requestQueue = nodeManager->requestCoinQueue();
        auto request = requestQueue.takeFirst();

        if (command == "y")
        {
            auto [receiver, amount, plsr] = request;
            nodeManager->sendCoinRequest(receiver, amount);
        }

        nodeManager->setListenCoinRequest(false);
        if (requestQueue.length() > 0)
        {
            request = requestQueue.takeFirst();
            auto [receiver, amount, plsr] = request;
            nodeManager->coinResponse(receiver, amount, plsr);
        }

        return;
    }

    if (command == "quit" || command == "exit")
    {
        qDebug() << "Exit...";
        qApp->quit();
    }

    if (command == "wipe")
    {
        Utils::wipeDataFiles();
        qDebug() << "Wiped and exit...";
        qApp->quit();
    }

    if (command == "logs on")
    {
        LogsManager::on();
        qDebug() << "Logs enabled";
    }

    if (command == "logs off")
    {
        qDebug() << "Logs disabled";
        LogsManager::off();
    }

    if (command.left(3) == "dir")
    {
        auto list = command.split(" ");
        if (list.length() == 2)
        {
            QString program = list[1].trimmed();

            if (!program.isEmpty())
            {
                QProcess::execute(program, { QDir::currentPath() });
                return;
            }
        }

#if defined(Q_OS_LINUX)
        QProcess::execute("xdg-open", { QDir::currentPath() });
#elif defined(Q_OS_WIN)
        QProcess::execute("explorer.exe", { QDir::currentPath().replace("/", "\\") });
#elif defined(Q_OS_MAC)
        QProcess::execute("open", { QDir::currentPath() });
#else
        qDebug() << "Command \"dir\" not implemented for this platform";
#endif
    }

    if (command.left(6) == "sendtx")
    {
        qDebug() << "sendtx";
        auto mainActorId = accController->getMainActor()->getId();
        BigNumber companyId = BigNumber(*nodeManager->getAccController()->getActorIndex()->companyId);

        QStringList sendtx = command.split(" ");
        if (sendtx.length() == 3)
        {
            QByteArray toId = sendtx[1].toUtf8();
            QByteArray toAmount = sendtx[2].toUtf8();
            qDebug() << "sendtx" << toId << toAmount;

            BigNumber receiver(toId);
            BigNumber amount = Transaction::visibleToAmount(toAmount);

            if (mainActorId != companyId)
                nodeManager->createTransaction(receiver, amount);
            else
            {
                for (int i = 0; i != 50; ++i)
                {
                    amount++;
                    nodeManager->createTransactionFrom(companyId, receiver, amount);
                }
            }
        }
    }

    if (command == "getconnsize" || command == "conns")
    {
        qDebug() << "Connections:" << nodeManager->getClientList();
    }

    if (command.left(4) == "gena")
    {
        auto list = command.split(" ");

        if (list.length() != 2)
            return;

        long long count = list[1].toLongLong();

        for (long long i = 0; i != count; ++i)
            accController->createActor(true);
        qDebug() << "----------------------------------------------------------------"
                 << "Actor generation complete, count:" << count;
    }
}

void ConsoleManager::setNodeManager(NodeManager *value)
{
    nodeManager = value;
    accController = nodeManager->getAccController();
    netManager = nodeManager->getNetManager();
}

void ConsoleManager::startInput()
{
#if defined(Q_OS_WIN)
    std::cout << std::flush;
    // m_notifier = new QWinEventNotifier(GetStdHandle(STD_INPUT_HANDLE));
    // connect(m_notifier, &QWinEventNotifier::activated, this, &ConsoleManager::text);
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
    connect(&notifier, &QSocketNotifier::activated, this, &ConsoleManager::text);
#endif

    connect(this, &ConsoleManager::textReceived, this, &ConsoleManager::commandReceiver);
}

QString ConsoleManager::getSomething(const QString &name)
{
    QString something;

    qInfo().noquote().nospace() << "Enter " + name + ":";

    QTextStream cin(stdin);
    while (something.isEmpty())
        cin >> something;

    if (something == "wipe")
    {
        Utils::wipeDataFiles();
        qDebug() << "Wiped and exit...";
        std::exit(0);
    }

    if (something == "empty")
        something = "";

    return something;
}

#endif
