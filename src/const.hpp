/*
    OTPLib
    A QT interface for E1.59 (Entertainment Technology Object Transform Protocol (OTP)) 
    Copyright (C) 2019  Marcus Birkin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef CONST_HPP
#define CONST_HPP

#include <QTime>
#include <QHostAddress>
#include "types.hpp"
#include "network/pdu/pdu_types.hpp"

namespace OTP {

    namespace RANGES {
        /* 9.3 Priority */
        const range_t Priority = {0, 200};

        /* Table 7-2: Address Numbering */
        inline const range_t System = {1,200};
        inline const range_t Group = {1,60000};
        inline const range_t Point = {1,4000000000};
    }

    /* Table 15-2: IPv4 Multicast Addresses */
    inline const QHostAddress OTP_Transform_Message_IPv4 = QHostAddress("239.159.1.0"); // Base address excluding system number
    inline const QHostAddress OTP_Advertisement_Message_IPv4 = QHostAddress("239.159.2.1");

    /* Table 15-3: IPv6 Multicast Addresses */
    inline const QHostAddress OTP_Transform_Message_IPv6 = QHostAddress("FF18:0000:0000:0000:009f:0000:0001:0000"); // Base address excluding system number
    inline const QHostAddress OTP_Advertisement_Message_IPv6 = QHostAddress("FF18:0000:0000:0000:009f:0000:0002:0001");

    namespace PDU {
        /* Table A-1: Vector Defines */
        // OTP
        inline const vector_t VECTOR_OTP_TRANSFORM_MESSAGE = 0xFF01;
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_MESSAGE = 0xFF02;
        // OTP Transform
        inline const vector_t VECTOR_OTP_POINT = 0x0001;
        // OTP Point
        inline const vector_t VECTOR_OTP_MODULE = 0x0001;
        // OTP Advertisement
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_MODULE = 0x0001;
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_NAME = 0x0002;
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_SYSTEM = 0x0003;
        // OTP Module Advertisement
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_MODULE_LIST = 0x0001;
        // OTP Name Advertisement
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_NAME_LIST = 0x0001;
        // OTP System Advertisement
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_SYSTEM_LIST = 0x0001;
    }

    /* Table A-2: Timing Defines */
    // OTP Transform
    inline const std::chrono::milliseconds OTP_TRANSFORM_TIMING_MIN = std::chrono::milliseconds(1);
    inline const std::chrono::milliseconds OTP_TRANSFORM_TIMING_MAX = std::chrono::milliseconds(50);
    inline const std::chrono::milliseconds OTP_TRANSFORM_FULL_POINT_SET_TIMING_MIN = std::chrono::milliseconds(2800);
    inline const std::chrono::milliseconds OTP_TRANSFORM_FULL_POINT_SET_TIMING_MAX = std::chrono::milliseconds(3000);
    inline const std::chrono::milliseconds OTP_TRANSFORM_DATA_LOSS_TIMEOUT = std::chrono::milliseconds(7500);
    // OTP Module Advertisement
    inline const std::chrono::milliseconds OTP_ADVERTISEMENT_TIMING = std::chrono::seconds(10);
    inline const std::chrono::milliseconds OTP_ADVERTISEMENT_STARTUP_WAIT = std::chrono::seconds(12);
    inline const std::chrono::milliseconds OTP_ADVERTISEMENT_TIMEOUT = std::chrono::seconds(30);
    // OTP Name Advertisement
    inline const std::chrono::milliseconds OTP_NAME_ADVERTISEMENT_MAX_BACKOFF = std::chrono::seconds(5);
    // OTP System Advertisement
    inline const std::chrono::milliseconds OTP_SYSTEM_ADVERTISEMENT_MAX_BACKOFF = std::chrono::seconds(5);

    /* Table A-3: Additional Defines */
    inline const quint16 OTP_PORT = 5568;
    inline const quint16 ESTA_MANUFACTURER_ID = 0x0000;
}
#endif // CONST_HPP
