#ifndef QTIOSNotifier_H
#define QTIOSNotifier_H

#include <QObject>

class QtIosNotifier: public QObject
{
    Q_OBJECT

public:
    QtIosNotifier();

public:
    bool show(const QString &notifyString);

private:
    void                *m_Delegate;
};

#endif // QTIOSNotifier_H
