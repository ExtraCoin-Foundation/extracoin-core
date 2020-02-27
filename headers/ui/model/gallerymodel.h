#ifndef GALLERYMODEL_H
#define GALLERYMODEL_H

#include "ui/model/abstractmodel.h"
#include "ui/gallerythread.h"

class GalleryModel : public AbstractModel
{
    Q_OBJECT

public:
    explicit GalleryModel(AbstractModel* parent = nullptr);

    Q_INVOKABLE void loadGallery();
    Q_INVOKABLE double sizeCache();
    Q_INVOKABLE void clearCache();

public slots:
    void toModel(QVariantList images);

private slots:
    void finished();

private:
    const QString dirPath = "thumbs";
    bool isStarted = false;
    GalleryThread* galleryThread;
};

#endif // GALLERYMODEL_H
