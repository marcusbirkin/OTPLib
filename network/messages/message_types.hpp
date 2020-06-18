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
#ifndef MESSAGE_TYPES_HPP
#define MESSAGE_TYPES_HPP
#include <QString>
#include "../pdu/pdu_types.hpp"

namespace OTP::MESSAGES
{
    namespace  OTPTransformMessage {
        typedef OTP::PDU::OTPLayer::folio_t folio_t;
        typedef OTP::PDU::OTPLayer::page_t page_t;
    }

    namespace OTPModuleAdvertisementMessage {
        typedef OTP::PDU::OTPModuleAdvertisementLayer::item_t item_t;
        typedef OTP::PDU::OTPModuleAdvertisementLayer::list_t list_t;
        typedef OTP::PDU::OTPLayer::folio_t folio_t;
        typedef OTP::PDU::OTPLayer::page_t page_t;
    }

    namespace OTPNameAdvertisementMessage {
        typedef OTP::PDU::OTPNameAdvertisementLayer::item_t item_t;
        typedef OTP::PDU::OTPNameAdvertisementLayer::list_t list_t;
        typedef OTP::PDU::OTPLayer::folio_t folio_t;
        typedef OTP::PDU::OTPLayer::page_t page_t;
    }

    namespace OTPSystemAdvertisementMessage {
        typedef OTP::PDU::OTPSystemAdvertisementLayer::item_t item_t;
        typedef OTP::PDU::OTPSystemAdvertisementLayer::list_t list_t;
        typedef OTP::PDU::OTPLayer::folio_t folio_t;
        typedef OTP::PDU::OTPLayer::page_t page_t;
    }
}

#endif // MESSAGE_TYPES_HPP
