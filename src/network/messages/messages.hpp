/**
 * @file        messages.hpp
 * @brief       Messages includes
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
#ifndef MESSAGES_HPP
#define MESSAGES_HPP

/**
 * @internal
 * @brief Messages
 * @details OTP Messages typically carry one or more Protocol Data Units, or PDUs.
 * 
 */
namespace OTP::MESSAGES {}

#include "otp_transform_message.hpp"
#include "otp_module_advertisement_message.hpp"
#include "otp_name_advertisement_message.hpp"
#include "otp_system_advertisement_message.hpp"

#endif // MESSAGES_HPP
