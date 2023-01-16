/**
 * @file        merger.cpp
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
#include "merger.hpp"

using namespace OTP;
Merger::Merger(system_t system, Container *parent)
    : QThread(parent),
      running(true),
      system(system),
      dirty(true)
{
    QThread::setObjectName(QString("Merger system %1").arg(system));
    QThread::start();
}

Merger::~Merger()
{
    running = false;
    wait();
}

void Merger::run()
{
    qDebug() << parent()->parent() << "Started merger thread system" << system;

    while (running)
    {
        dirty.waitFor(std::chrono::seconds(1));
        doMerge();
        dirty = false;
    }

    qDebug() << parent()->parent() << "Stopping merger thread system" << system;
}

void Merger::quit()
{
    running = false;
}

void Merger::setDirty()
{
    dirty = true;
}

void Merger::doMerge()
{
    // Winning source for each address, based on priority
    const auto &componentMap = parent()->componentMap;
    const auto &addressMap = parent()->addressMap;
    auto &winningSources = parent()->winningSources;

    for (auto component = componentMap.cbegin(); component != componentMap.cend(); component++)
    {
        if (!running) return;

        QMutexLocker addressLock(&parent()->addressMapMutex);
        cid_t cid = component.key();
        const auto groups = addressMap.value(cid).value(system);
        for (auto group = groups.cbegin(); group != groups.cend(); group++)
        {
            if (!running) return;

            for (auto point = group->cbegin(); point != group->cend(); point++)
            {
                if (!running) return;

                auto address = address_t(system, group.key(), point.key());

                auto pdA = addressMap[winningSources[address]][address.system][address.group][address.point];
                const auto pdB = addressMap[cid][address.system][address.group][address.point];

                if (!pdB || pdB->isExpired()) continue;
                if (!pdA || pdA->isExpired() || pdB->getPriority() > pdA->getPriority())
                {
                    winningSources[address] = cid;
                    pdA = pdB;
                }
            }
        }
    }
}
