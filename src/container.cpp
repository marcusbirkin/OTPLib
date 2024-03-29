/**
 * @file        container.cpp
 * @brief       Primary OTP Network container
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
#include "container.hpp"
#include "otp.hpp"
#include "merger.hpp"
#include <QTimer>
#include <QMutexLocker>

using namespace OTP;

Container::Container(QObject *parent) :
    QObject(parent)
{
    componentTimeout.start(OTP_COMPONENT_TIMEOUT);
    connect(&componentTimeout, &QTimer::timeout, this, &Container::pruneComponentList);
}

Container::~Container()
{
}

void Container::clearComponents()
{
    for (const auto &cid : getComponentList())
        removeComponent(cid);
}

void Container::addComponent(
        const cid_t &cid,
        const QHostAddress &IPAddr,
        const name_t &name,
        component_t::type_t type,
        const moduleList_t &list)
{
    bool existing = componentMap.contains(cid);
    if (!existing)
    {
        componentMap.insert(cid, component_t());
        qDebug() << parent() << "- New component" << cid << name.toString() << IPAddr;
        emit newComponent(cid);
    }

    if (!name.isNull() && (componentMap.value(cid).getName() != name))
    {
        componentMap[cid].setName(name);
        qDebug() << parent() << "- Updated component (Name)" << cid << name.toString() << IPAddr;
        emit updatedComponent(cid, name);
    }

    if (componentMap.value(cid).getIPAddr() != IPAddr)
    {
        componentMap[cid].setIPAddr(IPAddr);
        qDebug() << parent() << "- Updated component (IP)" << cid << name.toString() << IPAddr;
        emit updatedComponent(cid, IPAddr);
    }

    if (componentMap[cid].getType() != type)
    {
        componentMap[cid].setType(type);
        emit updatedComponent(cid, type);
    }

    if (!list.empty())
    {
        addModule(cid, list);
    }

    componentMap[cid].updateLastSeen();
}

void Container::removeComponent(const cid_t &cid)
{
    if (componentMap.contains(cid))
    {
        const auto name = componentMap.value(cid).getName().toString();
        const auto IPAddr = componentMap.value(cid).getIPAddr();
        componentMap.remove(cid);
        qDebug() << parent() << "- Removed component" << cid << name << IPAddr;
        emit removedComponent(cid);
    }
}

bool Container::changeComponentCID(cid_t oldCID, cid_t newCID)
{
    if (!componentMap.contains(oldCID) || componentMap.contains(newCID))
        return false;

    // Move
    componentMap[newCID] = std::move(componentMap[oldCID]);
    {
        QMutexLocker lock(&addressMapMutex);
        addressMap[newCID] = std::move(addressMap[oldCID]);
    }
    removeComponent(oldCID);

    qDebug() << parent() << "- Changed component CID" << oldCID << newCID;
    emit newComponent(newCID);

    return true;
}

QList<cid_t> Container::getComponentList() const
{
    return componentMap.keys();
}

component_t Container::getComponent(cid_t cid) const
{
    if (componentMap.contains(cid))
        return componentMap[cid];

    return component_t();
}

cid_t Container::getWinningComponent(address_t address) const
{
    return winningSources.value(address);
}

void Container::clearSystems()
{
    QMutexLocker lock(&addressMapMutex);
    addressMap.clear();
}

void Container::addSystem(cid_t cid, system_t system)
{
    if (!system.isValid()) return;

    if (!getSystemList(cid).contains(system))
    {
        {
            QMutexLocker lock(&addressMapMutex);
            addressMap[cid][system];
        }

        qDebug() << parent() << "- New system" << cid << system;
        emit newSystem(cid, system);
    }
}

void Container::removeSystem(cid_t cid, system_t system)
{
    QMutexLocker lock(&addressMapMutex);
    if (addressMap[cid].contains(system))
    {
        addressMap[cid].remove(system);
        lock.unlock();
        qDebug() << parent() << "- Removed system" << cid << system;
        emit removedSystem(cid, system);
    }

    mergerThreads.remove(system);
}

QList<system_t> Container::getSystemList() const
{
    QMutexLocker lock(&addressMapMutex);
    QList<system_t> ret;
    QHashIterator<cid_t, systemMap_t> i(addressMap);
    while (i.hasNext()) {
        i.next();
        ret << getSystemList(i.key());
    }

    std::sort(ret.begin(),ret.end());
    ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
    return ret;
}

QList<system_t> Container::getSystemList(cid_t cid) const
{
    QMutexLocker lock(&addressMapMutex);
    if (!addressMap.contains(cid)) return QList<system_t>();

    QList<system_t> ret = addressMap[cid].keys();
    std::sort(ret.begin(),ret.end());
    return ret;
}

void Container::setSystemDirty(system_t system)
{
    if (!mergerThreads.contains(system))
        mergerThreads[system] = std::make_shared<Merger>(system, this);

    mergerThreads.value(system)->setDirty();
}

void Container::addGroup(cid_t cid, system_t system, group_t group)
{
    if (!group.isValid()) return;

    addSystem(cid, system);
    bool existing = getGroupList(cid, system).contains(group);
    bool updated = getGroupList(system).contains(group);
    if (!existing)
    {
        {
            QMutexLocker lock(&addressMapMutex);
            addressMap[cid][system][group] = pointMap_t();
        }
        if (updated)
        {
            // New to this CID
            qDebug() << parent() << "- Updated group" << cid << system << group;
            emit updatedGroup(cid, system, group);
        } else {
            qDebug() << parent() << "- New group" << cid << system << group;
            emit newGroup(cid, system, group);
        }
    }
}

void Container::removeGroup(cid_t cid, system_t system, group_t group)
{
    if (getGroupList(cid, system).contains(group))
    {
        {
            QMutexLocker lock(&addressMapMutex);
            addressMap[cid][system].remove(group);
        }
        qDebug() << parent() << "- Removed Group" << cid << system << group;
        emit removedGroup(cid, system, group);
    }
}

QList<group_t> Container::getGroupList(system_t system) const
{
    QList<group_t> ret;
    QHashIterator<cid_t, systemMap_t> i(addressMap);
    while (i.hasNext()) {
        i.next();
        ret << getGroupList(i.key(), system);
    }

    std::sort(ret.begin(),ret.end());
    ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
    return ret;
}

QList<group_t> Container::getGroupList(cid_t cid, system_t system) const
{
    QMutexLocker lock(&addressMapMutex);
    if (!addressMap.contains(cid)) return QList<group_t>();
    if (!addressMap[cid].contains(system)) return QList<group_t>();

    QList<group_t> ret = addressMap[cid][system].keys();
    std::sort(ret.begin(),ret.end());
    return ret;
}

void Container::addModule(cid_t cid, const moduleList_t &list)
{
    for (const auto &item : list)
        addModule(cid, item);
}
void Container::addModule(cid_t cid, const component_t::ModuleItem_t &item)
{
    bool newItem = true;
    if (getModuleList(cid).contains(item))
        newItem = false;
    componentMap[cid].addModuleItem(item);
    if (newItem) {
        qDebug() << parent() << "- Added module" << item.ManufacturerID << item.ModuleNumber << cid;
        emit updatedComponent(cid, componentMap[cid].getModuleList());
    }

    if (!moduleListTimeoutMap.contains(cid)) {
        moduleListTimeoutMap[cid]= std::make_shared<QTimer>(this);
        moduleListTimeoutMap[cid]->setSingleShot(true);
        moduleListTimeoutMap[cid]->start(OTP_ADVERTISEMENT_TIMEOUT);
        connect(moduleListTimeoutMap[cid].get(), &QTimer::timeout,
                this, [this, cid]()
        {
            pruneModuleList(cid);
        });
    }

    pruneModuleList(cid);
}
void Container::removeModule(cid_t cid, const component_t::ModuleList_t &list)
{
    for (const auto &item : list)
        removeModule(cid, item);
}
void Container::removeModule(cid_t cid, const component_t::ModuleItem_t &item)
{
    if (getModuleList(cid).contains(item)) {
        componentMap[cid].removeModuleItem(item);
        qDebug() << parent() << "- Removed module" << item.ManufacturerID << item.ModuleNumber << cid;
        emit updatedComponent(cid, componentMap[cid].getModuleList());
    }
}
moduleList_t Container::getModuleList(cid_t cid) const
{
    return componentMap[cid].getModuleList();
}

void Container::addPoint(cid_t cid, address_t address, priority_t priority)
{
    if (!address.point.isValid()) return;
    if (!priority.isValid()) return;
    componentMap[cid].updateLastSeen();

    addGroup(cid, address.system, address.group);
    bool existing = getPointList(cid, address.system, address.group).contains(address.point);
    if (existing)
    {
        {
            QMutexLocker lock(&addressMapMutex);
            addressMap[cid][address.system][address.group][address.point]->updateLastSeen();
        }
        emit updatedPoint(cid, address.system, address.group, address.point);
    } else
    {
        auto newDetails = pointDetails_t(new pointDetails);
        {
            QMutexLocker lock(&addressMapMutex);
            addressMap[cid][address.system][address.group].insert(address.point, newDetails);
        }
        qDebug() << parent() << "- New point" << cid << address.system << address.group << address.point << "(Priority: " << priority << ")";
        emit newPoint(cid, address.system, address.group, address.point);
    }

    if (!pointTimeoutMap.contains(address)) {
        pointTimeoutMap[address] = std::make_shared<QTimer>(this);
        pointTimeoutMap[address]->setSingleShot(true);
        connect(pointTimeoutMap[address].get(), &QTimer::timeout, this,
            [this, cid, address]() {
                if(!isValid(address) || isExpired(cid, address))
                {
                    prunePointList(cid, address);
                }
            });
    }
    pointTimeoutMap[address]->start(OTP_TRANSFORM_DATA_LOSS_TIMEOUT); // (re)set the timeout
}

void Container::removePoint(cid_t cid, address_t address)
{
    if (!address.point.isValid()) return;
    if (!getPointList(address.system, address.group).contains(address.point)) return;

    {
        QMutexLocker lock(&addressMapMutex);
        addressMap[cid][address.system][address.group].remove(address.point);
    }
    qDebug() << parent() << "- Removed point" << cid << address.system << address.group << address.point;
    emit removedPoint(cid, address.system, address.group, address.point);
}

void Container::movePoint(cid_t cid, address_t oldAddress, address_t newAddress)
{
    if (!oldAddress.point.isValid()) return;
    if (!getPointList(oldAddress.system, oldAddress.group).contains(oldAddress.point)) return;

    if (!newAddress.point.isValid()) return;
    if (getPointList(newAddress.system, newAddress.group).contains(newAddress.point)) return;

    addPoint(cid, newAddress);
    {
        QMutexLocker lock(&addressMapMutex);
        addressMap[cid][newAddress.system][newAddress.group][newAddress.point] =
                std::move(addressMap[cid][oldAddress.system][oldAddress.group][oldAddress.point]);
    }
    removePoint(cid, oldAddress);

    qDebug() << parent() << "- Moved point" << cid
             << "From" << oldAddress.system << oldAddress.group << oldAddress.point
             << "To" << newAddress.system << newAddress.group << newAddress.point;
}

QList<point_t> Container::getPointList(system_t system, group_t group) const
{
    QList<point_t> ret;
    QHashIterator<cid_t, systemMap_t> i(addressMap);
    while (i.hasNext()) {
        i.next();
        ret << getPointList(i.key(), system, group);
    }

    std::sort(ret.begin(),ret.end());
    ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
    return ret;
}

QList<point_t> Container::getPointList(cid_t cid, system_t system, group_t group) const
{
    QMutexLocker lock(&addressMapMutex);

    if (!addressMap.contains(cid)) return QList<point_t>();
    if (!addressMap[cid].contains(system)) return QList<point_t>();
    if (!addressMap[cid][system].contains(group)) return QList<point_t>();

    QList<point_t> ret = addressMap[cid][system][group].keys();
    std::sort(ret.begin(),ret.end());
    return ret;
}

pointDetails_t Container::PointDetails(cid_t cid, address_t address)
{
    QMutexLocker lock(&addressMapMutex);
    return addressMap[cid][address.system][address.group][address.point];
}
pointDetails_t Container::PointDetails(cid_t cid, address_t address) const
{
    QMutexLocker lock(&addressMapMutex);
    if (addressMap[cid][address.system][address.group].contains(address.point))
        return addressMap[cid][address.system][address.group].value(address.point);
    else
        return std::make_shared<pointDetails>();
}

bool Container::isValid(const address_t address) const
{
    return getPointList(address.system, address.group).contains(address.point);
}

void Container::prunePointList(const cid_t &cid, address_t address)
{
    if(!isValid(address) || isExpired(cid, address))
    {
        qDebug() << parent() << "- Expired point" << cid << address.system << address.group << address.point;
        emit expiredPoint(cid, address.system, address.group, address.point);
    }
}

void Container::pruneModuleList(const cid_t &cid)
{
    bool itemsRemoved = false;
    for (const auto &item : componentMap.value(cid).getModuleList())
    {
        if (componentMap.value(cid).isExpired(item))
        {
            componentMap[cid].removeModuleItem(item);
            qDebug() << parent() << "- Expired module" << cid << componentMap[cid].getModuleString(item);
            itemsRemoved = true;
        }
    }

    if (itemsRemoved)
        emit updatedComponent(cid, componentMap.value(cid).getModuleList());

    moduleListTimeoutMap[cid]->start(OTP_ADVERTISEMENT_TIMEOUT); // (re)start the timer
}

void Container::pruneComponentList()
{
    const auto componentList = getComponentList();
    for (const auto &cid : componentList)
        if (componentMap[cid].isExpired()) {
            qDebug() << parent() << "- Expired Component" << cid;
            removeComponent(cid);
        }
}
