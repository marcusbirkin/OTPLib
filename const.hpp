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

    /* Table 6-2: Address Numbering */
    namespace RANGES {
        const range_t System = {1,200};
        const range_t Group = {1,60000};
        const range_t Point = {1,4000000000};
    }

    /* Table 15-2: IPv4 Multicast Addresses */
    const QHostAddress OTP_Transform_Message_IPv4 = QHostAddress("239.159.1.0"); // Base address excluding system number
    const QHostAddress OTP_Advertisement_Message_IPv4 = QHostAddress("239.159.2.1");

    /* Table 15-3: IPv6 Multicast Addresses */
    const QHostAddress OTP_Transform_Message_IPv6 = QHostAddress("FF15:0000:0000:0000:009f:0000:0001:0000"); // Base address excluding system number
    const QHostAddress OTP_Advertisement_Message_IPv6 = QHostAddress("FF15:0000:0000:0000:009f:0000:0002:0001");

    namespace PDU {
        /* Table A-1: Vector Defines */
        // OTP
        const vector_t VECTOR_OTP_TRANSFORM_MESSAGE = 0xFF000001;
        const vector_t VECTOR_OTP_ADVERTISEMENT_MESSAGE = 0xFF000002;
        // OTP Transform
        const vector_t VECTOR_OTP_POINT = 0x00000001;
        // OTP Point
        const vector_t VECTOR_OTP_MODULE = 0x00000001;
        // OTP Advertisement
        const vector_t VECTOR_OTP_ADVERTISEMENT_MODULE = 0x00000001;
        const vector_t VECTOR_OTP_ADVERTISEMENT_NAME = 0x00000002;
        const vector_t VECTOR_OTP_ADVERTISEMENT_SYSTEM = 0x00000003;
        // OTP Module Advertisement
        const vector_t VECTOR_OTP_ADVERTISEMENT_MODULE_LIST = 0x00000001;
        // OTP Name Advertisement
        const vector_t VECTOR_OTP_ADVERTISEMENT_NAME_LIST = 0x00000001;
        // OTP System Advertisement
        const vector_t VECTOR_OTP_ADVERTISEMENT_SYSTEM_LIST = 0x00000001;
    }

    /* Table A-2: Timing Defines */
    // OTP Transform
    const std::chrono::milliseconds OTP_TRANSFORM_TIMING_MIN = std::chrono::milliseconds(1);
    const std::chrono::milliseconds OTP_TRANSFORM_TIMING_MAX = std::chrono::milliseconds(50);
    const std::chrono::milliseconds OTP_TRANSFORM_KEEPALIVE_TIMING_MIN = std::chrono::milliseconds(2800);
    const std::chrono::milliseconds OTP_TRANSFORM_KEEPALIVE_TIMING_MAX = std::chrono::milliseconds(3000);
    const std::chrono::milliseconds OTP_TRANSFORM_DATA_LOSS_TIMEOUT = std::chrono::milliseconds(7500);
    // OTP Module Advertisement
    const std::chrono::milliseconds OTP_ADVERTISEMENT_TIMING = std::chrono::seconds(10);
    const std::chrono::milliseconds OTP_ADVERTISEMENT_STARTUP_WAIT = std::chrono::seconds(12);
    const std::chrono::milliseconds OTP_ADVERTISEMENT_TIMEOUT = std::chrono::seconds(30);

    /* Table A-3: Additional Defines */
    const quint16 ACN_SDT_MULTICAST_PORT = 5568;
    const quint16 ESTA_MANUFACTURER_ID = 0x0000;
}
#endif // CONST_HPP
