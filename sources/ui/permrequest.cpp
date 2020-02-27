#include "ui/permrequest.h"

#include <QDebug>

PermRequest::PermRequest(QObject* parent)
    : QObject(parent)
{
}

void PermRequest::permissionsRequest(const QJSValue& func)
{
#ifdef Q_OS_ANDROID
    auto permissionCallback = [this, func](const QtAndroid::PermissionResultMap& result) {
        auto i = QHashIterator<QString, QtAndroid::PermissionResult>(result);
        bool isPermission = true;

        while (i.hasNext())
        {
            i.next();

            if (i.value() == QtAndroid::PermissionResult::Denied)
            {
                isPermission = false;
                break;
            }
        }

        emit this->permissionFinished(isPermission, func);
    };

    QtAndroid::requestPermissions(m_permissions, permissionCallback);
#else
    emit permissionFinished(true, func);
#endif
}

bool PermRequest::checkPermissions()
{
    for (auto&& permission : m_permissions)
    {
        if (!checkPermission(permission))
            return false;
    }

    return true;
}

bool PermRequest::checkPermission(const QString& permission)
{
#ifdef Q_OS_ANDROID
    return QtAndroid::checkPermission(permission) == QtAndroid::PermissionResult::Granted;
#else
    Q_UNUSED(permission)
    return true;
#endif
}

QStringList PermRequest::permissions() const
{
    return m_permissions;
}

void PermRequest::setPermissions(QStringList permissions)
{
    if (m_permissions == permissions)
        return;

    m_permissions = permissions;
    emit permissionsChanged(m_permissions);
}
