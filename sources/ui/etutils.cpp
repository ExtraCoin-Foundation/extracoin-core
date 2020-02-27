#include "ui/etutils.h"

#include <QCryptographicHash>
#include <QElapsedTimer>
#include <QImageReader>
#include <QFileInfoList>
#include <QUrl>

EtUtils::EtUtils(QObject *parent)
    : QObject(parent)
{
#if defined(Q_OS_ANDROID)
    QAndroidJniObject vibroString = QAndroidJniObject::fromString("vibrator");
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod(
        "org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
    QAndroidJniObject appctx =
        activity.callObjectMethod("getApplicationContext", "()Landroid/content/Context;");
    vibratorService = appctx.callObjectMethod("getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;",
                                              vibroString.object<jstring>());
#endif

    auto networkState = [this](bool isOnline) {
        bool isActive = false;

        QList<QNetworkConfiguration> activeConfigs = ncm.allConfigurations(QNetworkConfiguration::Active);
        if (activeConfigs.count() > 0)
            isActive = true;
        isActive = true;

        qDebug() << "[UI Network Status]" << (isOnline && isActive);
        setNetworkActive(isOnline && isActive);
    };

    networkState(true);
    QObject::connect(&ncm, &QNetworkConfigurationManager::onlineStateChanged, networkState);

#ifdef QT_DEBUG
    setIsRelease(false);
#else
    setIsRelease(true);
#endif
}

QString EtUtils::keccak(const QString &value)
{
    return QCryptographicHash::hash(value.toUtf8(), QCryptographicHash::Keccak_256).toHex();
}

bool EtUtils::fileExists(const QString &filePath)
{
    return QFile::exists(QUrl(filePath).toLocalFile());
}

bool EtUtils::dirExists(const QString &dirPath)
{
    return QDir(QUrl(dirPath).toLocalFile()).exists();
}

bool EtUtils::isImage(const QString &filePath)
{
    QImageReader image(QUrl(filePath).toLocalFile());
    return image.canRead();
}

bool EtUtils::fileRemove(const QString &filePath)
{
    return QFile::remove(QUrl(filePath).toLocalFile());
}

void EtUtils::vibrate(int milliseconds)
{
#if defined(Q_OS_ANDROID)
    if (vibratorService.isValid())
    {
        jlong ms = milliseconds;
        jboolean hasvibro = vibratorService.callMethod<jboolean>("hasVibrator", "()Z");
        vibratorService.callMethod<void>("vibrate", "(J)V", ms);
    }
    else
    {
        qDebug() << "[Android] No vibrator service available";
    }
}
#else
    Q_UNUSED(milliseconds)
}
#endif

void EtUtils::wipe()
{
    Utils::wipeDataFiles();
}

void EtUtils::softWipe(QByteArray currentId)
{
    Utils::softWipe(currentId);
}

bool EtUtils::firstIdCreated()
{
    return QDir("data/1").exists();
}

QString EtUtils::toFilePath(const QString &path) // from absolute path to file:///
{
    return QUrl::fromLocalFile(path).toString();
}

QSize EtUtils::imageSize(const QString &path)
{
    QImageReader image(QUrl(path).toLocalFile());
    return image.size();
}

QString EtUtils::serverIp()
{
    QSettings settings;

    if (!settings.value("network/serverIp").isValid())
        settings.setValue("network/serverIp", Network::serverIp);

    return settings.value("network/serverIp").toString();
}

bool EtUtils::setServerIp(const QString &serverIp)
{
    if (serverIp == this->serverIp())
        return false;

    QSettings settings;
    settings.setValue("network/serverIp", serverIp);
    return true;
}

bool EtUtils::serverAllowLocal()
{
    QSettings settings;

    if (!settings.value("network/allowLocalServer").isValid())
        settings.setValue("network/allowLocalServer", "false");

    return settings.value("network/allowLocalServer").toBool();
}

bool EtUtils::setAllowLocal(bool allowLocalServer)
{
    if (allowLocalServer == this->serverAllowLocal())
        return false;

    QSettings settings;
    settings.setValue("network/allowLocalServer", allowLocalServer);
    return true;
}

QByteArray EtUtils::serialize(QList<QByteArray> list)
{
    for (auto &&value : list)
    {
        if (value.indexOf("'") != -1)
            value.replace("'", "\\'");
    }

    QByteArray serialized = "'" + list.join("','") + "'";
    return serialized;
}

QString EtUtils::serializeStr(QStringList list)
{
    for (auto &&value : list)
    {
        if (value.indexOf("'") != -1)
            value.replace("'", "\\'");
    }

    QString serialized = "'" + list.join("','") + "'";
    return serialized;
}

