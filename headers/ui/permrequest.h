#ifndef PERMREQUEST_H
#define PERMREQUEST_H

#include <QObject>
#include <QJSValue>

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#endif

class PermRequest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList permissions READ permissions WRITE setPermissions NOTIFY
                   permissionsChanged)

public:
    explicit PermRequest(QObject* parent = nullptr);
    // Q_INVOKABLE bool permissionRequest(const QString& permission);
    Q_INVOKABLE void permissionsRequest(const QJSValue& func);
    Q_INVOKABLE bool checkPermissions();
    Q_INVOKABLE bool checkPermission(const QString& permission);
    QStringList permissions() const;

public slots:
    void setPermissions(QStringList permissions);

signals:
    void permissionsChanged(QStringList permissions);
    void permissionFinished(bool isPermission, QJSValue func);

private:
    QStringList m_permissions;
};

#endif // PERMREQUEST_H
