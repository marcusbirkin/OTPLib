include($$PWD/version.pri)

SOURCES += \
    $$PWD/otp.cpp \
    $$PWD/container.cpp \
    $$PWD/producer.cpp \
    $$PWD/consumer.cpp \
    $$PWD/types.cpp \
    $$PWD/socket.cpp \
    $$PWD/network/pdu/pdu.cpp \
    $$PWD/network/pdu/pdu_types.cpp \
    $$PWD/network/pdu/otp_layer.cpp \
    $$PWD/network/pdu/otp_transform_layer.cpp \
    $$PWD/network/pdu/otp_point_layer.cpp \
    $$PWD/network/pdu/otp_module_layer.cpp \
    $$PWD/network/pdu/otp_advertisement_layer.cpp \
    $$PWD/network/pdu/otp_module_advertisement_layer.cpp \
    $$PWD/network/pdu/otp_name_advertisement_layer.cpp \
    $$PWD/network/pdu/otp_system_advertisement_layer.cpp \
    $$PWD/network/messages/otp_module_advertisement_message.cpp \
    $$PWD/network/messages/otp_name_advertisement_message.cpp \
    $$PWD/network/messages/otp_system_advertisement_message.cpp \
    $$PWD/network/messages/otp_transform_message.cpp \
    $$PWD/network/modules/modules.cpp \
    $$PWD/network/modules/modules_const.cpp

HEADERS += \
    $$PWD/otp.hpp \
    $$PWD/container.hpp \
    $$PWD/const.hpp \
    $$PWD/enums.hpp \
    $$PWD/types.hpp \
    $$PWD/socket.hpp \
    $$PWD/network/pdu/pdu.hpp \
    $$PWD/network/pdu/pdu_const.hpp \
    $$PWD/network/pdu/pdu_types.hpp \
    $$PWD/network/pdu/otp_layer.hpp \
    $$PWD/network/pdu/otp_transform_layer.hpp \
    $$PWD/network/pdu/otp_point_layer.hpp \
    $$PWD/network/pdu/otp_module_layer.hpp \
    $$PWD/network/pdu/otp_advertisement_layer.hpp \
    $$PWD/network/pdu/otp_module_advertisement_layer.hpp \
    $$PWD/network/pdu/otp_name_advertisement_layer.hpp \
    $$PWD/network/pdu/otp_system_advertisement_layer.hpp \
    $$PWD/network/messages/messages.hpp \
    $$PWD/network/messages/otp_module_advertisement_message.hpp \
    $$PWD/network/messages/otp_name_advertisement_message.hpp \
    $$PWD/network/messages/otp_system_advertisement_message.hpp \
    $$PWD/network/messages/otp_transform_message.hpp \
    $$PWD/network/messages/message_types.hpp \
    $$PWD/network/messages/message_const.hpp \
    $$PWD/network/modules/modules.hpp \
    $$PWD/network/modules/modules_types.hpp \
    $$PWD/network/modules/modules_const.hpp
