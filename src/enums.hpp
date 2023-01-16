/**
 * @file        enums.hpp
 * @brief       Miscellaneous enumerations
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
 *  but WITHOUT ANYs WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef ENUMS_H
#define ENUMS_H

namespace OTP
{
    /**
     * @brief Component operating mode
     * 
     */
    enum mode_e
    {
        Producer, /**< A Producer is any network device transmitting OTP Transform Messages. */
        Consumer /**< A Consumer is the intended target of information from a Producer. */
    } ;
}

#endif // ENUMS_H
