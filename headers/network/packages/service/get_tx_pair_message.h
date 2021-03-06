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

#ifndef GET_TX_PAIR_MESSAGE_H
#define GET_TX_PAIR_MESSAGE_H

#include "network/packages/base_message.h"

namespace Messages {

struct GetTxPairMessage : IMessage
{
    static const short FIELD_SIZE = 2;
    static const short FIELDS_COUNT = 2;

    BigNumber senderId;
    BigNumber receiverId;

    // IMessage interface
public:
    void operator=(QByteArray &serialized) override;
    short getFieldsCount() const override;
    QByteArray serialize() const override;
    void deserialize(const QByteArray &serilaized) override;
};
}

#endif // GET_TX_PAIR_MESSAGE_H
