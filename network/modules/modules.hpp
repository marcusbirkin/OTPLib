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
#ifndef MODULES_HPP
#define MODULES_HPP

#include "modules_const.hpp"
#include "modules_types.hpp"
#include "../pdu/pdu_types.hpp"

namespace ACN::OTP::MODULES {
    namespace STANDARD {
        additional_t getAdditional(
                PDU::OTPModuleLayer::vector_t standardModule,
                pointDetails_t pointDetails);

        timestamp_t getTimestamp(
                PDU::OTPModuleLayer::vector_t standardModule,
                pointDetails_t pointDetails);
    }
}

#endif // MODULES_HPP
