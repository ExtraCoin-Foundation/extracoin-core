#ifndef GALLERYTHREAD_H
#define GALLERYTHREAD_H

#include <QDir>
#include <QVariantList>

class GalleryThread : public QObject
{
    Q_OBJECT

public:
    explicit GalleryThread(QObject *parent = nullptr);
    void setFromCount(int from, int count);

signals:
    void toModel(QVariantList list);
    void finished();

public slots:
    void process();

private:
    int from;
    int count;

    const QString dirPath = "thumbs";
    const QString path = QDir::currentPath() + "/" + dirPath + "/";
};

#endif // GALLERYTHREAD_H
