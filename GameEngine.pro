greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    src/fontmanager.cpp \
    src/gamescene.cpp \
    src/main.cpp \
    src/pixmapmanager.cpp \
    src/sprite.cpp \
    src/view.cpp

HEADERS += \
    src/fontmanager.h \
    src/gamescene.h \
    src/pixmapmanager.h \
    src/resourceholder.h \
    src/sprite.h \
    src/spritesheet.h \
    src/utils.h \
    src/view.h

RESOURCES += \
    resource.qrc
