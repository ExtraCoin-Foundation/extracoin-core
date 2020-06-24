#include "datastorage/genesis_block.h"

GenesisBlock::GenesisBlock()
    : Block()
{
    this->type = Config::GENESIS_BLOCK_TYPE;
}

GenesisBlock::GenesisBlock(const GenesisBlock &block)
    : Block(block)
{
    this->prevGenHash = block.getPrevGenHash();
}

GenesisBlock::GenesisBlock(const QByteArray &serialized)
{
    deserialize(serialized);
}

GenesisBlock::GenesisBlock(const QByteArray &_data, const Block &prevBlock, const QByteArray &prevGenHash)
    : Block(_data, prevBlock)
    , prevGenHash(prevGenHash)
{
    this->type = Config::GENESIS_BLOCK_TYPE;
}

void GenesisBlock::addRow(const GenesisDataRow &row)
{
    this->data += Serialization::serialize({ row.serialize() }, Serialization::DEFAULT_FIELD_SIZE);
}

QByteArray GenesisBlock::getDataForDigSig() const
{
    return Block::getDataForDigSig();
}

QByteArray GenesisBlock::getDataForHash() const
{
    return Block::getDataForHash();
}

bool GenesisBlock::deserialize(const QByteArray &serialized)
{
    QList<QByteArray> l = Serialization::deserialize(serialized, FIELDS_SIZE);
    if (l.length() == 8)
    {
        initFields(l);
        return true;
    }
    return false;
}

QByteArray GenesisBlock::serialize() const
{
    QList<QByteArray> list;
    list << getType() << getIndex().toByteArray() << QByteArray::number(getDate()) << getData()
         << getPrevHash() << getHash() << getPrevGenHash() << getSignatures();
    return Serialization::serialize(list, FIELDS_SIZE);
}

void GenesisBlock::initFields(QList<QByteArray> &list)
{
    type = list.takeFirst();
    index = BigNumber(list.takeFirst());
    date = list.takeFirst().toLongLong();
    data = list.takeFirst();
    prevHash = list.takeFirst();
    hash = list.takeFirst();
    prevGenHash = list.takeFirst();
    QByteArray signs = list.takeFirst();
    QByteArrayList lists = Serialization::deserialize(signs, FIELDS_SIZE);
    for (const auto &tmp : lists)
    {
        QByteArrayList tmps = Serialization::deserialize(tmp, FIELDS_SIZE);
        if (tmps.length() == 3)
            signatures.append({ tmps.at(0), tmps.at(1), bool(tmps.at(2).toInt()) });
    }
}

QList<GenesisDataRow> GenesisBlock::extractDataRows() const
{
    QList<QByteArray> txsData = Serialization::deserialize(data, Serialization::DEFAULT_FIELD_SIZE);
    QList<GenesisDataRow> genesisDataRows;
    for (const QByteArray &dataRow : txsData)
    {
        genesisDataRows.append(GenesisDataRow(dataRow));
    }
    return genesisDataRows;
}

bool GenesisBlock::isGenesisBlock(const QByteArray &serialized)
{
    return serialized.contains(Config::GENESIS_BLOCK_TYPE);
}

void GenesisBlock::setPrevGenHash(const QByteArray &value)
{
    prevGenHash = value;
}

QByteArray GenesisBlock::getPrevGenHash() const
{
    return prevGenHash;
}
