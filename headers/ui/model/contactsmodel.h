#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include "ui/model/abstractmodel.h"
#include "datastorage/index/actorindex.h"
#include "profile/profile.h"

#include <QTimer>

class UiController;

class ContactsModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit ContactsModel(AbstractModel *parent = nullptr);
    void setActorIndex(ActorIndex *actorIndex);
    void setUi(UiController *value);
    void setAccController(AccountController *value);

    Q_INVOKABLE void localLoad();

public slots:
    void addProfile(QString actorId, Profile profile);

private:
    ActorIndex *actorIndex;
    UiController *ui;
    AccountController *accController;

    QStringList profiles;

    QTimer timer;
    QList<Profile> todoProfiles;
};

#endif // CONTACTSMODEL_H
