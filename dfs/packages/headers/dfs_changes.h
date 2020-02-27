#ifndef DFS_CHANGES_H
#define DFS_CHANGES_H

#include "dfs/packages/headers/dfs_message_interface.h"
#include "headers/network/packages/message_interface.h"
#include <QFile>

namespace DistFileSystem {
struct DfsChanges : Messages::ISmallMessage
{
    const short FIELDS_COUNT = 7;

    QString filePath;

    QList<QByteArray> data;
    QByteArray range;
    int changeType = -1;
    QByteArray userId;
    QByteArray signature;
    QByteArray messHash;

    const QList<QByteArray> serializedParams() const;
    void operator=(QList<QByteArray> &list);
    // ISmallMessage interface
public:
    void operator=(QByteArray &serialized) override;
    bool isEmpty() const override;
    short getFieldsCount() const override;
    QByteArray serialize() const override;
    void deserialize(const QByteArray &serialized) override;
};
}

#endif // DFS_CHANGES_H
