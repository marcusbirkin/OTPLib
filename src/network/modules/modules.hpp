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

/**
 * @brief A set of properties of a Point, such as its position or rotation. 
 * The data structure of a Module is either defined by this standard, or it is manufacturer-specific.
 * 
 */
namespace OTP::MODULES {
    namespace STANDARD {
        /**
         * @brief Extract the Additional Fields from point details
         * 
         * @param standardModule Module ident
         * @param pointDetails Point details to extract from
         * @return Additional fields 
         */
        additional_t getAdditional(PDU::OTPModuleLayer::ident_t standardModule,
                pointDetails_t pointDetails);

        /**
         * @brief Extract the sample timestamp for the module from point details 
         * 
         * @param standardModule Module ident
         * @param pointDetails Point details to extract from
         * @return Sample timestamp 
         */
        timestamp_t getTimestamp(
                PDU::OTPModuleLayer::ident_t standardModule,
                pointDetails_t pointDetails);
    }
}

#endif // MODULES_HPP
