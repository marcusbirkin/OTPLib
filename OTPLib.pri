QT *= network
CONFIG *= c++17

include($$PWD/version.pri)

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$PWD/src/otp.cpp \
    $$PWD/src/container.cpp \
    $$PWD/src/component.cpp \
    $$PWD/src/producer.cpp \
    $$PWD/src/consumer.cpp \
    $$PWD/src/types.cpp \
    $$PWD/src/socket.cpp \
    $$PWD/src/network/pdu/pdu.cpp \
    $$PWD/src/network/pdu/pdu_types.cpp \
    $$PWD/src/network/pdu/otp_layer.cpp \
    $$PWD/src/network/pdu/otp_transform_layer.cpp \
    $$PWD/src/network/pdu/otp_point_layer.cpp \
    $$PWD/src/network/pdu/otp_module_layer.cpp \
    $$PWD/src/network/pdu/otp_advertisement_layer.cpp \
    $$PWD/src/network/pdu/otp_module_advertisement_layer.cpp \
    $$PWD/src/network/pdu/otp_name_advertisement_layer.cpp \
    $$PWD/src/network/pdu/otp_system_advertisement_layer.cpp \
    $$PWD/src/network/messages/otp_module_advertisement_message.cpp \
    $$PWD/src/network/messages/otp_name_advertisement_message.cpp \
    $$PWD/src/network/messages/otp_system_advertisement_message.cpp \
    $$PWD/src/network/messages/otp_transform_message.cpp \
    $$PWD/src/network/modules/modules.cpp \
    $$PWD/src/network/modules/modules_types.cpp \
    $$PWD/src/network/modules/modules_const.cpp

HEADERS += \
    $$PWD/src/bugs.hpp \
    $$PWD/src/component.hpp \
    $$PWD/src/otp.hpp \
    $$PWD/src/container.hpp \
    $$PWD/src/const.hpp \
    $$PWD/src/enums.hpp \
    $$PWD/src/types.hpp \
    $$PWD/src/socket.hpp \
    $$PWD/src/network/pdu/pdu.hpp \
    $$PWD/src/network/pdu/pdu_const.hpp \
    $$PWD/src/network/pdu/pdu_types.hpp \
    $$PWD/src/network/pdu/otp_layer.hpp \
    $$PWD/src/network/pdu/otp_transform_layer.hpp \
    $$PWD/src/network/pdu/otp_point_layer.hpp \
    $$PWD/src/network/pdu/otp_module_layer.hpp \
    $$PWD/src/network/pdu/otp_advertisement_layer.hpp \
    $$PWD/src/network/pdu/otp_module_advertisement_layer.hpp \
    $$PWD/src/network/pdu/otp_name_advertisement_layer.hpp \
    $$PWD/src/network/pdu/otp_system_advertisement_layer.hpp \
    $$PWD/src/network/messages/messages.hpp \
    $$PWD/src/network/messages/otp_module_advertisement_message.hpp \
    $$PWD/src/network/messages/otp_name_advertisement_message.hpp \
    $$PWD/src/network/messages/otp_system_advertisement_message.hpp \
    $$PWD/src/network/messages/otp_transform_message.hpp \
    $$PWD/src/network/messages/message_types.hpp \
    $$PWD/src/network/messages/message_const.hpp \
    $$PWD/src/network/modules/modules.hpp \
    $$PWD/src/network/modules/modules_types.hpp \
    $$PWD/src/network/modules/modules_const.hpp
