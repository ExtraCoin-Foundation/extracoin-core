#ifndef IOSUTILS_H
#define IOSUTILS_H

#include <QDir>
#include <QVariantList>

class iOSUtils : public QObject
{
    Q_OBJECT

public:
    explicit iOSUtils(QObject *parent = nullptr);
    static bool iPhoneLess();
    static QVariantMap fetchAssets(const int mediaType);
};

#endif // IOSUTILS_H
