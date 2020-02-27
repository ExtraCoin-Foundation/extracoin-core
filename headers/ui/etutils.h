#ifndef ETUTILS_H
#define ETUTILS_H

#include <QDir>
#include <QSettings>
#include <QNetworkConfigurationManager>
#include <QClipboard>
#include <QApplication>
#include <QProcess>

#include "utils/utils.h"
#include "utils/db_connector.h"

#include "profile/profile.h"
#include "datastorage/searchfilters.h"

#if defined(Q_OS_ANDROID)
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#endif

class EtUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool networkActive READ networkActive WRITE setNetworkActive NOTIFY networkActiveChanged)
    Q_PROPERTY(bool isRelease READ isRelease WRITE setIsRelease NOTIFY isReleaseChanged)

public:
    EtUtils(QObject* parent = nullptr);

    // data readFile(file)
    // writeFile(file, data)
    Q_INVOKABLE QString keccak(const QString& value);
    Q_INVOKABLE bool fileExists(const QString& filePath);
    Q_INVOKABLE bool dirExists(const QString& dirPath);
    Q_INVOKABLE bool isImage(const QString& filePath);
    Q_INVOKABLE bool fileRemove(const QString& filePath);
    Q_INVOKABLE void vibrate(int milliseconds);
    Q_INVOKABLE void wipe();
    Q_INVOKABLE void softWipe(QByteArray currentId);
    Q_INVOKABLE bool firstIdCreated();
    Q_INVOKABLE QString toFilePath(const QString& path);
    Q_INVOKABLE QSize imageSize(const QString& path);
    Q_INVOKABLE QString serverIp();
    Q_INVOKABLE bool setServerIp(const QString& serverIp);
    Q_INVOKABLE bool serverAllowLocal();
    Q_INVOKABLE bool setAllowLocal(bool allowLocalServer);
    Q_INVOKABLE void copyDir(const QString& src, const QString& dst);
    Q_INVOKABLE uint filesCountInDir(const QString& path);
    Q_INVOKABLE Profile makeProfile();
    Q_INVOKABLE SearchFilters makeFilters();
    Q_INVOKABLE int privateProfileExist();
    Q_INVOKABLE QString country(int id);
    Q_INVOKABLE QVariantList findCountries(QString country);
    Q_INVOKABLE void copyToClipboard(const QString& str);
    Q_INVOKABLE void makePath(const QString& str);

    static QByteArray serialize(QList<QByteArray> list);
    static QString serializeStr(QStringList list);
    static QList<QByteArray> deserialize(const QString& serialized);
    static QStringList deserializeStr(const QString& serialized);
    static bool autoWipe(int wipe);
    bool networkActive() const;
    bool isRelease() const;
    Q_INVOKABLE QString clipboardText();
    Q_INVOKABLE QString getPureHtml(QString newText);
    Q_INVOKABLE QVariantList fieldNames(const QString& field);

    Q_INVOKABLE int freeMemory();
    Q_INVOKABLE bool isSqlite(QString filePath);
    Q_INVOKABLE bool isGif(QString fileUrl);
    Q_INVOKABLE QVariantList tableNames(QString dbPath);
    Q_INVOKABLE QVariantList tableData(QString dbPath, QString table);
    Q_INVOKABLE void openFolder(QString folder, QString program);

public slots:
    void setNetworkActive(bool networkActive);
    void setIsRelease(bool isRelease);

signals:
    void networkActiveChanged(bool networkActive);
    void isReleaseChanged(bool isRelease);

private:
#if defined(Q_OS_ANDROID)
    QAndroidJniObject vibratorService;
#endif
    QNetworkConfigurationManager ncm;
    bool m_networkActive = false;
    bool m_isRelease = false;
    QClipboard* clipboard = qApp->clipboard();
};

#endif // ETUTILS_H
