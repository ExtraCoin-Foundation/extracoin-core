#include "ui/model/gallerymodel.h"
#include "managers/thread_pool.h"

#include <QThread>

GalleryModel::GalleryModel(AbstractModel *parent)
    : AbstractModel(parent)
{
    setModelRoles({ "original", "thumb", "name" });
    QDir().mkdir(dirPath);
}

void GalleryModel::loadGallery()
{
    if (isStarted)
        return;

#ifndef Q_OS_IOS
    clear();
    isStarted = true;

    galleryThread = new GalleryThread;
    galleryThread->setFromCount(1, 10000);
    connect(galleryThread, &GalleryThread::toModel, this, &GalleryModel::toModel);
    connect(galleryThread, &GalleryThread::finished, this, &GalleryModel::finished);

    ThreadPool::addThread(galleryThread);
#endif
}

double GalleryModel::sizeCache()
{
    qint64 size = 0;
    QDir dir(dirPath);

    auto files = dir.entryList(QDir::Files);
    for (auto &&file : files)
    {
        QFileInfo fileInfo(dir, file);
        size += fileInfo.size();
    }

    double sizeMb = double(size) / 1024 / 1024;
    return sizeMb;
}

void GalleryModel::clearCache()
{
    QDir(dirPath).removeRecursively();
    QDir().mkdir(dirPath);
    clear();
}

void GalleryModel::toModel(QVariantList images)
{
    for (auto &&variant : images)
        append(variant.toMap());
}

void GalleryModel::finished()
{
    isStarted = false;
}
