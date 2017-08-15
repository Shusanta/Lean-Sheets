QT += widgets
#unix:qtHaveModule(dbus): QT += dbus widgets

HEADERS += leansheets.h leandelegate.h leanitem.h \

SOURCES += main.cpp \
           leansheets.cpp \
           leandelegate.cpp \
           leanitem.cpp \

RESOURCES += \
    leanfiles.qrc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/itemviews/leansheets
INSTALLS += target
