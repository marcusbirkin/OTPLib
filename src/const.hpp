/**
 * @file        const.hpp
 * @brief       Constants used in the OTP standard
 * @details     Part of OTPLib - A QT interface for E1.59
 * @authors     Marcus Birkin
 * @copyright   Copyright (C) 2019 Marcus Birkin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef CONST_HPP
#define CONST_HPP

#include <QTime>
#include <QHostAddress>
#include "types.hpp"
#include "network/pdu/pdu_types.hpp"

namespace OTP {
    /**
     * @brief Valid ranges for custom types
     * 
     */
    namespace RANGES {
        /** 
         * @name ANSI E1.59:2021 9.3 Priority
         * 
         * @{
         */  

        /** @brief Priorities shall be in the decimal range 0 through 200. 
         * Priorities 201 through 255 are reserved for future use. 
         * Consumers shall discard any Point with a reserved priority. 
         * Producers that do not support variable priority shall transmit a priority of 100.
         */
        inline const range_t Priority = {0,200};

        /**@}*/ // ANSI E1.59:2021 9.3 Priority

        /** 
         * @name ANSI E1.59:2021 Table 7-2: Address Numbering  
         * 
         * @{
         */  

        /**
         * @brief Valid range for System numbers
         * 
         */
        inline const range_t System = {1,200};

        /**
         * @brief Valid range for Group numbers
         * 
         */
        inline const range_t Group = {1,60000};

        /**
         * @brief Valid range for Point numbers
         * 
         */
        inline const range_t Point = {1,4000000000};

        /**@}*/ // ANSI E1.59:2021 Table 7-2: Address Numbering
    }

    /** 
     * @internal
     * @name ANSI E1.59:2021 Table 15-2: IPv4 Multicast Addresses 
     * 
     * @{
     */  

    /**
     * @internal
     * @brief OTP Transform Message IPv4 Mutlicast Base Address
     * @details The fourth byte is replaced by the system number 
     * 
     * Byte 1 | Byte 2 | Byte 3 | Byte 4
     * :----: | :----: | :----: | :----:
     * 239    | 159    | 1      | System Number
     * 
     */
    inline const QHostAddress OTP_Transform_Message_IPv4 = QHostAddress("239.159.1.0");

    /**
     * @internal
     * @brief OTP Advertisement Message IPv4 Mutlicast Address
     * 
     */
    inline const QHostAddress OTP_Advertisement_Message_IPv4 = QHostAddress("239.159.2.1");

    /** @internal @}*/ // ANSI E1.59:2021 Table 15-2: IPv4 Multicast Addresses 

    /** 
     * @internal
     * @name ANSI E1.59:2021 15-3: IPv6 Multicast Addresses 
     *@{*/ 

    /**
     * @internal
     * @brief OTP Transform Message IPv6 Mutlicast Base Address
     * @details The fifth byte of the Group ID is replaced by the system number 
     * 
     * Prefix (1 Byte) | Flags (4 bits) | Scope (4 bits) | Group ID (112 bits)
     * :-------------  | :------------: | :------------: | :-----------------:
     * 0xff            | 0 R P T        | 0x8            | 00::9f:00:01:[System Number]
     * 
     */
    inline const QHostAddress OTP_Transform_Message_IPv6 = QHostAddress("FF18:0000:0000:0000:009f:0000:0001:0000");
    
    /**
     * @internal
     * @brief OTP Advertisement Message IPv6 Mutlicast Address
     * 
     */
    inline const QHostAddress OTP_Advertisement_Message_IPv6 = QHostAddress("FF18:0000:0000:0000:009f:0000:0002:0001");
    
    /** @internal @}*/  // ANSI E1.59:2021 15-3: IPv6 Multicast Addresses 

    namespace PDU {
        /** 
         * @internal
         * @name ANSI E1.59:2021 Table A-1: Vector Defines 
         *
         * @{
         */ 
            
        /**
         * @internal
         * @brief OTP Layer Vector indicating an OTP Transform Message PDU
         * @details 
         * Producers sending an OTP Transform Message shall set the OTP Layer’s Vector to
         * VECTOR_OTP_TRANSFORM_MESSAGE, indicating 
         * that the OTP Layer is wrapping an OTP Transform PDU.
         */
        inline const vector_t VECTOR_OTP_TRANSFORM_MESSAGE = 0x0001;

        /**
         * @internal
         * @brief OTP Layer Vector indicating an OTP Advertisement Message PDU
         * @details
         * Components sending an OTP Advertisement Message shall set the OTP Layer’s Vector to
         * VECTOR_OTP_ADVERTISEMENT_MESSAGE to indicate 
         * the OTP Layer is wrapping an OTP Advertisement PDU.
         */
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_MESSAGE = 0x0002;

        /**
         * @internal
         * @brief OTP Transform Layer Vector indicating an OTP Point PDU
         * @details 
         * The vector appearing in the Transform Layer Vector field shall be VECTOR_OTP_POINT. 
         * Consumers shall ignore Transform PDUs with any other vectors.
         */
        inline const vector_t VECTOR_OTP_POINT = 0x0001;

        /**
         * @internal
         * @brief OTP Point Layer Vector indicating an OTP Module PDU
         * @details 
         * The Vector in the Point Layer indicates the type of information in the Data field of the Point PDU. 
         * It shall be VECTOR_OTP_MODULE. Consumers shall ignore Point PDUs with any other vectors.
         * 
         */
        inline const vector_t VECTOR_OTP_MODULE = 0x0001;

        /**
         * @internal
         * @brief OTP Advertisement Layer Vector indicating an OTP Module Advertisement Message
         * @details 
         * Consumers sending an OTP Module Advertisement Message shall set the vector to 
         * VECTOR_OTP_ADVERTISEMENT_MODULE, indicating that it contains module advertisement data.
         * 
         */
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_MODULE = 0x0001;

        /**
         * @internal
         * @brief OTP Advertisement Layer Vector indicating an OTP Name Advertisement Message
         * @details 
         * Components sending an OTP Name Advertisement Message shall set the vector to 
         * VECTOR_OTP_ADVERTISEMENT_NAME, indicating that it contains name advertisement data.
         * 
         */
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_NAME = 0x0002;

        /**
         * @internal
         * @brief OTP Advertisement Layer Vector indicating an OTP System Advertisement Message
         * @details 
         * Components sending an OTP System Advertisement Message shall set the vector to 
         * VECTOR_OTP_ADVERTISEMENT_SYSTEM, indicating that it contains system advertisement data.
         * 
         */
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_SYSTEM = 0x0003;

        /**
         * @internal
         * @brief OTP Module Advertisement Layer Vector indicating an OTP Module List Advertisement Message
         * @details 
         * The Module Advertisement Layer’s Vector shall be set to 
         * VECTOR_OTP_ADVERTISEMENT_MODULE_LIST, indicating that it contains a List of Module Identifiers.
         * 
         */
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_MODULE_LIST = 0x0001;

        /**
         * @internal
         * @brief OTP Name Advertisement Layer Vector indicating an OTP Name List Advertisement Message
         * @details 
         * The Name Advertisement Layer’s Vector shall be set to 
         * VECTOR_OTP_ADVERTISEMENT_NAME_LIST, indicating that it contains a List of Address Point Descriptions.
         * 
         */
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_NAME_LIST = 0x0001;

        /**
         * @internal
         * @brief OTP System Advertisement Layer Vector indicating an OTP System List Advertisement Message
         * @details 
         * The System Advertisement Layer’s Vector shall be set to 
         * VECTOR_OTP_ADVERTISEMENT_SYSTEM_LIST, indicating that it contains a List of System Numbers.
         * 
         */
        inline const vector_t VECTOR_OTP_ADVERTISEMENT_SYSTEM_LIST = 0x0001;

        /** @internal @}*/ // ANSI E1.59:2021 Table A-1: Vector Defines
    }

    /** 
     * @internal
     * @name ANSI E1.59:2021 Table A-2: Timing Defines 
     *
     * @{
     */ 

    /**
     * @brief OTP Transform - Interval Minimum
     * @details 
     * Producers shall transmit an OTP Folio of OTP Transform Messages for each System they are transmitting 
     * at a fixed rate between OTP_TRANSFORM_TIMING_MIN and OTP_TRANSFORM_TIMING_MAX. 
     * This OTP Folio shall contain either a Full Point Set, or all Points with changing OTP Module Layer data.
     * 
     */
    inline const std::chrono::milliseconds OTP_TRANSFORM_TIMING_MIN = std::chrono::milliseconds(1);

    /**
     * @brief OTP Transform - Interval Maximum
     * @details 
     * Producers shall transmit an OTP Folio of OTP Transform Messages for each System they are transmitting 
     * at a fixed rate between OTP_TRANSFORM_TIMING_MIN and OTP_TRANSFORM_TIMING_MAX. 
     * This OTP Folio shall contain either a Full Point Set, or all Points with changing OTP Module Layer data.
     * 
     */
    inline const std::chrono::milliseconds OTP_TRANSFORM_TIMING_MAX = std::chrono::milliseconds(50);

    /**
     * @internal
     * @brief OTP Transform - Full Set Interval Minimum
     * @details 
     * Regardless of the contents of other OTP Folios, for each System being transmitted a single OTP Folio 
     * containing a Full Point Set shall always be transmitted at intervals of between 
     * OTP_TRANSFORM_FULL_POINT_SET _TIMING_MIN and OTP_TRANSFORM_FULL_POINT_SET_TIMING_MAX.
     * 
     * <b>Full Point Set:</b> A Full Point Set for a Producer includes every Point for a specific System 
     * for which a Producer is currently providing transform information.
     * 
     */
    inline const std::chrono::milliseconds OTP_TRANSFORM_FULL_POINT_SET_TIMING_MIN = std::chrono::milliseconds(2800);
    
    /**
     * @internal
     * @brief OTP Transform - Full Set Interval Maximum
     * @details 
     * Regardless of the contents of other OTP Folios, for each System being transmitted a single OTP Folio 
     * containing a Full Point Set shall always be transmitted at intervals of between 
     * OTP_TRANSFORM_FULL_POINT_SET _TIMING_MIN and OTP_TRANSFORM_FULL_POINT_SET_TIMING_MAX.
     * 
     * <b>Full Point Set:</b> A Full Point Set for a Producer includes every Point for a specific System 
     * for which a Producer is currently providing transform information.
     * 
     */
    inline const std::chrono::milliseconds OTP_TRANSFORM_FULL_POINT_SET_TIMING_MAX = std::chrono::milliseconds(3000);

    /**
     * @internal
     * @brief OTP Transform - Timeout
     * @details 
     * A Consumer shall wait OTP_TRANSFORM_DATA_LOSS_TIMEOUT after receiving the last OTP Transform Message 
     * from a Producer before entering a data loss state. 
     * Behavior of Consumers once in data loss is beyond the scope of this standard.
     * 
     */
    inline const std::chrono::milliseconds OTP_TRANSFORM_DATA_LOSS_TIMEOUT = std::chrono::milliseconds(7500);

    /**
     * @internal
     * @brief OTP Module Advertisement - Interval
     * @details 
     * Consumers shall transmit an OTP Module Advertisement Message every 
     * OTP_ADVERTISEMENT_TIMING, containing a list of all Module Identifiers the Consumer is currently 
     * interested in receiving.
     */
    inline const std::chrono::milliseconds OTP_ADVERTISEMENT_TIMING = std::chrono::seconds(10);
    
    /**
     * @internal
     * @brief OTP Module Advertisement - Transform Delay
     * @details 
     * Producers must wait OTP_ADVERTISEMENT_STARTUP_WAIT after initialization before 
     * transmitting any OTP Transform Messages.
     * 
     */
    inline const std::chrono::milliseconds OTP_ADVERTISEMENT_STARTUP_WAIT = std::chrono::seconds(12);

    /**
     * @internal
     * @brief OTP Module Advertisement - Timeout
     * @details
     * Producers shall transmit only the Modules (and any associated Modules listed in Table 16-1) for which
     * they have received in an OTP Module Advertisement Message within
     * OTP_ADVERTISEMENT_TIMEOUT.
     * 
     */
    inline const std::chrono::milliseconds OTP_ADVERTISEMENT_TIMEOUT = std::chrono::seconds(30);
    
    /**
     * @internal
     * @brief OTP Name Advertisement - Reply backoff interval maximum
     * @details 
     * When an OTP Producer supporting Name Advertisement receives an OTP Name Advertisement Message 
     * with the OTP Name Advertisement Layer’s Options Request/Response bit set to 0, it shall transmit an 
     * OTP Name Advertisement Message unicast to the source of the request, after a randomly selected delay,
     * within the range of 0 seconds – OTP_NAME_ADVERTISEMENT_MAX_BACKOFF, inclusive.
     */
    inline const std::chrono::milliseconds OTP_NAME_ADVERTISEMENT_MAX_BACKOFF = std::chrono::seconds(5);

    /**
     * @internal
     * @brief OTP System Advertisement - Reply backup interval maximum
     * @details 
     * When an OTP Producer receives an OTP System Advertisement Message with the 
     * OTP System Advertisement Layer’s Options Request/Response bit set to 0, it shall transmit an OTP System
     * Advertisement Message unicast to the source of the request, after a randomly selected delay within the
     * range of 0 seconds – OTP_SYSTEM_ADVERTISEMENT_MAX_BACKOFF, inclusive.
     * 
     */
    inline const std::chrono::milliseconds OTP_SYSTEM_ADVERTISEMENT_MAX_BACKOFF = std::chrono::seconds(5);
    /** @internal @}*/ // ANSI E1.59:2021 Table A-2: Timing Defines

    /** 
     * @internal
     * @name Unofficial 
     *
     * @{
     */ 

    /** 
     * @internal
     * @brief Component Timeout 
     * @details Time to wait before declaring a component lost/offline
     * 
     * ANSI E1.59:2021 doesn't specify a component timeout this however should be a suitable interval 
     * 
     */
    inline const std::chrono::milliseconds OTP_COMPONENT_TIMEOUT = OTP_ADVERTISEMENT_TIMEOUT; 

    /** @internal @}*/ // Unofficial

    /** 
     * @internal
     * @name ANSI E1.59:2021 Table A-3: Additional Defines 
     *
     * @{
     */ 

    /**
     * @internal
     * @brief Network UDP port
     * @details When operating over [UDP], the multicast destination port shall be set to OTP_PORT.
     * 
     */ 
    inline const quint16 OTP_PORT = 5568;

    /**
     * @internal
     * @brief ESTA Manufactuerer ID
     * @details 
     * Modules are uniquely identified using a Module Identifier, 
     * which is composed of a Manufacturer ID and a 16-bit Module Number. 
     * For all standard Modules, the Manufacturer ID shall be set to ESTA_MANUFACTURER_ID.
     */
    inline const quint16 ESTA_MANUFACTURER_ID = 0x0000;

    /** @internal @}*/ // ANSI E1.59:2021 Table A-3: Additional Defines
}
#endif // CONST_HPP