QList<QByteArray> EtUtils::deserialize(const QString &serialized)
{
    QStringList deserializedStr = serialized.mid(1, serialized.length() - 2).split("','");
    QList<QByteArray> deserialized;

    for (auto &&value : deserializedStr)
    {
        if (value.indexOf("'") != -1)
            value.replace("\\'", "'");
        deserialized << value.toUtf8();
    }

    return deserialized;
}

QStringList EtUtils::deserializeStr(const QString &serialized)
{
    QStringList deserialized = serialized.mid(1, serialized.length() - 2).split("','");

    for (auto &&value : deserialized)
    {
        if (value.indexOf("'") != -1)
            value.replace("'", "\\'");
    }

    return deserialized;
}

bool EtUtils::autoWipe(int wipe)
{
    QSettings settings;
    QString num = QString::number(wipe);

    if (!settings.value("wipes/" + num).isValid())
        settings.setValue("wipes/" + num, false);

    if (!settings.value("wipes/" + num).toBool())
    {
        qDebug() << "Wipe all";
        Utils::wipeDataFiles();
        settings.setValue("wipes/" + num, true);
        return true;
    }

    return false;
}

bool EtUtils::networkActive() const
{
    return m_networkActive;
}

bool EtUtils::isRelease() const
{
    return m_isRelease;
}

QString EtUtils::clipboardText()
{
    QString text = clipboard->text();
    text.replace("￼", "");
    text.replace("<", "&lt;");
    text.replace(">", "&gt;");
    return text;
}

QString EtUtils::getPureHtml(QString newText)
{
    int toDel = newText.indexOf("<p");
    newText = newText.remove(0, toDel);
    toDel = newText.indexOf(">") + 1;
    newText = newText.remove(0, toDel);
    int end = QString("</p></body></html>").length();
    // if (newText.right(end) == "</p></body></html>")
    newText.chop(end);
    newText.replace("<!--StartFragment-->", "");
    newText.replace("<!--EndFragment-->", "");
    if (newText == "<br />")
        newText = "";
    return newText;
}

QVariantList EtUtils::fieldNames(const QString &field)
{
    const QMap<int, QString> *original;

    if (field == "unit")
        original = &Profile::unitMap;
    else if (field == "category")
        original = &Profile::categoryMap;
    else if (field == "body")
        original = &Profile::bodyMap;
    else if (field == "hair")
        original = &Profile::hairMap;
    else if (field == "hairLength")
        original = &Profile::hairLengthMap;
    else if (field == "eye")
        original = &Profile::eyeMap;
    else if (field == "ethnicity")
        original = &Profile::ethnicityMap;
    else if (field == "style")
        original = &Profile::styleMap;
    else if (field == "sports")
        original = &Profile::sportsMap;
    else if (field == "skin")
        original = &Profile::skinMap;
    else if (field == "direction")
        original = &Profile::directionMap;
    else if (field == "workStyle")
        original = &Profile::workStyleMap;
    else if (field == "fashion")
        original = &Profile::fashionMap;
    else if (field == "scope")
        original = &Profile::scopeMap;
    else
        return QVariantList();

    QVariantList list;

    for (auto &el : original->keys())
    {
        QVariantMap map = { { "index", el }, { "name", original->value(el) } };
        list << map;
    }

    return list;
}

void EtUtils::setNetworkActive(bool networkActive)
{
    if (m_networkActive == networkActive)
        return;

    m_networkActive = networkActive;
    emit networkActiveChanged(m_networkActive);
}

void EtUtils::setIsRelease(bool isRelease)
{
    if (m_isRelease == isRelease)
        return;

    m_isRelease = isRelease;
    emit isReleaseChanged(m_isRelease);
}

