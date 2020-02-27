#include "ui/model/portfoliomodel.h"
#include <random>
#include <string>

PortfolioModel::PortfolioModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "file", "vertical", "horizontal" });
}

void PortfolioModel::modelPortfolio(QStringList pictures, bool isFirst, bool clearModel)
{
    pic.clear();
    qDebug() << "modelPortfolio" << pictures << isFirst << count();
    QStringList temp;

    for (int i = 0; i < pictures.size(); i++)
    {
        temp.append(QUrl(pictures[i]).toLocalFile());
    }

    double w = 0, h = 0;
    int x = 0;
    bool left = true;
    QVariantMap pics;
    if (clearModel)
        clear();
    qDebug() << "modelPortfolio count" << count() << temp.size();
    // temp += pic;
    for (int i = 0; i < temp.size(); i++)
    {
        QImageReader reader(temp[i]);
        QSize size = reader.size();
        w = size.width();
        h = size.height();
        pics["file"] = temp[i];
        qDebug() << "1";
        if (!reader.error())
        {
            qDebug() << "2";
            if (w / h > 1.5 && w / h < 1.65 && x == 0 && left == true)
            {
                pics["vertical"] = 2;
                pics["horizontal"] = 2;
                append(pics);
                if (isFirst)
                    save(pics["file"].toString());
                skip(3);
            }
            else if (w / h > 1.65 && left == true)
            {
                pics["vertical"] = 1;
                pics["horizontal"] = 2;
                append(pics);
                if (isFirst)
                    save(pics["file"].toString());
                else
                    qDebug() << "Can you see my popo" << pics["file"].toString();
                skip(1);
            }
            else if (w / h < 0.83)
            {
                pics["vertical"] = 2;
                pics["horizontal"] = 1;
                ++x;
                append(pics);
                if (isFirst)
                    save(pics["file"].toString());
                else
                    qDebug() << "Can you see" << pics["file"].toString();
                if (i == temp.size() - 1)
                {
                    skip(2);
                }
                left = !left;
            }
            else
            {
                pics["vertical"] = 1;
                pics["horizontal"] = 1;

                append(pics);
                if (isFirst)
                    save(pics["file"].toString());
                else
                    qDebug() << "Can you see my popo" << pics["file"].toString();
                if (x == 1)
                {
                    skip(1);
                    x = 0;
                    left = !left;
                }
                left = !left;
            }
            if (x == 2 && i != temp.size() - 1)
            {
                skip(2);
                x = 0;
            }
        }
    }
    // pic = temp;
}

void PortfolioModel::shufflePortfolio(QStringList &pic)
{
    // std::srand(unsigned(std::time(nullptr)));
    // std::random_shuffle(pic.begin(), pic.end());
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(pic.begin(), pic.end(), g);
}

void PortfolioModel::moveImages(int from, int to, int n)
{
    //    auto temp = get(to);
    move(from, to, n);
}

void PortfolioModel::skip(int count)
{
    for (int i = 0; i != count; ++i)
    {
        append(QVariantMap({ { "file", "" }, { "vertical", 1 }, { "horizontal", 1 } }));
    }
}
