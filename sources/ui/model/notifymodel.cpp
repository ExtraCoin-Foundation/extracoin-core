#include "ui/model/notifymodel.h"

NotifyModel::NotifyModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "type", "userId", "otherId", "data", "date" });
}

QVariantMap NotifyModel::prepareNotify(const notification &notify)
{
    QVariantMap map;

    map["type"] = notify.type;
    map["date"] = notify.time;
    map["userId"] = QString(notify.data);
    map["otherId"] = QString("");

    switch (notify.type)
    {
    case notification::TxToUser:
    case notification::TxToMe:
    case notification::NewFollower:
        break;
    case notification::ChatMsg:
    case notification::ChatInvite:
    case notification::NewPost:
    {
        QByteArrayList list = notify.data.split(' ');
        if (list.size() == 2)
        {
            map["userId"] = QString(list[0]);
            map["otherId"] = QString(list[1]);
        }
        break;
    }
    }

    return map;
}