void EtUtils::copyDir(const QString &src, const QString &dst)
{
    const QDir dir(src);

    if (!dir.exists() || !QDir().mkdir(dst))
    {
        if (!QDir(dst).exists())
            return;
    }

    for (QString &d : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        copyDir(src + QDir::separator() + d, dst + QDir::separator() + d);

    for (QString &f : dir.entryList(QDir::Files))
    {
        const QString oneFileName = src + QDir::separator() + f;
        const QString twoFileName = dst + QDir::separator() + f;
        const QFileInfo oneFile(oneFileName);
        const QFileInfo twoFile(twoFileName);

        if (twoFile.exists() && oneFile.size() == twoFile.size())
            continue;
        else
            QFile::remove(twoFileName);

        QFile::copy(oneFileName, twoFileName);
    }
}

uint EtUtils::filesCountInDir(const QString &path)
{
    QDir dir(path);
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    return dir.count();
}

Profile EtUtils::makeProfile()
{
    return Profile();
}

SearchFilters EtUtils::makeFilters()
{
    return SearchFilters();
}

int EtUtils::privateProfileExist()
{
    return QDir("keystore/profile").entryList(QDir::Files).length();
}

QString EtUtils::country(int id)
{
    if (id == QLocale::UnitedStates)
        return "USA";

    QString country = QLocale::countryToString(QLocale::Country(id));

    static const QList<int> andish { QLocale::AntiguaAndBarbuda,
                                     QLocale::BosniaAndHerzegowina,
                                     QLocale::CeutaAndMelilla,
                                     QLocale::HeardAndMcDonaldIslands,
                                     QLocale::SaintKittsAndNevis,
                                     QLocale::SaintPierreAndMiquelon,
                                     QLocale::SaintVincentAndTheGrenadines,
                                     QLocale::SaoTomeAndPrincipe,
                                     QLocale::SouthGeorgiaAndTheSouthSandwichIslands,
                                     QLocale::SvalbardAndJanMayenIslands,
                                     QLocale::TrinidadAndTobago,
                                     QLocale::TurksAndCaicosIslands,
                                     QLocale::WallisAndFutunaIslands };

    if (andish.contains(id))
        country.replace("And", "&");

    return country;
}

QVariantList EtUtils::findCountries(QString find)
{
    static const QList<int> ignored { QLocale::World,
                                      QLocale::Europe,
                                      QLocale::EuropeanUnion,
                                      QLocale::LatinAmerica,
                                      QLocale::BritishIndianOceanTerritory,
                                      QLocale::FrenchGuiana,
                                      QLocale::OutlyingOceania,
                                      QLocale::PalestinianTerritories,
                                      QLocale::WesternSahara };

    find = find.toLower();
    QVariantList list;

    for (int i = 1; i < QLocale::LastCountry; i++)
    {
        if (ignored.contains(i))
            continue;

        const QString country = this->country(i).toLower();
        if (country.contains(find) || (find.contains("and") && country.contains(find.replace("and", "&"))))
            list << i;
    }

    return list;
}

void EtUtils::copyToClipboard(const QString &str)
{
    clipboard->setText(str, QClipboard::Clipboard);
    clipboard->setText(str, QClipboard::Selection);
}

void EtUtils::makePath(const QString &str)
{
    QDir().mkpath(str);
}

int EtUtils::freeMemory()
{
    // qint64 total = Utils::checkMemoryTotal() / 1024 / 1024;
    int free = Utils::checkMemoryFree() / 1024 / 1024;
    // int available = total - free;
    return free;
}

bool EtUtils::isSqlite(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray read = file.read(13);

    return read == "SQLite format";
}

bool EtUtils::isGif(QString fileUrl)
{
    if (fileUrl.indexOf(".gif", Qt::CaseInsensitive) != -1)
        return true;

    QString filePath = QUrl(fileUrl).toLocalFile();
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray read = file.read(3);

    return read == "GIF";
}

QVariantList EtUtils::tableNames(QString dbPath)
{
    if (dbPath.isEmpty() || !QFile::exists(dbPath))
        return {};
    DBConnector db;
    db.open(dbPath.toStdString());
    auto tables = db.tableNames();

    QVariantList list;
    for (std::string table : tables)
    {
        list << QString::fromStdString(table);
    }

    return list;
}

QVariantList EtUtils::tableData(QString dbPath, QString table)
{
    if (dbPath.isEmpty() || table.isEmpty() || !QFile::exists(dbPath))
        return {};
    DBConnector db(dbPath.toStdString());
    auto columnsRes = db.select("PRAGMA table_info(" + table.toStdString() + ");");
    QStringList columns;
    for (auto row : columnsRes)
        columns << row["name"].c_str();

    auto data = db.select("SELECT * FROM " + table.toStdString());
    db.close();

    QVariantList all;

    if (data.empty())
        return {};

    QVariantMap mapRow;
    int j = 1;
    for (QString column : columns)
    {
        mapRow[(j > 9 ? "cc" : "c") + QString::number(j)] = column;
        j++;
    }
    for (; j != 15; j++)
        mapRow[(j > 9 ? "cc" : "c") + QString::number(j)] = "";
    all << mapRow;

    for (DBRow row : data)
    {
        int i = 1;

        QVariantMap mapRow;
        for (QString column : columns)
        {
            mapRow[(i > 9 ? "cc" : "c") + QString::number(i)] = row[column.toStdString()].c_str();
            i++;
        }
        for (; i != 15; i++)
            mapRow[(i > 9 ? "cc" : "c") + QString::number(i)] = "";

        all << mapRow;
    }

    return all;
}

void EtUtils::openFolder(QString folder, QString program)
{
#ifndef Q_OS_IOS
    if (!program.isEmpty())
    {
        QProcess::startDetached(program, { QDir::currentPath() });
        return;
    }

#if defined(Q_OS_LINUX)
    QProcess::startDetached("xdg-open", { folder });
#elif defined(Q_OS_WIN)
    QProcess::startDetached("explorer.exe", { folder.replace("/", "\\") });
#elif defined(Q_OS_MAC)
    QProcess::startDetached("open", { folder });
#else
    qDebug() << "openFolder not implemented for this platform";
#endif
#endif
}
