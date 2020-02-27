#include "ui/model/abstractmodel.h"

#include <QDebug>

AbstractModel::AbstractModel(QAbstractListModel *parent, const QList<QByteArray> &list)
    : QAbstractListModel(parent)
{
    setModelRoles(list);
}

int AbstractModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return datas.length();
}

int AbstractModel::count() const
{
    return m_count;
}

void AbstractModel::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(m_count);
}

QHash<int, QByteArray> AbstractModel::roleNames() const
{
    return roles;
}

QVariant AbstractModel::data(const QModelIndex &index, int role) const
{
    QVariantMap variants = datas[index.row()];
    return variants[roles[role]];
}

bool AbstractModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    set(index.row(), roles[role], value);

    return true;
}

void AbstractModel::prepend(const QVariantMap &variant)
{
    insert(0, variant);
}

void AbstractModel::append(const QVariantMap &variant)
{
    // qDebug() << "append" << variant;
    insert(datas.length(), variant);
}

void AbstractModel::insert(int i, const QVariantMap &variant)
{
    beginInsertRows(QModelIndex(), i, i);

    datas.insert(i, variant);

    endInsertRows();
    setCount(datas.length());
}

void AbstractModel::inserts(int i, const QVariantList &variants)
{
    beginInsertRows(QModelIndex(), i, i + variants.length() - 1);

    int tempI = i;
    for (auto &&variant : variants)
        datas.insert(tempI++, variant.toMap());

    setCount(datas.length());

    endInsertRows();
    dataChanged(index(i), index(i + variants.length() - 1));
}

void AbstractModel::remove(int index, int count = 0)
{
    beginRemoveRows(QModelIndex(), index, index + count - 1);
    while (count--)
        datas.removeAt(index);
    endRemoveRows();
    setCount(datas.count());
}

QVariantMap AbstractModel::get(int index)
{
    // qDebug() << "GET" << index << m_count - 1 << (index > m_count - 1);
    if (index > m_count - 1 || index < 0)
        return QVariantMap();
    return datas[index];
}

void AbstractModel::set(int indx, const QByteArray &role, const QVariant &value)
{
    auto &val = datas[indx];
    val[role] = value;
    emit dataChanged(index(indx, 0), index(indx, 0), QVector<int>() << roles.key(role));
}

void AbstractModel::move(int from, int to, int n)
{
    if (from >= m_count || from < 0 || to < 0 || to >= m_count
        || !beginMoveRows(QModelIndex(), from, from + n - 1, QModelIndex(), to > from ? to + 1 : to))
        return;
    if (n > 1 && from + n < to && to + n < m_count)
    {
        qDebug() << "n > 1";
        for (int i = 0; i < n; i++)
            datas.move(from + i, to + i);
    }
    else
        datas.move(from, to);
    endMoveRows();
}

QList<QByteArray> AbstractModel::getModelRoles() const
{
    return modelRoles;
}

void AbstractModel::setModelRoles(const QList<QByteArray> &value)
{
    modelRoles = value;

    int roleCount = Qt::UserRole;
    for (auto &&role : modelRoles)
        roles[++roleCount] = role;
}

void AbstractModel::appendFromJson(const QString &fileName)
{
    QFile file(fileName);

    if (!file.exists())
        return;

    if (file.open(QFile::ReadOnly))
    {
        QString json = file.readAll();
        auto doc = QJsonDocument::fromJson(json.toUtf8());
        auto var = doc.toVariant().toMap();
        var["alphabet"] = "";
        append(var);
    }

    file.close();
}

void AbstractModel::insertFromJson(int index, const QString &fileName)
{
    QFile file(fileName);

    if (!file.exists())
        return;

    if (file.open(QFile::ReadOnly))
    {
        QString json = file.readAll();
        auto doc = QJsonDocument::fromJson(json.toUtf8());
        auto var = doc.toVariant().toMap();
        var["alphabet"] = "";
        insert(index, var);
    }

    file.close();
}

QVariantMap AbstractModel::loadJson(const QString &fileName)
{
    QFile file(fileName);
    QVariantMap map;

    if (file.open(QFile::ReadOnly))
    {
        QString json = file.readAll();
        map = QJsonDocument::fromJson(json.toUtf8()).toVariant().toMap();
        file.close();
    }

    return map;
}

QList<QVariantMap> &AbstractModel::list()
{
    return datas;
}

void AbstractModel::clear()
{
    beginResetModel();
    datas.clear();
    setCount(0);
    endResetModel();
}
