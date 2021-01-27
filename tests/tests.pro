QT += testlib network
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase c++17
CONFIG -= app_bundle

TARGET = OTPLib_TESTS
TEMPLATE = app

include(network/pdu/test_pdu.pri)

CONFIG(release, debug|release): LIBS += -L$$PWD/../build/release/ -lOTPLib
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build/debug/ -lOTPLib

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src
