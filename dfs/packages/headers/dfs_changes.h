/*
 * ExtraChain Core
 * Copyright (C) 2020 ExtraChain Foundation <extrachain@gmail.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef DFS_CHANGES_H
#define DFS_CHANGES_H

#include "dfs/packages/headers/dfs_message_interface.h"
#include "headers/network/packages/message_interface.h"
#include <QFile>

namespace DistFileSystem {
struct DfsChanges : Messages::ISmallMessage
{
    const short FIELDS_COUNT = 9;

    QString filePath;

    QList<QByteArray> data;
    QByteArray range;
    int changeType = -1;
    QByteArray userId;
    QByteArray sign;
    QByteArray messHash;
    QByteArray prevHash;
    int fileVersion = -1;

    const QList<QByteArray> serializedParams() const;
    void operator=(QList<QByteArray> &list);
    // ISmallMessage interface
public:
    void operator=(QByteArray &serialized) override;
    bool isEmpty() const override;
    short getFieldsCount() const override;
    QByteArray serialize() const override;
    void deserialize(const QByteArray &serialized) override;
    QByteArray prepareSign();
};
}

#endif // DFS_CHANGES_H
