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
#ifndef PDU_CONST_HPP
#define PDU_CONST_HPP

#include "../../const.hpp"
#include "pdu_types.hpp"

namespace ACN::OTP::PDU {

    const size_t NAME_LENGTH = 32;

    /* Section 5 OTP Root Layer */
    namespace OTPRootLayer {
        const ambleSize_t PREAMBLE_SIZE = 0x0010;
        const ambleSize_t POSTAMBLE_SIZE = 0x0000;
        const acnIdent_t ACN_PACKET_IDENT = QByteArrayLiteral("\x41\x53\x43\x2d\x45\x31\x2e\x31\x37\x00\x00\x00");
        const size_t ACN_PACKET_IDENT_LENGTH = 12;
        const flags_length_t::flags_t FLAGS = ACN::PDU::FLAGS_VECTOR | ACN::PDU::FLAGS_HEADER | ACN::PDU::FLAGS_DATA;
        const vector_t VECTOR = VECTOR_ROOT_OTP;
    }

    /* Section 7 OTP Layer */
    namespace OTPLayer {
        const flags_length_t::flags_t FLAGS = ACN::PDU::FLAGS_VECTOR | ACN::PDU::FLAGS_HEADER | ACN::PDU::FLAGS_DATA;
        const QList<vector_t> VECTOR = {
            VECTOR_OTP_TRANSFORM_MESSAGE,
            VECTOR_OTP_ADVERTISEMENT_MESSAGE};
        const protocol_t PROTOCOL_VERSION = 0x1;
        const options_t OPTIONS = 0x0;
        const reserved_t RESERVED = 0x0;
    }

    /* Section 8 OTP Transform Layer */
    namespace OTPTransformLayer {
        const flags_length_t::flags_t FLAGS = ACN::PDU::FLAGS_VECTOR | ACN::PDU::FLAGS_HEADER | ACN::PDU::FLAGS_DATA;
        const vector_t VECTOR = VECTOR_OTP_POINT;
        const options_t OPTIONS = 0x0;
        const reserved_t RESERVED = 0x0;
    }

    /* Section 9 OTP Point Layer */
    namespace OTPPointLayer {
        const flags_length_t::flags_t FLAGS = ACN::PDU::FLAGS_VECTOR | ACN::PDU::FLAGS_HEADER | ACN::PDU::FLAGS_DATA;
        const vector_t VECTOR = VECTOR_OTP_POINT;
        const options_t OPTIONS = 0x0;
        const reserved_t RESERVED = 0x0;
    }

    /* 10 OTP Module Layer */
    namespace OTPModuleLayer {
        const flags_length_t::flags_t FLAGS = ACN::PDU::FLAGS_VECTOR | ACN::PDU::FLAGS_HEADER | ACN::PDU::FLAGS_DATA;
    }

    /* 11 OTP Advertisement Layer */
    namespace OTPAdvertisementLayer {
        const flags_length_t::flags_t FLAGS = ACN::PDU::FLAGS_VECTOR | ACN::PDU::FLAGS_HEADER | ACN::PDU::FLAGS_DATA;
        const QList<vector_t> VECTOR = {
            VECTOR_OTP_ADVERTISEMENT_MODULE,
            VECTOR_OTP_ADVERTISEMENT_NAME,
            VECTOR_OTP_ADVERTISEMENT_SYSTEM};
        const reserved_t RESERVED = 0x0;
    }

    /* 12 OTP Module Advertisement Layer */
    namespace OTPModuleAdvertisementLayer {
        const flags_length_t::flags_t FLAGS = ACN::PDU::FLAGS_VECTOR | ACN::PDU::FLAGS_HEADER | ACN::PDU::FLAGS_DATA;
        const vector_t VECTOR = VECTOR_OTP_ADVERTISEMENT_MODULE_LIST;
        const reserved_t RESERVED = 0x0;
        namespace RANGES {
            const range_t ListSize = {4,1280};
        }
    }

    /* 13 OTP Name Advertisement Layer */
    namespace OTPNameAdvertisementLayer {
        const flags_length_t::flags_t FLAGS = ACN::PDU::FLAGS_VECTOR | ACN::PDU::FLAGS_HEADER | ACN::PDU::FLAGS_DATA;
        const vector_t VECTOR = VECTOR_OTP_ADVERTISEMENT_NAME_LIST;
        const reserved_t RESERVED = 0x0;
        namespace RANGES {
            const range_t ListSize = {0,1326};
        }
    }

    /* 14 OTP System Advertisement Layer */
    namespace OTPSystemAdvertisementLayer {
        const flags_length_t::flags_t FLAGS = ACN::PDU::FLAGS_VECTOR | ACN::PDU::FLAGS_HEADER | ACN::PDU::FLAGS_DATA;
        const vector_t VECTOR = VECTOR_OTP_ADVERTISEMENT_SYSTEM_LIST;
        const reserved_t RESERVED = 0x0;
        namespace RANGES {
            const range_t ListSize = {0,200};
        }
    }
}


#endif // PDU_CONST_HPP
