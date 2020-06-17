/*
    OTPLib
    A QT interface for E1.59 (Entertainment Technology Object Transform Protocol (OTP))
    Copyright (C) 2020 Marcus Birkin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#include "modules_types.hpp"
#include "modules_const.hpp"

namespace OTP::MODULES::STANDARD {

    RotationModule_t::rotation_t& operator+=(RotationModule_t::rotation_t& l, const RotationModule_t::rotation_t& r) {
        RotationModule_t::rotation_t::type min = VALUES::RANGES::getRange(VALUES::ROTATION).getMin();
        RotationModule_t::rotation_t::type max = VALUES::RANGES::getRange(VALUES::ROTATION).getMax();
        RotationModule_t::rotation_t::type size = max - min + 1;

        l.data += r.data;

        if (l.data < min)
            l.data += size * ((min - l.data) / size + 1);

        l.data = min + (l.data - min) % size;
        return l;
    }
}
