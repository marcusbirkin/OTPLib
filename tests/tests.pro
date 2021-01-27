QT += testlib network
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

win32-msvc*: CONFIG += c++latest

TARGET = OTPLib_TESTS
TEMPLATE = app

include($$PWD/../version.pri)
include($$PWD/../src/source.pri)
include(network/pdu/test_pdu.pri)
