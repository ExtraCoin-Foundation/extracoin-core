#ifndef EVENTSMODEL_H
#define EVENTSMODEL_H

#include <QVariant>
#include <QList>
#include <QDebug>

#include "ui/model/abstractmodel.h"
#include "dfs/managers/headers/card_manager.h"

class EventsModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)

public:
    explicit EventsModel(AbstractModel *parent = nullptr);
    ~EventsModel();

    Q_INVOKABLE void loadEvents();
    void addEvent(QVariantMap);
    Q_INVOKABLE void clear();
    QString userId() const;
    Q_INVOKABLE QVariantMap event(QString userId, QString file);

public slots:
    void setUserId(QString userId);
    void loadEvent(const QString &eventFile);

signals:
    void userIdChanged(QString userId);

private:
    void loadMyEvents();

    std::vector<std::string> files;
    QString m_userId = "-333";
};

#endif // EVENTSMODEL_H
