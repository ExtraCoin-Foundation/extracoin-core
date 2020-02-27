SOURCES += \
    $$PWD/sources/ui/extracoinclient.cpp \
    sources/ui/ui_resolver.cpp \
    sources/ui/notificationclient.cpp \
    sources/ui/permrequest.cpp \
    sources/ui/profiledata.cpp \
    sources/ui/etutils.cpp \
    sources/ui/translater.cpp \
    sources/ui/ui_controller.cpp \
    sources/ui/gallerythread.cpp \
    sources/ui/documenthandler.cpp \
    sources/ui/model/portfoliomodel.cpp \
    sources/ui/model/abstractmodel.cpp \
    sources/ui/model/newsmodel.cpp \
    sources/ui/model/eventsmodel.cpp \
    sources/ui/model/chatlistmodel.cpp \
    sources/ui/model/chatmodel.cpp \
    sources/ui/model/contactsmodel.cpp \
    sources/ui/model/contractsmodel.cpp \
    sources/ui/model/searchmodel.cpp \
    sources/ui/model/calendarmodel.cpp \
    sources/ui/model/gallerymodel.cpp \
    sources/ui/model/notifymodel.cpp \
    sources/ui/wallet/walletcontroller.cpp \
    sources/ui/wallet/walletlistmodel.cpp \
    sources/ui/wallet/recentactivitiesmodel.cpp \
    sources/ui/wallet/availablewalletsmodel.cpp

HEADERS += \
    $$PWD/headers/ui/extracoinclient.h \
    headers/ui/network_factory.h \
    headers/ui/notificationclient.h \
    headers/ui/permrequest.h \
    headers/ui/profiledata.h \
    headers/ui/etutils.h \
    headers/ui/ui_controller.h \
    headers/ui/gallerythread.h \
    headers/ui/documenthandler.h \
    headers/ui/translater.h \
    headers/ui/model/portfoliomodel.h \
    headers/ui/model/abstractmodel.h \
    headers/ui/model/newsmodel.h \
    headers/ui/model/eventsmodel.h \
    headers/ui/model/chatlistmodel.h \
    headers/ui/model/chatmodel.h \
    headers/ui/model/contractsmodel.h \
    headers/ui/model/contactsmodel.h \
    headers/ui/model/searchmodel.h \
    headers/ui/model/calendarmodel.h \
    headers/ui/model/gallerymodel.h \
    headers/ui/model/notifymodel.h \
    headers/ui/ui_resolver.h \
    headers/ui/wallet/walletcontroller.h \
    headers/ui/wallet/walletlistmodel.h \
    headers/ui/wallet/recentactivitiesmodel.h \
    headers/ui/wallet/availablewalletsmodel.h \

ios {
    QMAKE_TARGET_BUNDLE_PREFIX = com.excfoundation.extracoin
#    QMAKE_DEVELOPMENT_TEAM =
    OBJECTIVE_SOURCES += sources/ui/iosutils.mm \
                        sources/ui/qtiosnotifier.mm
    OBJECTIVE_HEADERS += headers/ui/iosutils.h \
                        headers/ui/qtiosnotifier.h
    LIBS += -framework Photos -framework AssetsLibrary
    LIBS += -framework UIKit
    LIBS += -framework Foundation
    LIBS += -framework UserNotifications
} 
