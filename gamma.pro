QT += core
QT += gui
QT += opengl
QT += widgets
QT += charts
QT += printsupport
QT += serialport
QT += network
#QT += core5compat
QT -= sql

CONFIG += c++14

#CONFIG += console
CONFIG += warn_on

TEMPLATE  = app
TARGET    = gamma
LOCALE    = en_US ru_RU

INCLUDEPATH += .
DEPENDPATH  += $(INCLUDEPATH)

#DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES  += QT_NO_CAST_TO_ASCII
DEFINES  += QT_LOGGING_TO_CONSOLE=1
# DEFINES +=  QT_NO_DEBUG_OUTPUT


include(pri/defs.pri)
include(pri/install.pri)
include(pri/locale.pri)
include(pri/prj.pri)

include(src.pri)

RESOURCES += images/images.qrc
win32:RC_FILE = images/q_app_logo.rc
