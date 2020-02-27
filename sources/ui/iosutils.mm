#include "ui/iosutils.h"

#import <sys/utsname.h>
#import <Photos/PHAsset.h>
#import <Photos/Photos.h>

#include <QDateTime>
#include <QString>
#include <QVariantMap>
#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include <QImageReader>

const int MEDIA_PHOTOS = 1;
const int MEDIA_VIDEOS = 2;
#pragma unused (MEDIA_VIDEOS)
const QString MEDIA_PHOTO_SUFFIX = "%1";
const QString MEDIA_VIDEO_SUFFIX = "MP4";


iOSUtils::iOSUtils(QObject *parent)
    : QObject(parent)
{
}

bool iOSUtils::iPhoneLess()
{
    QStringList iPhonesLess = { "iPhone10,3", "iPhone10,6", "iPhone11,2", "iPhone11,4", "iPhone11,6", "iPhone11,8", "iPhone12,1" , "iPhone12,3", "iPhone12,5" };

    struct utsname systemInfo;
    uname(&systemInfo);
    NSString* deviceModel = [NSString stringWithCString:systemInfo.machine
                                               encoding:NSUTF8StringEncoding];

    QString iPhoneCurrent = QString::fromNSString(deviceModel);
    qDebug() << "iPhone Model:" << iPhoneCurrent;

    return iPhonesLess.contains(iPhoneCurrent);
}

QVariantMap iOSUtils::fetchAssets(const int mediaType)
{
    QVariantMap albumsWithSortedAssetsMap;
    QMap<QDateTime, QString> assetsMap;
    // get the smart albums
    PHFetchResult *smartAlbums = [PHAssetCollection fetchAssetCollectionsWithType:PHAssetCollectionTypeSmartAlbum subtype:PHAssetCollectionSubtypeAlbumRegular options:nil];
    for (NSUInteger i = 0; i < smartAlbums.count; i++) {
        PHAssetCollection *assetCollection = smartAlbums[i];
        // the smart album name
        QString theAlbum = QString::fromNSString(assetCollection.localizedTitle);

        // get photo collections for this album
        PHFetchResult *assetsFetchResult = [PHAsset fetchAssetsInAssetCollection:assetCollection options:nil];

        // to get it sorted by modificationDate we use a Map
        assetsMap.clear();

        // get the Assets (Photos or Videos)
        for (NSUInteger x = 0; x < assetsFetchResult.count; x ++) {
         //   qDebug() << "x" << x;
            PHAsset *asset = assetsFetchResult[x];

            int mt = asset.mediaType;
            if(mt != mediaType) {
                continue;
                qDebug() << " zhz";
            }
            QDateTime modificationDate = QDateTime::fromNSDate(asset.modificationDate);

            QString theSuffix;
            if(mediaType == MEDIA_PHOTOS) {
                theSuffix = MEDIA_PHOTO_SUFFIX;
            } else {
                theSuffix = MEDIA_VIDEO_SUFFIX;
            }

            // now constructing the URL
            // https://stackoverflow.com/questions/42579544/ios-phassetcollection-localizedtitle-always-returning-english-name
            // https://stackoverflow.com/questions/28887638/how-to-get-an-alasset-url-from-a-phasset
            // AF3C1B68-222B-43D3-9478-53097429F1B2/L0/001 becomes
            // assets-library://asset/asset.JPG?id=AF3C1B68-222B-43D3-9478-53097429F1B2&ext=JPG
            QString theId = QString::fromNSString(asset.localIdentifier);
            QString theUrl = "assets-library://asset/asset";

            //theUrl.append(theSuffix)
            theUrl.append("?id=");
            theUrl.append(theId.split('/').first());
            // theUrl.append("&ext=").append(theSuffix);


           /* if(QFile(theUrl).exists()){
               qDebug() << theUrl << " exist";
            }   else {
                qDebug() << "no1";
                theSuffix = "HEIC";
                QString theUrl = "assets-library://asset/asset.";

                theUrl.append(theSuffix).append("?id=");
                theUrl.append(theId.split('/').first());
                theUrl.append("&ext=").append(theSuffix);

                if(QFile(theUrl).exists()){
                              qDebug() << theUrl << " exist";
                    //do nithing
                }else{
                                    qDebug() << "no2";
                    theSuffix = "PNG";
                    QString theUrl = "assets-library://asset/asset.";

                    theUrl.append(theSuffix).append("?id=");
                    theUrl.append(theId.split('/').first());
                    theUrl.append("&ext=").append(theSuffix);
             if(QFile(theUrl).exists()) {qDebug() << theUrl << " exist";
             }else {                qDebug() << "no3";}*///}
                   // }
            // qDebug() << "URL: " << theUrl;

         //  qDebug() << "file" << x << theUrl <<  QFile(theUrl).exists();
            assetsMap.insert(modificationDate, theUrl);

        } // assets in collection

        // now we can fill our list of assets-URLs (sorted descendend by modificationDate)
        QStringList sortedAssetsList;
        QMapIterator<QDateTime, QString> assetsIterator(assetsMap);
        assetsIterator.toBack();
        while (assetsIterator.hasPrevious()) {
            assetsIterator.previous();
            sortedAssetsList.append(assetsIterator.value());
        }

        // insert sorted list of URLLs into albums map
        albumsWithSortedAssetsMap.insert(theAlbum, sortedAssetsList);
//        for( QVariantMap::Iterator it = albumsWithSortedAssetsMap.begin() ; it != albumsWithSortedAssetsMap.end() ; ++it)
//        {
//            qDebug() << it.key();
//        }


    } // media collections in smart album
    // now we have all albums sorted by album name together with sorted collections of assets, where the newest comes first



    return albumsWithSortedAssetsMap;
}

