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
#ifndef PDU_HPP
#define PDU_HPP

#include "otp_root_layer.hpp"
#include "otp_layer.hpp"
#include "otp_transform_layer.hpp"
#include "otp_point_layer.hpp"
#include "otp_module_layer.hpp"
#include "otp_advertisement_layer.hpp"
#include "otp_module_advertisement_layer.hpp"
#include "otp_name_advertisement_layer.hpp"
#include "otp_system_advertisement_layer.hpp"

namespace OTP::PDU {
    vector_t extractVector(PDUByteArray ba);
    flags_length_t::pduLength_t extractLength(PDUByteArray ba);
}

#endif // PDU_HPP
