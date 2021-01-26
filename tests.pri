QT += testlib network
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TEMPLATE = app

include($$PWD/tests/network/pdu/test_pdu.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/./release/ -lOTPLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/./debug/ -lOTPLib
else:unix: LIBS += -L$$OUT_PWD/./ -lOTPLib

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src
