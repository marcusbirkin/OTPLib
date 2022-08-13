/*
    OTPLib
    A QT interface for E1.59 (Entertainment Technology Object Transform Protocol (OTP))
    Copyright (C) 2022 Marcus Birkin

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

#include "eventflag.hpp"
#include <chrono>

EventFlag::EventFlag()
    : EventFlag(false)
{}

EventFlag::EventFlag(bool initalValue)
    : flag(initalValue)
{}

void EventFlag::operator=(bool set)
{
    std::lock_guard lk(mutex);
    flag = set;
    cv.notify_all();
}

EventFlag::operator bool() const
{
    std::lock_guard lk(mutex);
    return flag;
}

void EventFlag::wait(bool state) const
{
    std::unique_lock lk(mutex);
    return cv.wait(lk, [state, this]{ return flag == state; });
}

bool EventFlag::waitFor(bool state, std::chrono::milliseconds timeout) const
{
    std::unique_lock lk(mutex);
    return cv.wait_for(lk, timeout, [state, this]{ return flag == state; });
}
