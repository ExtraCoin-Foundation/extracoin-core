#include "ui/gallerythread.h"

#include <QDebug>
#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#endif
#ifdef Q_OS_IOS
#include "ui/iosutils.h"
#endif
#include <QImage>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QImageReader>
#include <QDirIterator>

GalleryThread::GalleryThread(QObject *parent)
    : QObject(parent)
{
}

void GalleryThread::setFromCount(int from, int count)
{
    this->from = from;
    this->count = count;
}

void GalleryThread::process()
{
    QVariantList list;

    if (from < 1)
        from = 1;

#ifdef Q_OS_ANDROID
    QAndroidJniObject imagesJava = QAndroidJniObject::callStaticObjectMethod(
        "com/excfoundation/extracoin/NotificationClient", "LastGalleryPictures",
        "(Landroid/app/Activity;II)Ljava/lang/String;", QtAndroid::androidActivity().object(), from, count);
    auto images = imagesJava.toString().split("##");
    images.removeLast();
    std::reverse(images.begin(), images.end());
#else
#ifdef Q_OS_IOS
    QStringList images = iOSUtils::fetchAssets(1).take("All Photos").toStringList();

#else
    QDirIterator it(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).last(),
                    QStringList() << "*.png"
                                  << "*.PNG"
                                  << "*.jpg"
                                  << "*.jpeg"
                                  << "*.JPG"
                                  << "*.gif"
                                  << "*.GIF",
                    QDir::Files, QDirIterator::Subdirectories);

    QDir directory(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).last());
    QStringList images = directory.entryList(QStringList() << "*.png"
                                                           << "*.PNG"
                                                           << "*.jpg"
                                                           << "*.jpeg"
                                                           << "*.JPG"
                                                           << "*.gif"
                                                           << "*.GIF",
                                             QDir::Files, QDir::Time);
    for (auto &&image : images)
        image = directory.path() + "/" + image;

    int j = 0;
    while (it.hasNext())
    {
        QString image = it.next();
        images << image;

        if (j++ == 1000)
            break;
    }

#endif
#endif

    int i = 0;
    for (auto &&imageOriginal : images)
    {
        if (imageOriginal.right(4).toLower() == ".gif")
        {
            list << QVariantMap { { "original", imageOriginal }, { "thumb", imageOriginal } };
            if (i++ % 20)
            {
                toModel(list);
                list.clear();
            }
            continue;
        }

        QString thumb = QCryptographicHash::hash(imageOriginal.toUtf8(), QCryptographicHash::Md5).toHex();

        if (!QFile(path + thumb).exists())
        {
            QImageReader reader(imageOriginal);

            if (!reader.canRead())
                continue;

            QByteArray format = reader.format();
            reader.setAutoTransform(true);
            QImage image = reader.read();

            if (image.isNull())
                continue;

            image.scaled(500, 500, Qt::KeepAspectRatioByExpanding).save(path + thumb, format);
        }

        list << QVariantMap { { "original", imageOriginal }, { "thumb", thumb } };

        if (i++ % 15)
        {
            toModel(list);
            list.clear();
        }
    }

    toModel(list);
    finished();
}
