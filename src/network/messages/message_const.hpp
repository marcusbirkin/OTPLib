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

namespace OTP::MESSAGES
{
    /*
     * 6.3 Length
     */

    namespace OTPTransformMessage {
        namespace RANGES {
            inline const range_t MESSAGE_SIZE = {134, 1472};
        }
    }

    namespace OTPSystemAdvertisementMessage {
        namespace RANGES {
            inline const range_t MESSAGE_SIZE = {96, 296};
        }
    }

    namespace OTPNameAdvertisementMessage {
        namespace RANGES {
            inline const range_t MESSAGE_SIZE = {96, 1461};
        }
    }

    namespace OTPModuleAdvertisementMessage {
        namespace RANGES {
            inline const range_t MESSAGE_SIZE = {96, 1471};
        }
    }
}

#endif // MESSAGE_CONST_HPP
