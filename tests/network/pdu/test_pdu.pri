INCLUDEPATH += \
    $$PWD \
    $$PWD/../messages

SOURCES += \
    $$PWD/test_pdu.cpp \
    $$PWD/test_otp_layer.cpp \
    $$PWD/test_otp_transform_layer.cpp \
    $$PWD/test_otp_advertisement_layer.cpp

HEADERS += \
    $$PWD/../messages/test_appendix_B.h \
    $$PWD/test_otp_layer.hpp \
    $$PWD/test_otp_transform_layer.hpp \
    $$PWD/test_otp_advertisement_layer.hpp
