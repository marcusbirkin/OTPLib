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

#ifndef EVENTFLAG_H
#define EVENTFLAG_H

#include <atomic>
#include <condition_variable>
#include <mutex>

/**
 * @brief Thread safe event flag for synchronisation and notifications
 */
class EventFlag
{
public:
    EventFlag();
    explicit EventFlag(bool initalValue);
    EventFlag(EventFlag&) = delete;
    EventFlag(const EventFlag&) = delete;
    void operator=(EventFlag&) = delete;
    void operator=(const EventFlag&) = delete;

    /**
     * @brief Set flag value
     * @param set New flag value
     */
    void operator=(bool set);

    /**
     * @brief Get flag value
     * @return Current state of flag
     */
    operator bool() const;

    /**
     * @brief Wait for the flag to be set
     */
    void wait() const { wait(true); }

    /**
     * @brief Wait for the flag to be set to specfic state
     * @param state State to wait for
     */
    void wait(bool state) const;

    /**
     * @brief Wait for the flag to be set, or timeout
     * @param timeout Timeout before giving up on flag
     * @return flag value, will return false if timeout occurred
     */
    bool waitFor(std::chrono::milliseconds timeout) const { return waitFor(true, timeout); }

    /**
     * @brief Wait for the flag to be set to specfic state, or timeout
     * @param state State to wait for
     * @param timeout Timeout before giving up on flag
     * @return flag value, will return the inverse of 'state' on timeout
     */
    bool waitFor(bool state, std::chrono::milliseconds timeout) const;

private:
    std::atomic_bool flag;
    mutable std::condition_variable cv;
    mutable std::mutex mutex;
};

#endif // EVENTFLAG_H
