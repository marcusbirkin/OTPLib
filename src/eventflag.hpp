/**
 * @file        eventflag.hpp
 * @brief       Thread safe event flag for synchronisation and notifications
 * @details     Part of OTPLib - A QT interface for E1.59
 * @authors     Marcus Birkin
 * @copyright   Copyright (C) 2022 Marcus Birkin
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

#ifndef EVENTFLAG_H
#define EVENTFLAG_H

#include <atomic>
#include <condition_variable>
#include <mutex>

/**
 * @internal
 * @brief Thread safe event flag for synchronisation and notifications
 */
class EventFlag
{
public:
    /**
     * @brief Construct a new Event Flag object
     * 
     */
    EventFlag();

    /**
     * @brief Construct a new Event Flag object
     * 
     * @param initalValue Inital state of event flag
     */
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
    operator bool();

    /**
     * @brief Wait for the flag to be set
     */
    void wait() { wait(true); }

    /**
     * @brief Wait for the flag to be set to specfic state
     * @param state State to wait for
     */
    void wait(bool state);

    /**
     * @brief Wait for the flag to be set, or timeout
     * @param timeout Timeout before giving up on flag
     * @return flag value, will return false if timeout occurred
     */
    bool waitFor(std::chrono::milliseconds timeout) { return waitFor(true, timeout); }

    /**
     * @brief Wait for the flag to be set to specfic state, or timeout
     * @param state State to wait for
     * @param timeout Timeout before giving up on flag
     * @return flag value, will return the inverse of 'state' on timeout
     */
    bool waitFor(bool state, std::chrono::milliseconds timeout);

private:
    std::atomic_bool flag;
    std::condition_variable cv;
    std::mutex mutex;
};

#endif // EVENTFLAG_H
