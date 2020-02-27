#include "utils/utils.h"

template <typename T>

std::string to_string(T value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

QByteArray Utils::calcKeccak(const QByteArray &b)
{
    Keccak keccak;
    QByteArray hashmsg = keccak(b);
    return hashmsg;
}

// SERIALIZATION //

QByteArray Serialization::serialize(const QList<QByteArray> &list)
{
    return list.join(DEFAULT_FIELD_SPLITTER) + DEFAULT_FIELD_SPLITTER;
}

QByteArray Serialization::serialize(const QList<QByteArray> &list, const QByteArray &delimiter)
{
    QByteArray result;
    for (const QByteArray &v : list)
    {
        result.append(v).append(delimiter);
    }
    return result;
}

QByteArray Serialization::serialize(const QList<QByteArray> &list, char delimiter)
{

    return serialize(list, QByteArray(1, delimiter));
}

QList<QByteArray> Serialization::deserialize(const QByteArray data, const QByteArray &delim)
{
    if (data.isEmpty())
    {
        return QList<QByteArray>();
    }
    QList<QByteArray> resList;
    QByteArray currentData;
    bool temp = false;
    currentData.clear();
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i] == delim[0])
        {
            if ((data.size() - i) < delim.size())
            {
                resList.append(currentData);
                return resList;
            }
            for (int j = 0; j < delim.size(); j++)
                if (data[i + j] != delim[j])
                {
                    temp = true;
                    break;
                }
            if (!temp)
            {
                resList.append(currentData);
                i = i + delim.size() - 1;
                currentData.clear();
                continue;
            }
            else
            {
                currentData.append(data[i]);
                temp = false;
                continue;
            }
        }
        currentData.append(data[i]);
    }
    return resList;
}

QString Serialization::serializeString(const QStringList &list)
{
    return serializeString(list, DEFAULT_FIELD_SPLITTER);
}

QString Serialization::serializeString(const QStringList &list, const QByteArray &delimiter)
{
    return list.join(delimiter) + delimiter;
}

QStringList Serialization::deserializeString(const QString &serialized)
{
    QStringList list = serialized.split(DEFAULT_FIELD_SPLITTER);
    list.removeLast();
    return list;
}

std::vector<std::string> Utils::split(const std::string &s, char c)
{
    auto end = s.cend();
    auto start = end;

    std::vector<std::string> v;
    for (auto it = s.cbegin(); it != end; ++it)
    {
        if (*it != c)
        {
            if (start == end)
                start = it;
            continue;
        }
        if (start != end)
        {
            v.emplace_back(start, it);
            start = end;
        }
    }
    if (start != end)
        v.emplace_back(start, end);
    return v;
}

std::vector<std::string> Utils::split(const std::string &s)
{
    return split(s, Serialization::DEFAULT_FIELD_SPLITTER.at(0));
}

QString KeyStore::makeKeyFileName(QString name)
{
    return name + KEY_TYPE;
}

void FileSystem::createFolderIfNotExist(QString path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        dir = QDir();
        dir.mkpath(path);
    }
}

int Utils::compare(const QByteArray &one, const QByteArray &two)
{
    if (one.size() > two.size())
    {
        return one.size() - two.size();
    }
    else if (one.size() == two.size())
    {
        return static_cast<int>(one == two);
    }
    else
        return two.size() - one.size();
}

QByteArray Utils::getParam(const QString &param, const QByteArray &jsonDocument)
{
    QJsonDocument doc = QJsonDocument::fromBinaryData(jsonDocument);
    return doc.object().value(param).toString().toLocal8Bit();
}

bool FileSystem::tryToOpen(QFile &file, QIODevice::OpenMode mode)
{
    if (!file.open(mode))
    {
        qCritical().noquote() << QString("[WARNING] Can't open [%1] file").arg(file.fileName());
        return false;
    }
    return true;
}
QList<QString> Serialization::deserialize(const QString &serialized, char delimiter)
{
    QStringList list = serialized.split(delimiter);
    //  list.removeLast();
    QList<QString> result;
    for (const QString &v : list)
    {
        result.append(v);
    }
    return result;
}

QByteArray Serialization::serializeStored(const QList<QByteArray> list)
{
    if (list.size() != 2)
    {
        qDebug() << "error in serializeStored, list have more or less then two elements";
        return "Error";
    }
    QByteArray res = list.at(0);
    res.append(DFS_HEADER_END_DELIMETR);
    res.append(list.at(1));
    return res;
}

