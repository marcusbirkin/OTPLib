/**
 * @file        bugs.hpp
 * @brief       Workarounds for known bugs in compilers or Qt
 * @details     Part of OTPLib - A QT interface for E1.59
 * @authors     Marcus Birkin
 * @copyright   Copyright (C) 2020 Marcus Birkin
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
#ifndef BUGS_HPP
#define BUGS_HPP

/* Compiler error C2666 when using std::bitset and custom float class (64 bit)
https://developercommunity.visualstudio.com/content/problem/406329/compiler-error-c2666-when-using-stdbitset-and-cust.html
https://bugreports.qt.io/browse/QTBUG-72073
*/
#ifdef _MSC_VER
    #if _MSC_VER < 1920 // Visual Studio 2019 RTW (16.0)
        #define QT_NO_FLOAT16_OPERATORS
    #endif
#endif


#endif // BUGS_HPP
