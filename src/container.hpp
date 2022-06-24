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
#include "types.hpp"

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

        cid_t getWinningComponent(address_t address) const;

        void clearSystems();

        void addSystem(cid_t cid, system_t system);
        void removeSystem(cid_t cid, system_t system);
        QList<system_t> getSystemList() const;
        QList<system_t> getSystemList(cid_t cid) const;

        void addGroup(cid_t cid, system_t system, group_t group);
        void removeGroup(cid_t cid, system_t system, group_t group);
        QList<group_t> getGroupList(system_t system) const;
        QList<group_t> getGroupList(cid_t cid, system_t system) const;

        void addModule(cid_t cid, const component_s::ModuleList_t&);
        void addModule(cid_t cid, const component_s::ModuleItem_t&);
        void removeModule(cid_t cid, const component_s::ModuleList_t&);
        void removeModule(cid_t cid, const component_s::ModuleItem_t&);
        moduleList_t getModuleList(cid_t cid) const;

        void addPoint(cid_t cid, address_t address, priority_t priority = priority_t());
        void addPoint(cid_t cid, system_t system, group_t group, point_t point, priority_t priority = priority_t())
            { addPoint(cid, {system, group, point}, priority); }
        void removePoint(cid_t cid, address_t address);
        void removePoint(cid_t cid, system_t system, group_t group, point_t point)
            { removePoint(cid, {system, group, point}); }
        void movePoint(cid_t cid, address_t oldAddress, address_t newAddress);
        void movePoint(cid_t cid,
                       system_t oldSystem, group_t oldGroup, point_t oldPoint,
                       system_t newSystem, group_t newGroup, point_t newPoint)
            { movePoint(cid, {oldSystem, oldGroup, oldPoint}, {newSystem, newGroup, newPoint}); }
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
        void newComponent(OTP::cid_t);
        void newSystem(OTP::cid_t, OTP::system_t);
        void newGroup(OTP::cid_t, OTP::system_t, OTP::group_t);
        void newPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

        void updatedComponent(const OTP::cid_t&, const OTP::name_t&);
        void updatedComponent(const OTP::cid_t&, const QHostAddress&);
        void updatedComponent(const OTP::cid_t&, const OTP::moduleList_t&);
        void updatedComponent(const OTP::cid_t&, OTP::component_t::type_t);
        void updatedSystem(OTP::cid_t, OTP::system_t);
        void updatedGroup(OTP::cid_t, OTP::system_t, OTP::group_t);
        void updatedPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

        void removedComponent(OTP::cid_t);
        void removedSystem(OTP::cid_t, OTP::system_t);
        void removedGroup(OTP::cid_t, OTP::system_t, OTP::group_t);
        void expiredPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);
        void removedPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

    private slots:
        void pruneModuleList(OTP::cid_t cid);
        void pruneComponentList();

    private:
        addressMap_t addressMap;
        componentMap_t componentMap;
        QTimer componentTimeout;
        QMap<cid_t, std::shared_ptr<QTimer>> moduleListTimeoutMap;
        QMap<address_t, std::shared_ptr<QTimer>> pointTimeoutMap;
    };
}

#endif // CONTAINER_H
