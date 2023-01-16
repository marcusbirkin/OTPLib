/**
 * @file        pdu_const.hpp
 * @brief       Constants used relating to PDUs in the OTP standard
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
#ifndef PDU_CONST_HPP
#define PDU_CONST_HPP

#include "../../const.hpp"
#include "pdu_types.hpp"

namespace OTP::PDU {

    /**
     * @brief Maximum size of OTP Names
     * @details Point and Component Names shall not exceed 32 octets
     * 
     */
    const size_t NAME_LENGTH = 32;

    /**
     * @brief ANSI E1.59:2021 Section 6 OTP Layer
     * 
     */
    namespace OTPLayer {
        /** 
         * @brief OTP Packet Identifier
         * @details 
         * The OTP Packet Identifier shall contain the following sequence of hexadecimal characters: 
         * 0x4f 0x54 0x50 0x2d 0x45 0x31 0x2e 0x35 0x39 0x00 0x00 0x00.
         * 
         */
        inline const otpIdent_t OTP_PACKET_IDENT = QByteArrayLiteral("\x4f\x54\x50\x2d\x45\x31\x2e\x35\x39\x00\x00\x00");
        
        /**
         * @brief Layer length field offset
         * @details PDU octet offset for length field
         * 
         */
        const pduLength_t LENGTHOFFSET = static_cast<pduLength_t>(OTP_PACKET_IDENT.size() + sizeof(vector_t) + sizeof(pduLength_t));

        /**
         * @brief Allowed vectors for this PDU Layer
         * 
         */
        inline const QList<vector_t> VECTOR = {
            VECTOR_OTP_TRANSFORM_MESSAGE,
            VECTOR_OTP_ADVERTISEMENT_MESSAGE 
        }; 

        /**
         * @brief Expected value for options field
         * 
         */
        const options_t OPTIONS = 0x0;

        /**
         * @brief Expected value for reserved field
         * 
         */
        const reserved_t RESERVED = 0x0;
    }

    /**
     * @brief ANSI E1.59:2021 Section 8 OTP Transform Layer
     * 
     */
    namespace OTPTransformLayer {
        /**
         * @brief Layer length field offset
         * @details PDU octet offset for length field
         * 
         */
        constexpr pduLength_t LENGTHOFFSET = static_cast<pduLength_t>(sizeof(vector_t) + sizeof(pduLength_t));

        /**
         * @brief Expected value for vector field
         * 
         */
        const vector_t VECTOR = VECTOR_OTP_POINT;

        /**
         * @brief Expected value for reserved field
         * 
         */
        const reserved_t RESERVED = 0x0;
    }

    /**
     * @brief ANSI E1.59:2021 Section 9 OTP Point Layer
     * 
     */
    namespace OTPPointLayer {
        /**
         * @brief Layer length field offset
         * @details PDU octet offset for length field
         * 
         */
        constexpr pduLength_t LENGTHOFFSET = static_cast<pduLength_t>(sizeof(vector_t) + sizeof(pduLength_t));

        /**
         * @brief Expected value for vector field
         * 
         */
        const vector_t VECTOR = VECTOR_OTP_POINT;

        /**
         * @brief Expected value for options field
         * 
         */
        const options_t OPTIONS = 0x0;

        /**
         * @brief Expected value for reserved field
         * 
         */
        const reserved_t RESERVED = 0x0;
    }

    /**
     * @brief ANSI E1.59:2021 Section 10 OTP Module Layer
     * 
     */
    namespace OTPModuleLayer {
        /**
         * @brief Layer length field offset
         * @details PDU octet offset for length field
         * 
         */
        constexpr pduLength_t LENGTHOFFSET = static_cast<pduLength_t>(sizeof(manufacturerID_t) + sizeof(pduLength_t));
    }

    /**
     * @brief ANSI E1.59:2021 Section 11 OTP Advertisement Layer
     * 
     */
    namespace OTPAdvertisementLayer {
        /**
         * @brief Layer length field offset
         * @details PDU octet offset for length field
         * 
         */
        constexpr pduLength_t LENGTHOFFSET = static_cast<pduLength_t>(sizeof(vector_t) + sizeof(pduLength_t));

        /**
         * @brief Allowed vectors for this PDU Layer
         * 
         */
        inline const QList<vector_t> VECTOR = {
            VECTOR_OTP_ADVERTISEMENT_MODULE,
            VECTOR_OTP_ADVERTISEMENT_NAME,
            VECTOR_OTP_ADVERTISEMENT_SYSTEM};

        /**
         * @brief Expected value for reserved field
         * 
         */
        const reserved_t RESERVED = 0x0;
    }

    /**
     * @brief ANSI E1.59:2021 Section 12 OTP Module Advertisement Layer
     * 
     */
    namespace OTPModuleAdvertisementLayer {
        /**
         * @brief Layer length field offset
         * @details PDU octet offset for length field
         * 
         */
        constexpr pduLength_t LENGTHOFFSET = static_cast<pduLength_t>(sizeof(vector_t) + sizeof(pduLength_t));

        /**
         * @brief Expected value for vector field
         * 
         */
        const vector_t VECTOR = VECTOR_OTP_ADVERTISEMENT_MODULE_LIST;

        /**
         * @brief Expected value for reserved field
         * 
         */
        const reserved_t RESERVED = 0x0;

        /**
         * @brief Valid Ranges for layer
         * 
         */
        namespace RANGES {
            /**
             * @brief Allowed oclet size range of List field
             * 
             */
            inline const range_t ListSize = {4,1376};
        }
    }

    /**
     * @brief ANSI E1.59:2021 Section 13 OTP Name Advertisement Layer 
     * 
     */
    namespace OTPNameAdvertisementLayer {
        /**
         * @brief Layer length field offset
         * @details PDU octet offset for length field
         * 
         */
        constexpr pduLength_t LENGTHOFFSET = static_cast<pduLength_t>(sizeof(vector_t) + sizeof(pduLength_t));

        /**
         * @brief Expected value for vector field
         * 
         */
        const vector_t VECTOR = VECTOR_OTP_ADVERTISEMENT_NAME_LIST;

        /**
         * @brief Expected value for reserved field
         * 
         */
        const reserved_t RESERVED = 0x0;

        /**
         * @brief Valid Ranges for layer
         * 
         */
        namespace RANGES {
            /**
             * @brief Allowed oclet size range of List field
             * 
             */
            inline const range_t ListSize = {0,1365};
        }
    }

    /**
     * @brief ANSI E1.59:2021 Section 14 OTP System Advertisement Layer
     * 
     */
    namespace OTPSystemAdvertisementLayer {
        /**
         * @brief Layer length field offset
         * @details PDU octet offset for length field
         * 
         */
        constexpr pduLength_t LENGTHOFFSET = static_cast<pduLength_t>(sizeof(vector_t) + sizeof(pduLength_t));

        /**
         * @brief Expected value for vector field
         * 
         */
        const vector_t VECTOR = VECTOR_OTP_ADVERTISEMENT_SYSTEM_LIST;

        /**
         * @brief Expected value for reserved field
         * 
         */
        const reserved_t RESERVED = 0x0;

        /**
         * @brief Valid Ranges for layer
         * 
         */
        namespace RANGES {
            /**
             * @brief Allowed oclet size range of List field
             * 
             */
            inline const range_t ListSize = {0,200};
        }
    }
}

#endif // PDU_CONST_HPP
