/**
 * @file        container.hpp
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
#ifndef CONTAINER_H
#define CONTAINER_H

#include <QObject>
#include <QMutex>
#include "types.hpp"

namespace OTP
{
    class Merger;
    
    /**
     * @internal
     * @brief Primary OTP Network container type
     * @details Contains details on all known components and objects 
     * 
     */
    class Container : public QObject
    {
        Q_OBJECT
        friend class Merger;
    public:
        /**
         * @brief Construct a new Container
         * 
         * @param parent Parent object
         */
        explicit Container(QObject *parent = nullptr);
        ~Container();

        /**
         * @brief Clear details on all known components
         * 
         */
        void clearComponents();

        /**
         * @brief Add a new component
         * @details If the component already exists, then it's updated
         * 
         * @param cid Component IDenifier
         * @param IPAddr Component IP address
         * @param name Component Human readable name
         * @param type Is this a producer or consumer?
         * @param moduleList List of known modules for this Component
         */
        void addComponent(
                const cid_t &cid,
                const QHostAddress &IPAddr,
                const name_t &name = name_t(),
                component_t::type_t type = component_t::consumer,
                const moduleList_t &moduleList = moduleList_t());
        /**
         * @brief Add a new consumer component
         * @details If the component already exists, then it's updated
         * 
         * @param cid Component IDenifier
         * @param IPAddr Component IP address
         * @param name Component Human readable name
         * @param moduleList List of known modules for this Component
         */
        void addComponent(
                const cid_t &cid,
                const QHostAddress &IPAddr,
                const name_t &name = name_t(),
                const moduleList_t &moduleList = moduleList_t())
            { addComponent(cid, IPAddr, name, component_t::consumer, moduleList); }

        /**
         * @brief Remove a component
         * 
         * @param cid Component IDenifier
         */
        void removeComponent(const cid_t &cid);

        /**
         * @brief Change the CID of a known component 
         * @details There may be test application uses where you need to change the CID but preserve all the objects
         * 
         * @param oldCID Existing Component IDenifier
         * @param newCID New Component IDenifier
         * @return true Change completed ok
         * @return false Change failed
         */
        bool changeComponentCID(cid_t oldCID, cid_t newCID);

        /**
         * @brief Get the of all known component cids
         * 
         * @return List of cids
         */
        QList<cid_t> getComponentList() const;

        /**
         * @brief Get details on known component
         * 
         * @param cid Component IDenifier to query
         * @return Component details 
         */
        component_t getComponent(cid_t cid) const;

        /**
         * @brief Get the winning component for a specific address
         * @details Winning component is based upon advertised priority level
         * 
         * @param address Address to query
         * @return Winning Component IDenifier
         */
        cid_t getWinningComponent(address_t address) const;

        /**
         * @brief Clear all known Systems for all known components
         * @details Also clears nested groups and points
         * 
         */
        void clearSystems();

        /**
         * @brief Add a system for a component
         * @details If the system already exists in the component, then it's updated
         * 
         * @param cid Component IDenifier
         * @param system New system
         */
        void addSystem(cid_t cid, system_t system);

        /**
         * @brief Remove a system from a component
         * 
         * @param cid Component IDenifier
         * @param system System to remove
         */
        void removeSystem(cid_t cid, system_t system);

        /**
         * @brief Get a list of all known systems
         * 
         * @return System list
         */
        QList<system_t> getSystemList() const;

        /**
         * @brief Get a list of all known systems for a specfic component
         * 
         * @param cid Component IDenifier
         * @return System list
         */
        QList<system_t> getSystemList(cid_t cid) const;

        /**
         * @brief Flag a system as dirty and in need of merging
         * 
         * @param system System to flag
         */
        void setSystemDirty(system_t system);

        /**
         * @brief Add a group for a component
         * @details If the group already exists in the component, then it's updated
         * If the system is new to the component, then addSystem() is called
         * 
         * @param cid Component IDenifier
         * @param system System containing the group
         * @param group New group
         */
        void addGroup(cid_t cid, system_t system, group_t group);

        /**
         * @brief Remove a group from a component
         * 
         * @param cid Component IDenifier
         * @param system System containing the group
         * @param group Group to remove
         */
        void removeGroup(cid_t cid, system_t system, group_t group);

        /**
         * @brief Get a list of all known systems for a specfic system
         * 
         * @param system System to query
         * @return Group list
         */
        QList<group_t> getGroupList(system_t system) const;

        /**
         * @brief Get a list of all known systems for a specfic components system
         * 
         * @param cid Component IDenifier
         * @param system System to query
         * @return Group list
         */
        QList<group_t> getGroupList(cid_t cid, system_t system) const;

        /**
         * @brief Add a multiple modules for a component
         * @details If the modules already exisit in the component, then it's updated
         * 
         * @param cid Component IDenifier
         * @param moduleList List of new Modules
         */
        void addModule(cid_t cid, const component_t::ModuleList_t& moduleList);

        /**
         * @brief Add a module for a component
         * @details If the module already exisits in the component, then it's updated
         * 
         * @param cid Component IDenifier
         * @param module New Module
         */
        void addModule(cid_t cid, const component_t::ModuleItem_t& module);

        /**
         * @brief Remove a multiple module from a component
         * 
         * @param cid Component IDenifier
         * @param moduleList List of Module to remove
         */
        void removeModule(cid_t cid, const component_t::ModuleList_t& moduleList) ;

        /**
         * @brief Remove a module from a component
         * 
         * @param cid Component IDenifier
         * @param module Module to remove
         */
        void removeModule(cid_t cid, const component_t::ModuleItem_t& module);

        /**
         * @brief Get a list of all known modules for a specfic components system
         * 
         * @param cid Component IDenifier
         * @return Module list
         */
        moduleList_t getModuleList(cid_t cid) const;

        /**
         * @brief Add a point for a component
         * @details If the module already exisits in the component, then it's updated
         * 
         * @param cid Component IDenifier
         * @param address Point address to add
         * @param priority Point priority
         */
        void addPoint(cid_t cid, address_t address, priority_t priority = priority_t());

        /**
         * @brief Add a point for a component
         * @details If the module already exisits in the component, then it's updated
         * 
         * @param cid Component IDenifier
         * @param system System of point to add
         * @param group Group of point to add
         * @param point Point to add
         * @param priority Point priority
         */
        void addPoint(cid_t cid, system_t system, group_t group, point_t point, priority_t priority = priority_t())
            { addPoint(cid, {system, group, point}, priority); }

        /**
         * @brief Remove a point from a component
         * 
         * @param cid Component IDenifier
         * @param address Address of point to remove
         */
        void removePoint(cid_t cid, address_t address);

        /**
         * @brief Remove a point from a component
         * 
         * @param cid Component IDenifier
         * @param system System of point to remove
         * @param group Group of point to remove
         * @param point Point to remove
         */
        void removePoint(cid_t cid, system_t system, group_t group, point_t point)
            { removePoint(cid, {system, group, point}); }

        /**
         * @brief Move a point from one address to another
         * @details All point details are moved
         * 
         * @param cid Component IDenifier
         * @param oldAddress Existing address of point
         * @param newAddress New address of point
         */
        void movePoint(cid_t cid, address_t oldAddress, address_t newAddress);

        /**
         * @brief Move a point from one address to another
         * @details All point details are moved
         * 
         * @param cid Component IDenifier
         * @param oldSystem Existing system of point
         * @param oldGroup Existing group of point
         * @param oldPoint Existing point
         * @param newSystem New sytem of point
         * @param newGroup New group of point
         * @param newPoint New point
         */
        void movePoint(cid_t cid,
                       system_t oldSystem, group_t oldGroup, point_t oldPoint,
                       system_t newSystem, group_t newGroup, point_t newPoint)
            { movePoint(cid, {oldSystem, oldGroup, oldPoint}, {newSystem, newGroup, newPoint}); }

        /**
         * @brief Get a list of all known point for a system's group
         * 
         * @param system System to query
         * @param group Group to query
         * @return Point list
         */
        QList<point_t> getPointList(system_t system, group_t group) const;

        /**
         * @brief Get a list of all known point for a specfic components system's group
         * 
         * @param cid Component IDenifier
         * @param system System to query
         * @param group Group to query
         * @return Point list
         */
        QList<point_t> getPointList(cid_t cid, system_t system, group_t group) const;

        /**
         * @brief Get details for point
         * 
         * @param cid Component IDenifier 
         * @param address Address of the point to query
         * @return Point details
         */
        pointDetails_t PointDetails(cid_t cid, address_t address);

        /**
         * @copydoc PointDetails()
         */
        pointDetails_t PointDetails(cid_t cid, address_t address) const;

        /**
         * @brief Get details for point
         * 
         * @param cid Component IDenifier 
         * @param system System of the point to query
         * @param group Group of the point to query
         * @param point Point to query
         * @return Point details
         */
        pointDetails_t PointDetails(cid_t cid, system_t system, group_t group, point_t point)
            { return PointDetails(cid, {system, group, point}); }

        /**
         * @brief Is the point valid (i.e. known)?
         * 
         * @param address Address of the point to query
         * @return true The point is known
         * @return false The point is unknown
         */
        bool isValid(address_t address) const;

        /**
         * @brief Is the point valid (i.e. known)?
         * 
         * @param system System of the point to query
         * @param group Group of the point to query
         * @param point Point to query
         * @return true The point is known
         * @return false The point is unknown
         */
        bool isValid(system_t system, group_t group, point_t point) const
            { return isValid(address_t{system, group, point}); }

        /**
         * @brief Has the point expired?
         * 
         * @param cid Component IDenifier 
         * @param address Address of the point to query
         * @return true Point has not expired
         * @return false Point has expired
         */
        bool isExpired(cid_t cid, address_t address) const
            { return PointDetails(cid, address)->isExpired(); }

        /**
         * @brief Has the point expired?
         * 
         * @param cid Component IDenifier 
         * @param system System of the point to query
         * @param group Group of the point to query
         * @param point Point to query
         * @return true Point has not expired
         * @return false Point has expired
         */
        bool isExpired(cid_t cid, system_t system, group_t group, point_t point) const
            { return isExpired(cid, {system, group, point}); }

    signals:
        /**
         * @brief Emitted when a new Component is discovered/added
         * 
         */
        void newComponent(OTP::cid_t);

        /**
         * @brief Emitted when a new System is discovered/added
         * 
         */
        void newSystem(OTP::cid_t, OTP::system_t);

        /**
         * @brief Emitted when a new Group is discovered/added
         * 
         */
        void newGroup(OTP::cid_t, OTP::system_t, OTP::group_t);

        /**
         * @brief Emitted when a new Point is discovered/added
         * 
         */
        void newPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

        /**
         * @brief Emitted when a Component has an updated name
         * 
         */
        void updatedComponent(const OTP::cid_t&, const OTP::name_t&);

        /**
         * @brief Emitted when a Component has an updated name host address
         * 
         */
        void updatedComponent(const OTP::cid_t&, const QHostAddress&);

        /**
         * @brief Emitted when a Component has an updated module list
         * 
         */
        void updatedComponent(const OTP::cid_t&, const OTP::moduleList_t&);

        /**
         * @brief Emitted when a Component changes type
         * @details Consumer to Producer, or Producer to Consumer
         * 
         */
        void updatedComponent(const OTP::cid_t&, OTP::component_t::type_t);

        /**
         * @brief Emitted when a new source for a system is added to the network
         * 
         */
        void updatedSystem(OTP::cid_t, OTP::system_t);

        /**
         * @brief Emitted when a new source for a group is added to the network
         * 
         */
        void updatedGroup(OTP::cid_t, OTP::system_t, OTP::group_t);

        /**
         * @brief Emitted when a new source for a point is added to the network
         * 
         */
        void updatedPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

        /**
         * @brief Emitted when a Component is removed from the network
         * 
         */
        void removedComponent(OTP::cid_t);

        /**
         * @brief Emitted when a System is removed from the network
         * 
         */
        void removedSystem(OTP::cid_t, OTP::system_t);

        /**
         * @brief Emitted when a Group is removed from the network
         * 
         */
        void removedGroup(OTP::cid_t, OTP::system_t, OTP::group_t);

        /**
         * @brief Emitted when a Point is expired from the network
         * 
         */
        void expiredPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

        /**
         * @brief Emitted when a Point is removed from the network
         * 
         */
        void removedPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

    private slots:
        /**
         * @brief Check for, and prune, expired points for specified component and address
         * 
         * @param cid CID of component to check
         * @param address Address to check
         */
        void prunePointList(const OTP::cid_t &cid, OTP::address_t address);

        /**
         * @brief Check for, and prune, expired modules for specified components
         * 
         * @param cid CID of component to check
         */
        void pruneModuleList(const OTP::cid_t &cid);

        /**
         * @brief Check for, and prune, expired components
         * @details Called regularly by QTimer componentTimeout
         */
        void pruneComponentList();

    private:
        /**
         * @brief Mutex to protect addressMap
         */
        mutable QMutex addressMapMutex;
        /**
         * @brief Container of point details indexed by CID, system, group, point
         */
        addressMap_t addressMap;

        /**
         * @brief Container of component details indexed by CID
         */
        componentMap_t componentMap;

        /**
         * @brief Regularly calls pruneComponentList()
         */
        QTimer componentTimeout;

        /**
         * @brief Container of QTimers to prune expired modules
         * @details Modules are pruned by pruneModuleList() called from lambda function created in addModule()
         */
        QMap<cid_t, std::shared_ptr<QTimer>> moduleListTimeoutMap;

        /**
         * @brief Container of QTimers to prune expired points
         * @details Points are pruned by prunePointList() called from lambda function created in addPoint()
         */
        QMap<address_t, std::shared_ptr<QTimer>> pointTimeoutMap;

        /**
         * @brief Container of Merger threads, to determine winning source for each address
         * @details winningSources is updated by this thread
         */
        QMap<system_t, std::shared_ptr<Merger>> mergerThreads;

        /**
         * @brief Container of winning component indexed by address
         * @details Updated by mergerThread
         */
        QHash<address_t, cid_t> winningSources;
    };
}

#endif // CONTAINER_H
