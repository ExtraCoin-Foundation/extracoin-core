#ifndef AVAILABLEWALLETSMODEL_H
#define AVAILABLEWALLETSMODEL_H

#include "ui/model/abstractmodel.h"

#include <QDebug>

class AvailableWalletsModel : public AbstractModel
{
    Q_OBJECT
public:
    explicit AvailableWalletsModel(AbstractModel *parent = nullptr);
    Q_INVOKABLE void getWalletList();

signals:
    void getWalletListFromActors();

public:
    void setWalletList(QStringList *walList);
};

#endif // AVAILABLEWALLETSMODEL_H
