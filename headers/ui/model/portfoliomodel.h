#ifndef PORTFOLIOMODEL_H
#define PORTFOLIOMODEL_H

#include "ui/model/abstractmodel.h"
#include <QVector>
#include <QImageReader>
#include <QVariantMap>
#include <algorithm>
#include <ctime>
#include <QStringList>
#include <QDebug>
#include <QUrl>
#include <string>

class PortfolioModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit PortfolioModel(AbstractModel* parent = nullptr);

    Q_INVOKABLE void modelPortfolio(QStringList pictures, bool isFirst, bool clearModel);
    Q_INVOKABLE void shufflePortfolio(QStringList& pic);
    Q_INVOKABLE void moveImages(int from, int to, int n);

signals:
    void save(QString fileName);

private:
    QStringList pic;
    void skip(int count);
};

#endif // PORTFOLIOMODEL_H
