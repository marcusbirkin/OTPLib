QT += testlib network
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TARGET = OTPLib_TESTS
TEMPLATE = app

include(network/pdu/test_pdu.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/release/ -lOTPLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/debug/ -lOTPLib
else:unix: LIBS += -L$$OUT_PWD/../src/ -lOTPLib

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src
