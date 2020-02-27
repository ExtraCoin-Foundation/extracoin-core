#ifndef SQLTABLEMODEL_H
#define SQLTABLEMODEL_H

#include <QAbstractTableModel>
#include <QSize>

class SqlTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SqlTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int /* section */, Qt::Orientation /* orientation */, int role) const override;

    Q_INVOKABLE void loadData(QString dbPath);

private:
    QVector<QVector<QString>> table;
};

#endif // SQLTABLEMODEL_H
