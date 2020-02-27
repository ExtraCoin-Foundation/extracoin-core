#include "ui/model/contactsmodel.h"

#include "ui/ui_controller.h"

ContactsModel::ContactsModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "profile", "name" });

    connect(&timer, &QTimer::timeout, [this]() {
        for (const auto &profile : todoProfiles)
        {
            addProfile(profile.userId(), profile);
        }
    });
    timer.start(5000);
}

void ContactsModel::setActorIndex(ActorIndex *actorIndex)
{
    this->actorIndex = actorIndex;
}

void ContactsModel::setAccController(AccountController *value)
{
    accController = value;
}

void ContactsModel::addProfile(QString actorId, Profile profile)
{
    if (!profile.isServiceExists())
    {
        if (todoProfiles.indexOf(profile) == -1)
            todoProfiles << profile;
        return;
    }
    else
    {
        todoProfiles.removeOne(profile);
    }

    QString userId = profile.userId();

    if (accController->getMainActor() != nullptr
        && (userId == accController->getMainActor()->getId().toActorId()
            || profile.at(0) == "6")) // TODO: use main actor
        return;

    if (!profiles.contains(userId))
    {
        QVariantMap profileMap;

        profileMap["userId"] = profile.userId();
        profileMap["firstName"] = profile.firstName();
        profileMap["lastName"] = profile.lastName();
        profileMap["avatar"] = profile.avatar();
        profileMap["miniAvatar"] = profile.miniAvatar();

        append(QVariantMap{ { "profile", profileMap },
                            { "name", profile.firstName() + " " + profile.lastName() } });

        profiles << userId;
    }
    // else change avatar
}

void ContactsModel::localLoad()
{
    QList<Profile> profiles;
    QString folderPath = "data";
    QStringList sectionList = QDir(folderPath).entryList(QDir::QDir::Dirs | QDir::NoDot | QDir::NoDotDot);

    for (const QString &section : sectionList)
    {
        QString profileFolderPath = folderPath + "/" + section + "/profile/" + section + ".profile";
        if (QFile().exists(profileFolderPath))
        {
            Profile profile = actorIndex->getProfile(section);
            addProfile(profile.userId(), profile);
        }
    }
}

void ContactsModel::setUi(UiController *value)
{
    ui = value;
}
