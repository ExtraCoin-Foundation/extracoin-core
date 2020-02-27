#ifndef NOTIFYMODEL_H
#define NOTIFYMODEL_H

#include "ui/model/abstractmodel.h"
#include "managers/notification_manager.h"

class NotifyModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit NotifyModel(AbstractModel* parent = nullptr);
    QVariantMap prepareNotify(const notification& notify);

public slots:

signals:

private:
};

#endif // NOTIFYMODEL_H
