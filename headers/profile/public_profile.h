#ifndef PUBLIC_PROFILE_H
#define PUBLIC_PROFILE_H

#include "profile/profile.h"
#include "utils/utils.h"

struct indexList
{
    indexList(long long curPos, int _size);
    long long currentPosition;
    int size;
};

class PublicProfile
{
public:
    PublicProfile(QByteArrayList _profile, QByteArray _sign, QByteArray path, QByteArray _id);
    PublicProfile(const QByteArray &serialize);
    PublicProfile(QByteArray _id, QByteArray _path);
    PublicProfile();
    QByteArray serialize() const;
    void setProfile(QByteArrayList profile, QByteArray path);
    void saveProfileFromNet(QByteArray newProfile);
    QByteArrayList getListProfile();
    QByteArray getProfile();
    static QByteArray serialize(QByteArrayList profileList);
    static QByteArrayList deserialize(QByteArray serializeData);
    QByteArrayList getQuickProfile(QByteArray _data);
    QByteArray sign = "";
    QByteArray idPath;
    QByteArray id;

signals:
    //
private:
    static void saveTokenNames(QByteArray id, QByteArray nameToken, QByteArray color);
    //    QList<indexList> index;
};
#endif // PUBLIC_PROFILE_H