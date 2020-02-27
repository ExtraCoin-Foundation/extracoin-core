#include "ui/model/searchmodel.h"
#include "dfs/managers/headers/card_manager.h"
#include <QDebug>

SearchModel::SearchModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "profile" });
}

void SearchModel::search()
{
    clear();
    loadedProfiles.clear();
    qDebug().noquote() << "New filters:" << m_filters;

    // this->currentId = currentId;
    // this->filters = filters;
    // this->searchName = searchName;
    // this->type = type;

    QStringList profileFiles;

    for (const auto &profileFileName : profileFiles)
    {
        loadProfile(profileFileName);
    }
}

void SearchModel::loadProfile(const QString &fileName)
{
    QString userId = fileName.mid(5, fileName.length() - (fileName.length() - fileName.indexOf("/", 5)) - 5);

    if (loadedProfiles.contains(userId))
        return;
    else
        loadedProfiles << userId;

    QFile profileFile(fileName);
    profileFile.open(QFile::ReadOnly);
    QVariantMap profile = QJsonDocument::fromJson(profileFile.readAll()).toVariant().toMap();
    profile["userId"] = userId;

    if (profile["userId"].toString() == currentId)
        return;

    QVariantMap role = profile["roleData"].toMap();

    type = type == 2 ? 3 : type;

    // if (profile["type"].toInt() != type + 1)
    //    continue;

    QString firstName = profile["firstName"].toString().toLower();
    QString lastName = profile["lastName"].toString().toLower();
    if (!searchName.isEmpty())
    {
        if (!firstName.startsWith(searchName.toLower()) && !lastName.startsWith(searchName.toLower()))
        {
            return;
        }
    }
    // qDebug() << name + name2 << searchName << name.startsWith(searchName.toLower())
    //          << name2.startsWith(searchName.toLower());

    if (profile["type"].toInt() > -1) // 1!!!
    {
        append({ { "profile", profile } });
        return;
    }

    if (/*profile["id"].toString() == name || */ !profile["type"].toBool())
        return;

    if (filters["isMan"] == true && filters["isWoman"] == false && profile["type"] == 1)
        return;
    if (filters["isMan"] == false && filters["isWoman"] == true && profile["type"] == 0)
        return;

    QStringList date = profile["birthday"].toStringList();
    if (QDate::currentDate().year() - date[2].toInt() < filters["minAge"].toInt()
        || QDate::currentDate().year() - date[2].toInt() > filters["maxAge"].toInt())
        return;

    // if(profile["location"] != filters['location'])
    //     continue;

    QStringList sizes = role["sizes"].toStringList();
    if (sizes[0].toInt() > filters["heightMax"].toInt() || sizes[0].toInt() < filters["heightMin"].toInt())
        return;

    if (sizes[2].toInt() > filters["shoesMax"].toInt() || sizes[2].toInt() < filters["shoesMin"].toInt())
        return;

    if (sizes[4].toInt() > filters["waistMax"].toInt() || sizes[4].toInt() < filters["waistMin"].toInt())
        return;

    if (sizes[5].toInt() > filters["bustMax"].toInt() || sizes[5].toInt() < filters["bustMin"].toInt())
        return;

    if (sizes[6].toInt() > filters["hipsMax"].toInt() || sizes[6].toInt() < filters["hipsMin"].toInt())
        return;

    bool next = false;
    bool br = false;
    for (int i = 0; i < 10; i++)
    {
        if (filters[forFilter[i]].toStringList().length() < 1)
            continue;
        for (int j = 0; j < role[forProfile[i]].toStringList().length(); j++)
        {
            for (int ij = 0; ij < filters[forFilter[i]].toStringList().length(); ij++)
            {
                next = false;
                if (role[forProfile[i]].toStringList()[j].toLower()
                    == filters[forFilter[i]].toStringList()[ij].toLower())
                {
                    next = true;
                    break;
                }
            }
            if (next)
                break;
        }
        if (!next)
        {
            br = true;
            break;
        }
    }
    if (br)
        return;

    append({ { "profile", profile } });
}

SearchFilters SearchModel::getFilters() const
{
    return m_filters;
}

void SearchModel::setFilters(SearchFilters filters)
{
    // if (m_filters == filters) // TODO
    //     return;

    m_filters = filters;
    // qDebug().noquote() << "New filters:" << filters;
}

void SearchModel::fromActorIndex(QList<Profile> list)
{
    clear();

    for (auto &&profile : list)
    {
        if (!profile.isServiceExists())
        {
            continue;
        }

        QVariantMap profileMap;

        profileMap["userId"] = profile.userId();
        profileMap["firstName"] = profile.firstName();
        profileMap["lastName"] = profile.lastName();
        profileMap["avatar"] = profile.avatar();
        profileMap["miniAvatar"] = profile.miniAvatar();

        append(QVariantMap { { "profile", profileMap } });
    }
}