QList<QByteArray> Serialization::desirializeStored(const QByteArray &serialize)
{
    QList<QByteArray> list = {};
    int index = serialize.indexOf(DFS_HEADER_END_DELIMETR);
    QByteArray line = serialize.mid(0, index);
    //    list << serialize.mid(0, 19);
    //    list << serialize.mid(19, 6);
    //    list << serialize.mid(25, 8);
    //    list << serialize.mid(33, 64);
    //    list << serialize.mid(97, 64);
    //    list << serialize.mid(161, 6);
    //    list << serialize.mid(167, 6);
    //    list << serialize.mid(173, 19);
    list << line << serialize.mid(index + 1, serialize.size() - index);
    return list;
}
QByteArray storedSpace::toByteArray(storedSpace::State state)
{
    if (state == storedSpace::State::NEWSTATE)
        return "NEWSTATE";
    if (state == storedSpace::State::CHANGEDS)
        return "CHANGEDS";
    if (state == storedSpace::State::DELSTATE)
        return "DELSTATE";
    return "UNRECOGS";
}

QString storedSpace::toString(storedSpace::State state)
{
    if (state == storedSpace::State::NEWSTATE)
        return "NEWSTATE";
    if (state == storedSpace::State::CHANGEDS)
        return "CHANGEDS";
    if (state == storedSpace::State::DELSTATE)
        return "DELSTATE";
    return "UNRECOGS";
}

storedSpace::State storedSpace::convertToDFSstate(QByteArray state)
{
    if (state == "NEWSTATE")
        return storedSpace::State::NEWSTATE;
    if (state == "CHANGEDS")
        return storedSpace::State::CHANGEDS;
    if (state == "DELSTATE")
        return storedSpace::State::DELSTATE;
    return storedSpace::State::UNRECOGS;
}

QByteArray Utils::intToByteArray(const int &number, const int &size)
{
    QByteArray num = QByteArray::number(number);
    QByteArray res = "";
    if (num.size() < size)
        for (int i = 0; i < size - num.size(); i++)
            res += "0";
    res += num;
    return res;
}

int Utils::qByteArrayToInt(const QByteArray &number)
{
    QByteArray num = "";
    int i = 0;
    //    bool flag = false;
    while (i < number.size())
    {
        if (number[i] == '0')
            i++;
        else
            break;
    }
    while (i < number.size())
    {
        num += number[i];
        i++;
    }
    int res = num.toInt();
    return res;
}

QByteArray Utils::calcKeccakForFile(const QString &path)
{
    QFile file(path);
    if (!file.exists())
    {
        qDebug() << "Utils::File not exist" << path;
        return "";
    }
    file.open(QIODevice::ReadOnly);
    long long _file_size = file.size();
    long long _data_offset = DataStorage::DATA_OFFSET;
    long long pos = 0;
    QList<QByteArray> hashList = {};
    while ((pos += _data_offset) < _file_size)
    {
        char *ch = new char[_data_offset];
        file.read(ch, _data_offset);
        hashList.append(Utils::calcKeccak(QByteArray(ch, _data_offset)));
        delete[] ch;
    }
    hashList.append(Utils::calcKeccak(file.read(_file_size - pos)));
    QByteArray hash;
    for (QByteArray &el : hashList)
        hash += el;
    file.close();
    return Utils::calcKeccak(hash);
}

QByteArray Serialization::universalSerialize(const QList<QByteArray> &list, const int &fiels_size)
{
    QByteArray serialized = "";
    for (const QByteArray &param : list)
    {
        serialized += Utils::intToByteArray(param.size(), fiels_size);
        serialized += param;
    }
    return serialized;
}

QList<QByteArray> Serialization::universalDeserialize(const QByteArray &serialized, const int &fiels_size)
{
    if (serialized.isEmpty() || serialized.length() <= fiels_size)
    {
        return {};
    }

    QList<QByteArray> list = {};
    int pos = 0;
    while (pos < serialized.size())
    {
        bool ok = true;
        int count = serialized.mid(pos, fiels_size)
                        .toInt(&ok); // Utils::qByteArrayToInt(serialized.mid(pos, fiels_size));
        if (!ok)
            return list;
        pos += fiels_size;
        QByteArray el = serialized.mid(pos, count);
        pos += count;
        if (el.isEmpty())
            list.append(el);
        else
            list << el;
    }
    //    serialized.remove(0, pos);
    return list;
}

