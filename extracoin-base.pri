contains(DEFINES, EXTRACOIN_CONSOLE) {
    QT -= gui 
}

TARGET = extracoin
VERSION = 0.7.12

include(settings.pri)
include(files.pri)

contains(DEFINES, EXTRACOIN_CLIENT) {
    CONFIG += qzxing_qml qzxing_multimedia enable_decoder_qr_code
}

include(../../extracoin-3rdparty/extracoin-3rdparty.pri)
include(../lib.pri)
INCLUDEPATH+=$$PWD/
DEPENDPATH+=$$PWD/
INCLUDEPATH+=$$PWD/headers
DEPENDPATH+=$$PWD/headers

contains(DEFINES, EXTRACOIN_CLIENT) {
    QT += gui qml quick quickcontrols2 widgets
    include(files-ui.pri)
}
