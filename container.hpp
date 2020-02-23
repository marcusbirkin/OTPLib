/*
    OTPLib
    A QT interface for E1.59 (Entertainment Technology Object Transform Protocol (OTP))
    Copyright (C) 2019  Marcus Birkin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef CONTAINER_H
#define CONTAINER_H

#include <QObject>
#include "otp.hpp"

namespace OTP
{
    class Container : public QObject
    {
        Q_OBJECT
    public:
        explicit Container(QObject *parent = nullptr);

        void clearComponents();

        void addComponent(
                const cid_t &cid,
                const QHostAddress &IPAddr,
                const name_t &name = name_t(),
                component_t::type_t type = component_t::consumer,
                const moduleList_t &list = moduleList_t());
        void addComponent(
                const cid_t &cid,
                const QHostAddress &IPAddr,
                const name_t &name = name_t(),
                const moduleList_t &list = moduleList_t())
            { addComponent(cid, IPAddr, name, component_t::consumer, list); }
        void removeComponent(const cid_t &cid);

        QList<cid_t> getComponentList() const;
        component_t getComponent(cid_t cid) const;

        void clearSystems();

        void addSystem(cid_t cid, system_t system);
        void removeSystem(cid_t cid, system_t system);
        QList<system_t> getSystemList() const;
        QList<system_t> getSystemList(cid_t cid) const;

        void addGroup(cid_t cid, system_t system, group_t group);
        void removeGroup(cid_t cid, system_t system, group_t group);
        QList<group_t> getGroupList(system_t system) const;
        QList<group_t> getGroupList(cid_t cid, system_t system) const;

        void addPoint(cid_t cid, address_t address, priority_t priority);
        void addPoint(cid_t cid, system_t system, group_t group, point_t point, priority_t priority)
            { addPoint(cid, {system, group, point}, priority); }
        void removePoint(cid_t cid, address_t address);
        void removePoint(cid_t cid, system_t system, group_t group, point_t point)
            { removePoint(cid, {system, group, point}); }
        QList<point_t> getPointList(system_t system, group_t group) const;
        QList<point_t> getPointList(cid_t cid, system_t system, group_t group) const;
        pointDetails_t PointDetails(cid_t cid, address_t address)
            { return addressMap[cid][address.system][address.group][address.point]; }
        pointDetails_t PointDetails(cid_t cid, address_t address) const
            { return addressMap[cid][address.system][address.group][address.point]; }
        pointDetails_t PointDetails(cid_t cid, system_t system, group_t group, point_t point)
            { return PointDetails(cid, {system, group, point}); }
        bool isValid(address_t address) const;
        bool isValid(system_t system, group_t group, point_t point) const
            { return isValid(address_t{system, group, point}); }
        bool isExpired(cid_t cid, address_t address) const
            { return PointDetails(cid, address)->isExpired(); }
        bool isExpired(cid_t cid, system_t system, group_t group, point_t point) const
            { return isExpired(cid, {system, group, point}); }

    signals:
        void newComponent(cid_t);
        void newSystem(cid_t, system_t);
        void newGroup(cid_t, system_t, group_t);
        void newPoint(cid_t, system_t, group_t, point_t);

        void updatedComponent(const cid_t&, const name_t&);
        void updatedComponent(const cid_t&, const QHostAddress&);
        void updatedComponent(const cid_t&, const moduleList_t&);
        void updatedComponent(const OTP::cid_t&, OTP::component_t::type_t);
        void updatedSystem(cid_t, system_t);
        void updatedGroup(cid_t, system_t, group_t);
        void updatedPoint(cid_t, system_t, group_t, point_t);

        void removedComponent(cid_t);
        void removedSystem(cid_t, system_t);
        void removedGroup(cid_t, system_t, group_t);
        void expiredPoint(cid_t, system_t, group_t, point_t);
        void removedPoint(cid_t, system_t, group_t, point_t);

    private slots:
        void pruneModuleList(cid_t cid);

    private:
        addressMap_t addressMap;
        componentMap_t componentMap;
        QMap<cid_t, std::shared_ptr<QTimer>> moduleListTimeoutMap;
        QMap<address_t, std::shared_ptr<QTimer>> pointTimeoutMap;
    };
}

#endif // CONTAINER_H
