#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include "ui/model/abstractmodel.h"
#include "datastorage/searchfilters.h"
#include "profile/profile.h"

class SearchModel : public AbstractModel
{
    Q_OBJECT
    Q_PROPERTY(SearchFilters filters READ getFilters WRITE setFilters)

public:
    explicit SearchModel(AbstractModel *parent = nullptr);

    Q_INVOKABLE void search();
    void loadProfile(const QString &fileName);

    SearchFilters getFilters() const;

public slots:
    void setFilters(SearchFilters filters);
    void fromActorIndex(QList<Profile>);

signals:
    void requestProfiles(SearchFilters filters);

private:
    QString currentId;
    QVariantMap filters;
    QString searchName;
    int type;

    QStringList loadedProfiles;

    const QStringList forFilter = { "bodyType",   "categoryModel", "ethnicity", "eyeType", "hairType",
                                    "hairLength", "scope",         "skin",      "sports",  "style" };
    const QStringList forProfile = { "body",       "category", "ethnicity", "eye",    "hair",
                                     "hairLength", "scope",    "skin",      "sports", "style" };
    SearchFilters m_filters;
};

#endif // SEARCHMODEL_H
