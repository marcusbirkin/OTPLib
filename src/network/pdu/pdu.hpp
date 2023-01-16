/**
 * @file        pdu.hpp
 * @details     Part of OTPLib - A QT interface for E1.59
 * @brief       Protocol Data Unit includes
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
#ifndef PDU_HPP
#define PDU_HPP

/**
 * @internal
 * @brief Protocol Data Unit
 * @details OTP Messages typically carry one or more Protocol Data Units, or PDUs. 
 * Each PDU contains 3 segments: Vector, Length, and Data.
 * 
 */
namespace OTP::PDU {}

#include "otp_layer.hpp"
#include "otp_transform_layer.hpp"
#include "otp_point_layer.hpp"
#include "otp_module_layer.hpp"
#include "otp_advertisement_layer.hpp"
#include "otp_module_advertisement_layer.hpp"
#include "otp_name_advertisement_layer.hpp"
#include "otp_system_advertisement_layer.hpp"

#endif // PDU_HPP
