/*
    OTPLib
    A QT interface for E1.59  (Entertainment  Technology  Object  Transform  Protocol  (OTP)) 
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
#ifndef MESSAGE_CONST_HPP
#define MESSAGE_CONST_HPP

#include "message_types.hpp"
#include "../../const.hpp"
#include "../../enums.hpp"
#include "../pdu/pdu_const.hpp"

namespace ACN::OTP::MESSAGES
{
    /*
     * 7.1 Flags & Length
     */

    namespace OTPTransformMessage {
        namespace RANGES {
            const range_t MESSAGE_SIZE = {157, 1472};
        }
    }

    namespace OTPModuleAdvertisementMessage {
        namespace RANGES {
            const range_t MESSAGE_SIZE = {113, 1469};
        }
    }

    namespace OTPNameAdvertisementMessage {
        namespace RANGES {
            const range_t MESSAGE_SIZE = {110, 1436};
        }
    }

    namespace OTPSystemAdvertisementMessage {
        namespace RANGES {
            const range_t MESSAGE_SIZE = {110, 310};
        }
    }
}

#endif // MESSAGE_CONST_HPP