void Utils::wipeDataFiles()
{
    QDir("blockchain").removeRecursively();
    QDir("data").removeRecursively();
    QDir("keystore").removeRecursively();
    QDir("tmp").removeRecursively();
    QFile("user.private").remove();
    QFile("user.private.login").remove();
    QFile(".fileList").remove();
    QFile(".settings").remove();
    QFile("network_cache").remove();

    /*
#ifdef EXTRACOIN_CONSOLE
    auto clearDir = [](const QString &dir, const QString &ignoredFile = "0") {
        QDir dirToClear(dir);
        auto filesList = dirToClear.entryInfoList(QDir::Files);

        for (auto &file : filesList)
        {
            if (file.fileName() != ignoredFile)
                QFile::remove(file.filePath());
        }
    };

    QByteArray companySection = TMP::companyActorId->right(2);
    QDir actorDir("blockchain/index/actors");
    auto dirsList = actorDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (auto &dir : dirsList)
    {
        if (dir.fileName() != companySection)
            QDir(dir.filePath()).removeRecursively();
    }

    clearDir("blockchain/index/actors/" + companySection, *TMP::companyActorId);
    clearDir("blockchain/index/blocks/0", "0");
    clearDir("keystore/personal", *TMP::companyActorId + ".key");
    clearDir("keystore/profile", *TMP::companyActorId + ".private");
    QDir("tmp").removeRecursively();
    QDir("data").removeRecursively();
    QFile(".fileList").remove();
    QFile("blockchain/index/actors/.first").remove();
    QFile("blockchain/index/actors/.last").remove();
    QFile("blockchain/index/blocks/.first").remove();
    QFile("blockchain/index/blocks/.last").remove();
#else
    QDir("blockchain").removeRecursively();
    QDir("data").removeRecursively();
    QDir("keystore").removeRecursively();
    QDir("tmp").removeRecursively();
    QFile("user.private").remove();
    QFile("user.private.login").remove();
    QFile(".fileList").remove();
#endif
    QFile(".extracoin.lock").remove();
    QFile(".settings").remove();
    */
}

void Utils::softWipe(const QString &currentId)
{
    auto clearDir = [](const QString &dir, const QString &ignoredFile = "0", bool isFile = true) {
        QDir dirToClear(dir);
        auto filesList = dirToClear.entryInfoList(isFile ? QDir::Files : QDir::Dirs | QDir::NoDotAndDotDot);

        for (auto &file : filesList)
        {
            qDebug() << file.fileName() << file.filePath();
            if (file.fileName() != ignoredFile)
                isFile ? QFile::remove(file.filePath()) : QDir(file.filePath()).removeRecursively();
        }
    };

    QDir("blockchain/index/blocks").removeRecursively();
    clearDir("blockchain/index/actors", currentId.right(2), false);
    clearDir("blockchain/index/actors/" + currentId.right(2), currentId, true);
    clearDir("data", "", true);
    clearDir("data/" + currentId, "profile", false);
    QFile::remove("data/" + currentId + "/root");
    QFile::remove("data/" + currentId + "/root.tmp");
    QDir("tmp").removeRecursively();
    QFile("network_cache").remove();
    QFile(".settings").remove();
    QFile(".fileList").remove();
}

FileList::FileList()
{
    fileList.setFileName(".fileList");
    if (fileList.exists())
    {
        init();
    }
    else
    {
        //
    }
}

void FileList::init()
{
    fileList.open(QIODevice::ReadOnly);
    qDebug() << "fileList INIT";
    int count = 0, currentPosition = 0;
    while (fileList.size() != currentPosition)
    {
        fileList.seek(currentPosition);
        count = Utils::qByteArrayToInt(fileList.read(4));

        if (fileList.read(FIELD_SIZE) == DATA_EMPTY)
        {
            indexList.append(indexRow(std::to_string(count), currentPosition, 0));
        }
        else
        {
            indexList.append(indexRow(fileList.read(64).toStdString(), currentPosition, 1));
        }

        currentPosition += count + FIELD_SIZE;
    }
    fileList.flush();
    fileList.close();
    checkForDelete();
    if (indexList.size() == 0)
        qDebug() << "indexList -> empty";
    else
        qDebug() << "indexList size->" << indexList.size();
}

void FileList::checkForDelete()
{
    qDebug() << "check delete fileList" << indexList.size();
    int count = 0;
    for (QList<indexRow>::iterator it = indexList.end() - 1; it != indexList.begin() - 1; it--)
    {
        if (it->used == 0)
        {
            fileList.open(QIODevice::ReadWrite);
            fileList.seek(it->currentPosition);
            fileList.resize(fileList.size() - (Utils::qByteArrayToInt(fileList.read(4)) + FIELD_SIZE));
            indexList.removeLast();
            fileList.flush();
            fileList.close();
            ++count;
            if (indexList.size() == 0)
                break;
        }
        else
        {
            qDebug() << "deleted in fileList " << count;
            return;
        }
    }
    qDebug() << "after delete" << indexList.size() << "; deleted " << count;
    return;
}

bool FileList::check(QByteArray hash)
{
    for (QList<indexRow>::iterator it = indexList.begin(); it != indexList.end(); it++)
    {
        if (it->hash == hash.toStdString())
            return true;
    }
    return false;
}

