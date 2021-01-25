QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

include($$PWD/OTPLib.pri)
include($$PWD/src/network/pdu/test_pdu.pri)
