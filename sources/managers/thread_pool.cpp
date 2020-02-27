#include "managers/thread_pool.h"

QThread *ThreadPool::addThread(QObject *worker)
{
    return ThreadPool::addThread(QList<QObject *>() << worker);
}

QThread *ThreadPool::addThread(QList<QObject *> workers)
{
    static int threadCount = 0;
    // mutex.tryLock();
    static QList<QThread *> threads;
    static bool isFirst = true;

    QThread *thread = new QThread();
    for (const auto &worker : workers)
    {
        //#ifdef EXTRACOIN_CLIENT
        worker->moveToThread(thread);
        //#endif
        QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
        QObject::connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        QObject::connect(thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    }

    QObject::connect(thread, &QThread::finished, [thread, workers]() {
        // threadCount--;
        //        threads.removeOne(thread); // ERROR!!!
        //        qDebug() << "Remove thread"
        //                 << "from pool with new length" << threadCount;
        thread->deleteLater();
    });

    if (isFirst)
    {
        qDebug() << "Connected with qApp";
        QObject::connect(qApp, &QCoreApplication::aboutToQuit, []() {
            qDebug() << "Remove all threads" << threadCount;

            for (auto &&thread : threads)
                thread->quit();
        });

        isFirst = false;
    }

    //    threads << thread;
    // threadCount++;
    //    qDebug() << "Add thread for" << workers << "to pool with new length" << threadCount;
    // mutex.unlock();
    thread->start();
    return thread;
}