void FileList::add(QByteArray hash, QByteArray data)
{
    if (check(hash) /*&& find(hash)->used == 1*/)
    {
        qDebug() << "file with this hash are have and use in FileList";
        return;
    }
    fileList.open(QIODevice::ReadWrite);
    qDebug() << "add to fileList->" << hash;
    if (indexList.size() == 0)
    {
        QByteArray serialize1 = Serialization::universalSerialize({ hash, data }, FIELD_SIZE);
        QByteArray serialize2 = Serialization::universalSerialize({ serialize1 }, FIELD_SIZE);
        indexList.append(indexRow(serialize2.mid(FIELD_SIZE * 2, 64).toStdString(), 0, 1));
        fileList.write(serialize2);
    }
    else
    {
        QList<indexRow>::iterator it = find(data.mid(0, FIELD_SIZE));
        if (it != indexList.end() && !it->used)
        {
            fileList.seek(find(data.mid(0, FIELD_SIZE))->currentPosition);
            QByteArray serialize1 = Serialization::universalSerialize({ hash, data }, FIELD_SIZE);
            QByteArray serialize2 = Serialization::universalSerialize({ serialize1 }, FIELD_SIZE);
            fileList.write(serialize2);
            find(serialize2.mid(0, FIELD_SIZE))->used = 1;
            find(serialize2.mid(0, FIELD_SIZE))->hash = data.mid(FIELD_SIZE * 2, 64).toStdString();
        }
        else
        {
            fileList.seek(fileList.size());
            QByteArray serialize1 = Serialization::universalSerialize({ hash, data }, FIELD_SIZE);
            QByteArray serialize2 = Serialization::universalSerialize({ serialize1 }, FIELD_SIZE);
            indexList.append(indexRow(serialize2.mid(FIELD_SIZE * 2, 64).toStdString(), fileList.pos(), 1));
            fileList.write(serialize2);
        }
    }
    fileList.flush();
    fileList.close();
}

void FileList::remove(QByteArray hash)
{
    if (!check(hash))
    {
        qDebug() << "hash or element not exist";
        return;
    }

    fileList.open(QIODevice::ReadWrite);
    fileList.seek(find(hash)->currentPosition + FIELD_SIZE);
    fileList.write(DATA_EMPTY);
    fileList.seek(find(hash)->currentPosition);
    find(hash)->used = 0;
    find(hash)->hash = QByteArray::number(Utils::qByteArrayToInt(fileList.read(4))).toStdString();
    fileList.flush();
    fileList.close();
    checkForDelete();
}

QList<indexRow>::iterator FileList::find(QByteArray key)
{
    for (QList<indexRow>::iterator it = indexList.begin(); it != indexList.end(); it++)
    {
        if (it->hash == key.toStdString())
            return it;
    }
    return indexList.end();
}

QByteArray FileList::operator[](int value)
{
    return FileList::at(value);
}

QByteArray FileList::at(QByteArray hash)
{
    QByteArray data = "";
    if (check(hash))
    {
        fileList.open(QIODevice::ReadOnly);
        fileList.seek(find(hash)->currentPosition);
        int quantity = Utils::qByteArrayToInt(fileList.read(FIELD_SIZE));
        int hashSize = Utils::qByteArrayToInt(fileList.read(FIELD_SIZE)) + 3 * FIELD_SIZE;
        fileList.seek(find(hash)->currentPosition + hashSize);
        data = fileList.read(quantity - hashSize + FIELD_SIZE);
        fileList.flush();
        fileList.close();
    }
    else
        qDebug() << "File is not exist in list-> return empty data";
    return data;
}

QByteArray FileList::at(int value)
{
    if (indexList.size() < value)
        qDebug() << "never at";
    else
    {
        return this->at(QByteArray::fromStdString(indexList[value].hash));
    }
    return "";
}

int FileList::getIndexSize()
{
    return indexList.size();
}

QByteArray FileList::getHash(int value)
{
    QByteArray data = "";
    if (indexList.size() < value || indexList.size() == 0)
        qDebug() << "never at";
    else
    {
        return QByteArray::fromStdString(indexList[value].hash);
    }

    return data;
}

void FileList::setFileList(const QFile &value)
{
    fileList.setFileName(value.fileName());
}

indexRow::indexRow(std::string _hash, long long pos, short use)
{
    hash = _hash;
    currentPosition = pos;
    used = use;
}
FileList::~FileList()
{
    this->checkForDelete();
}

qint64 Utils::checkMemoryFree()
{
    QStorageInfo x(qApp->applicationDirPath());
    qDebug() << "Free memory" << x.bytesFree() / 1024 / 1024 << "MB";
    return x.bytesFree();
}

qint64 Utils::checkMemoryTotal()
{
    QStorageInfo x(qApp->applicationDirPath());
    qDebug() << "Total memory" << x.bytesTotal() / 1024 / 1024 << "MB";
    return x.bytesTotal();
}
