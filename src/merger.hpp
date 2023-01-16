/**
 * @file        merger.hpp
 * @brief       Merger thread to determine winning component for each address
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
#ifndef MERGER_HPP
#define MERGER_HPP

#include <QThread>
#include "container.hpp"
#include "eventflag.hpp"

namespace OTP
{
    /**
     * @internal
     * @brief Merger thread to determine winning component for each address
     * @details Winner is determined by source priority
     * Updates winningSource container in parent class
     */
    class Merger : public QThread
    {
        public:
            /**
             * @brief Construct a new merger
             * 
             * @param system System to merge
             * @param parent Parent container
             */
            explicit Merger(system_t system, Container *parent);
            ~Merger();

            /**
             * @brief Stop merger loop
             */
            void quit();

            /**
             * @brief Flag system as dirty, needing merging
             */
            void setDirty();

        protected:
            /**
             * @brief QThread entry point
             */
            void run() override final;

        private:
            inline Container *parent() const { return static_cast<Container*>(QThread::parent()); }
            std::atomic<bool> running;

            const system_t system;
            EventFlag dirty;

            /**
             * @brief Run a single merge for the system
             */
            void doMerge();
    };
}

#endif // MERGER_HPP
