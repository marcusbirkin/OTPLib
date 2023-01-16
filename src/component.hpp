/**
 * @file        component.hpp
 * @brief       Common functions inhearited by Producers and Consumers components
 * @details     Part of OTPLib - A QT interface for E1.59
 * @authors     Marcus Birkin
 * @copyright   Copyright (C) 2020 Marcus Birkin
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
#ifndef COMPONENT_H
#define COMPONENT_H

#include <QObject>
#include "container.hpp"
#include "socket.hpp"
#include "types.hpp"
#include "network/modules/modules.hpp"
#include <QNetworkInterface>
#include <QAbstractSocket>

#if defined MAKE_OTP_LIB
    /**
     * @brief Export symbol as shared library
     * 
     */
    #define OTP_LIB_EXPORT Q_DECL_EXPORT
#else
    /**
     * @brief Import symbol from shared library
     * 
     */
    #define OTP_LIB_EXPORT Q_DECL_IMPORT
#endif

namespace OTP
{
    /**
     * @brief Common functions inhearited by Producers and Consumers components
     */
    class OTP_LIB_EXPORT Component : public QObject
    {
        Q_OBJECT
    protected:
        /**
         * @brief Construct a new Component object
         * 
         * @param iface Network interface to bind to
         * @param transport Network transport to use
         * @param CID Component IDentifier
         * @param name Human readable name
         * @param parent Parent object
         */
        explicit Component(
                QNetworkInterface iface,
                QAbstractSocket::NetworkLayerProtocol transport,
                cid_t CID,
                name_t name,
                QObject *parent = nullptr);

    /* OTP Network */
    public:
        /**
         * @brief Clear local copy of network
         * @details Clear all knowlage of any components and there objects
         * 
         */
        void ClearOTPMap();
    protected:
        /**
         * @internal
         * @brief Primary container for details on all things relating to the OTP Network
         * 
         */
        std::unique_ptr<Container> otpNetwork;

    /* Network Interface */
    public:
        /**
         * @brief Get the network interface assigned to component
         * 
         * @return Network interface 
         */
        QNetworkInterface getNetworkInterface() const { return iface; }

        /**
         * @brief Set the network interface assigned to component
         * 
         * @param value New network interface
         */
        void setNetworkInterface(QNetworkInterface value);

        /**
         * @brief Get the network transport(s) assigned to component
         * 
         * @return Network transport(s)
         */
        QAbstractSocket::NetworkLayerProtocol getNetworkTransport() const { return transport; }

        /**
         * @brief Set the network transport(s) assigned to component
         * @details Used to set IPv4 and/or IPv6
         * 
         * @param transport New network transport
         */
        void setNetworkTransport(QAbstractSocket::NetworkLayerProtocol transport);

        /**
         * @brief Get the current socket state of the network interface
         * @details It is expected in normal operation for this to return QAbstractSocket::BoundState
         * 
         * @param transport Specific transport to query
         * @return Socket state
         */
        QAbstractSocket::SocketState getNetworkinterfaceState(QAbstractSocket::NetworkLayerProtocol transport) const;

    signals:
        /**
         * @brief Emitted when the container uses a new network interface
         * 
         */
        void newNetworkInterface(QNetworkInterface);

        /**
         * @brief Emitted when the container uses a new network transport
         * 
         */
        void newNetworkTransport(QAbstractSocket::NetworkLayerProtocol);

        /**
         * @brief Emitted when the container network socket has a new state
         * 
         */
        void stateChangedNetworkInterface(QAbstractSocket::SocketState);

    private slots:
        /**
         * @brief Emitted when the container receives a new network datagram
         * 
         */
        virtual void newDatagram(QNetworkDatagram);

    private:
        /**
         * @brief Processes new OTP Transform Message datagrams
         * 
         * @param datagram Datagram to process
         * @return Success status
         */
        virtual bool receiveOTPTransformMessage(const QNetworkDatagram &datagram) = 0;

        /**
         * @brief Processes new OTP Module Advertisement Message datagrams
         * 
         * @param datagram Datagram to process
         * @return Success status
         */
        virtual bool receiveOTPModuleAdvertisementMessage(const QNetworkDatagram &datagram) = 0;

        /**
         * @brief Processes new OTP Name Advertisement Message datagrams
         * 
         * @param datagram Datagram to process
         * @return Success status
         */
        virtual bool receiveOTPNameAdvertisementMessage(const QNetworkDatagram &datagram) = 0;

        /**
         * @brief Processes new OTP System Advertisement Message datagrams
         * 
         * @param datagram Datagram to process
         * @return Success status
         */
        virtual bool receiveOTPSystemAdvertisementMessage(const QNetworkDatagram &datagram) = 0;

    protected:
        /**
         * @internal
         * @brief Temporary storage when assemblying a complete OTP folio 
         * 
         */
        folioMap_t folioMap;

    protected:
        /**
         * @internal
         * @brief (Re)Setup the network listener sockets
         * 
         */
        virtual void setupListener();

        /**
         * @internal
         * @brief Interface used by the component
         * 
         */
        QNetworkInterface iface;

        /**
         * @internal
         * @brief Network transport used by the component
         * 
         */
        QAbstractSocket::NetworkLayerProtocol transport;

        /**
         * @internal
         * @brief List of Qt signal/stop socket listener connections used by the component
         * 
         */
        QList<QMetaObject::Connection> listenerConnections;

        /**
         * @internal
         * @brief Used to protect SocketManager objects container list
         * 
         */
        mutable QMutex socketsMutex;

        /**
         * @internal
         * @brief List of SocketManager objects used by the component
         * @details socketsMutex should be locked before modifying
         * 
         */
        QMap<QAbstractSocket::NetworkLayerProtocol, QSharedPointer<SocketManager>> sockets;

    /* Local CID */
    public:
        /**
         * @brief Get the Component IDentifier of the local component
         * 
         * @return Component IDentifier 
         */
        cid_t getLocalCID() const { return CID; }

        /**
         * @brief Set the Component IDentifier of the local component
         * 
         * @param value net Component IDentifier 
         */
        void setLocalCID(OTP::cid_t value);

    signals:
        /**
         * @brief Emiited when the local component is given a new Component IDentifier
         * 
         */
        void newLocalCID(OTP::cid_t);

    protected:
        /**
         * @internal
         * @brief Local CID
         * 
         */
        cid_t CID;

    /* Local Name */
    public:
        /**
         * @brief Get the human readable name of the local component
         * 
         * @return Human readable name
         */
        name_t getLocalName() const { return name; }

        /**
         * @brief Set the human readable name of the local component
         * 
         * @param name New name
         */
        void setLocalName(OTP::name_t name);

    signals:
        /**
         * @brief Emiited when the local component has a new human readable name
         * 
         * @param name Components updated name
         */
        void newLocalName(OTP::name_t name);

    protected:
        /**
         * @internal
         * @brief Local component name
         * 
         */
        name_t name;

    /* Local Systems */
    public:
        /**
         * @brief Get a list of systems for the local component
         * 
         * @return System list
         */
        QList<system_t> getLocalSystems() const;

        /**
         * @brief Add a system to the local component
         * 
         * @param system System to add
         */
        virtual void addLocalSystem(system_t system);

        /**
         * @brief Remove a system from the local component
         * 
         * @param system System to remove
         */
        virtual void removeLocalSystem(system_t system);

    signals:
        /**
         * @brief Emiited when the local component has a new system
         * 
         */
        void newLocalSystem(OTP::system_t);

        /**
         * @brief Emiited when the local component removes a system
         * 
         */
        void removedLocalSystem(OTP::system_t);

    /* Local Modules */
    public:
        /**
         * @brief Get a list of modules for the local component
         * 
         * @return List of modules 
         */
        moduleList_t getLocalModules() const;

        /**
         * @brief Add a module to the local component
         * 
         * @param module Module to add
         */
        void addLocalModule(moduleList_t::value_type module);

        /**
         * @brief Remove a system from the local component
         * 
         * @param module Module to remove
         */
        void removeLocalModule(moduleList_t::value_type module);

    signals:
        /**
         * @brief Emiited when the local component has a new module
         * 
         */
        void newLocalModule(moduleList_t::value_type);

        /**
         * @brief Emiited when the local component removes a module
         * 
         */
        void removedLocalModule(moduleList_t::value_type);

    /* Components */
    public:
        /**
         * @brief Get a list of known components
         * 
         * @return List of Component IDentifiers
         */
        QList<cid_t> getComponents() const { return otpNetwork->getComponentList(); }

        /**
         * @brief Get details on component
         * 
         * @param cid Component IDentifier to query
         * @return Component details
         */
        component_t getComponent(cid_t cid) const { return otpNetwork->getComponent(cid); }

        /**
         * @brief Is the component expired?
         * @details Was the last message from this component beyond the timeout?
         * 
         * @param cid Component IDentifier to query
         * @return true Component expired
         * @return false Component active
         */
        bool isComponentExpired(cid_t cid) const;

    signals:
        /**
         * @brief Emitted when a new component is found
         * 
         */
        void newComponent(OTP::cid_t);

        /**
         * @brief Emitted when a new component is removed
         * 
         */
        void removedComponent(OTP::cid_t);

        /**
         * @brief Emitted when the components name is updated
         * 
         */
        void updatedComponent(const OTP::cid_t&, const OTP::name_t&);

        /**
         * @brief Emitted when the components IP Address is updated
         * 
         */
        void updatedComponent(const OTP::cid_t&, const QHostAddress&);

        /**
         * @brief Emitted when the components module list is updated
         * 
         */
        void updatedComponent(const OTP::cid_t&, const OTP::moduleList_t &);

        /**
         * @brief Emitted if the components role is changed
         * @details Components are assume to be consumers unless proven otherwise
         * This would be emitted when a component is proven to be a producer
         * 
         */
        void updatedComponent(const OTP::cid_t&, OTP::component_t::type_t);

    /* Systems */
    public:
        /**
         * @brief Get a list of known systems
         * 
         * @return System list
         */
        QList<system_t> getSystems() const { return otpNetwork->getSystemList(); }

        /**
         * @brief Get list of systems belonging to a component
         * 
         * @param cid Component IDentifier to query
         * @return System list
         */
        QList<system_t> getSystems(cid_t cid) const { return otpNetwork->getSystemList(cid); }

    signals:
        /**
         * @brief Emitted when a new system is added to a component
         * 
         */
        void newSystem(OTP::cid_t, OTP::system_t);

        /**
         * @brief Emitted when a system is removed from a component
         * 
         */
        void removedSystem(OTP::cid_t, OTP::system_t);

    /* Groups */
    public:
        /**
         * @brief Get a list of known groups for a system
         * 
         * @param system System to query
         * @return Group list
         */
        QList<group_t> getGroups(system_t system) const { return otpNetwork->getGroupList(system); }

        /**
         * @brief Get a list of known groups for a specfic component's system
         * 
         * @param cid Component IDentifier to query
         * @param system System to query
         * @return Group list
         */
        QList<group_t> getGroups(cid_t cid, system_t system) const { return otpNetwork->getGroupList(cid, system); }

        /**
         * @brief Add a new group for a component
         * 
         * @param cid Component IDentifier
         * @param system System containing the group
         * @param group New group
         */
        void addGroup(cid_t cid, system_t system, group_t group) {  otpNetwork->addGroup(cid, system, group); }

        /**
         * @brief Is the group expired for this component
         * @details Was the last message from this group, for a specfic component, beyond the timeout?
         * 
         * @param cid Component IDentifier containing the group
         * @param system System containing the group
         * @param group Group to query
         * @return true Group expired
         * @return false Group active
         */
        bool isGroupExpired(cid_t cid, system_t system, group_t group) const;

        /**
         * @brief Is the group expired
         * @details Was the last message from this group beyond the timeout?
         * 
         * @param system System containing the group
         * @param group Group to query
         * @return true Group expired
         * @return false Group active
         */
        bool isGroupExpired(system_t system, group_t group) const
            { return isGroupExpired(cid_t(), system, group); }

    signals:
        /**
         * @brief Emitted when a new group is added to a component
         * 
         */
        void newGroup(OTP::cid_t, OTP::system_t, OTP::group_t);

        /**
         * @brief Emitted when a group is removed from a component
         * 
         */
        void removedGroup(OTP::cid_t, OTP::system_t, OTP::group_t);

    /* Points */
    public:
        /**
         * @brief Get a list of known points for a system/group
         * 
         * @param system System containing the group
         * @param group Group to query
         * @return Point list
         */
        QList<point_t> getPoints(system_t system, group_t group) const { return otpNetwork->getPointList(system, group); }

        /**
         * @brief Get a list of known points for a system/group, for a specfic component
         * 
         * @param cid Component IDentifier containing the group
         * @param system System containing the group
         * @param group Group to query
         * @return Point list
         */
        QList<point_t> getPoints(cid_t cid, system_t system, group_t group) const { return otpNetwork->getPointList(cid, system, group); }

        /**
         * @brief Is the point address known?
         * 
         * @param address Point address
         * @return true Point is known
         * @return false Unknown point
         */
        bool isPointValid(address_t address) const;

        /**
         * @brief Is the point address known to a specfic component?
         * 
         * @param cid Component IDentifier to query
         * @param address Point address
         * @return true Point is known for this component
         * @return false  Unknown point for this component
         */
        bool isPointValid(cid_t cid, address_t address) const;

        /**
         * @brief Get the point name assigned from a specfic component
         * 
         * @param cid Component IDentifier
         * @param address Point address to query
         * @return Point name 
         */
        QString getPointName(cid_t cid, address_t address) const;

        /**
         * @brief Get the point name assigned from a specfic component
         * 
         * @param cid Component IDentifier
         * @param system System containing the point
         * @param group Group containing the point
         * @param point Point to query
         * @return Point name  
         */
        QString getPointName(cid_t cid, system_t system, group_t group, point_t point) const
            { return getPointName(cid, address_t(system, group, point)); }

        /**
         * @brief Get the point name
         * 
         * @param address Point address to query
         * @return Point name 
         */
        QString getPointName(address_t address) const
            { return getPointName(cid_t(), address); }

        /**
         * @brief Get the point name
         * 
         * @param system System containing the point
         * @param group Group containing the point
         * @param point Point to query
         * @return Point name  
         */
        QString getPointName(system_t system, group_t group, point_t point) const
            { return getPointName(address_t(system, group, point)); }

        /**
         * @brief When was this point last seen, from a specfic component 
         * 
         * @param cid Component IDentifier
         * @param address Point address to query
         * 
         * @return Time of last activity for this point 
         */
        QDateTime getPointLastSeen(cid_t cid, address_t address) const;

        /**
         * @brief When was this point last seen, from a specfic component 
         * 
         * @param cid Component IDentifier
         * @param system System containing the point
         * @param group Group containing the point
         * @param point Point to query
         * @return Time of last activity for this point  
         */
        QDateTime getPointLastSeen(cid_t cid, system_t system, group_t group, point_t point) const
            { return getPointLastSeen(cid, address_t(system, group, point)); }

        /**
         * @brief When was this point last seen
         * 
         * @param address Point address to query
         * @return Time of last activity for this point  
         */
        QDateTime getPointLastSeen(address_t address) const
            { return getPointLastSeen(cid_t(), address); }

        /**
         * @brief Get the Point Last Seen object
         * 
         * @param system System containing the point
         * @param group Group containing the point
         * @param point Point to query
         * @return Time of last activity for this point   
         */
        QDateTime getPointLastSeen(system_t system, group_t group, point_t point) const
            { return getPointLastSeen(address_t(system, group, point)); }

        /**
         * @brief Is the point expired, from a specfic component 
         * @details Was the last message from, a specfic component, for this point beyond the timeout?
         * 
         * @param cid Component IDentifier
         * @param address Point address to query
         * @return true Point expired
         * @return false Point active
         */
        bool isPointExpired(cid_t cid, address_t address) const;

        /**
         * @brief Is the point expired, from a specfic component 
         * @details Was the last message from, a specfic component, for this point beyond the timeout?
         * 
         * @param cid Component IDentifier
         * @param system System containing the point
         * @param group Group containing the point
         * @param point Point to query
         * @return true Point expired
         * @return false Point active
         */
        bool isPointExpired(cid_t cid, system_t system, group_t group, point_t point) const
            { return isPointExpired(cid, address_t(system, group, point)); }

        /**
         * @brief Is the point expired
         * @details Was the last message from this point beyond the timeout?
         * 
         * @param address Point address to query
         * @return true Point expired
         * @return false Point active
         */
        bool isPointExpired(address_t address) const
            { return isPointExpired(cid_t(), address); }

        /**
         * @brief Is the point expired
         * @details Was the last message from this point beyond the timeout?
         * 
         * @param system System containing the point
         * @param group Group containing the point
         * @param point Point to query
         * @return true Point expired
         * @return false Point active
         */
        bool isPointExpired(system_t system, group_t group, point_t point) const
            { return isPointExpired(address_t(system, group, point)); }

    signals:
        /**
         * @brief Emitted when a new point is added to a component
         * 
         */
        void newPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

        /**
         * @brief Emitted when a point is removed from a componet
         * 
         */
        void removedPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

        /**
         * @brief Emitted when a point updated
         * @details This includes any datagram received from a producer regarding this point
         * This would be used to trigger checking, for example, the points current position
         * 
         */
        void updatedPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

        /**
         * @brief Emitted when a point expires, and the data is no longer active
         * 
         */
        void expiredPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

    /* Addresses */
    public:
        /**
         * @brief Get a list of known addresses (system/group/point)
         * 
         * @return Address list
         */
        QList<address_t> getAddresses();

        /**
         * @brief Get a list of known addresses (system/group/point) for a specific system
         * 
         * @param system System to query
         * @return Address list
         */
        QList<address_t> getAddresses(system_t system);

        /**
         * @brief Get a list of known addresses (system/group/point) for a specific system and group
         * 
         * @param system System containing the group
         * @param group Group to query
         * @return QList<address_t> 
         */
        QList<address_t> getAddresses(system_t system, group_t group);

    /* Standard Modules */
    public:
        /**
         * @brief Helper function - Get the string value of a scale
         * 
         * @param scale Scale to convert
         * @param html Format for HTML?
         * @return String of scale 
         */
        QString getScaleString(MODULES::STANDARD::PositionModule_t::scale_t scale, bool html = false) const;

        /**
         * @brief Helper function - Get the string value of a unit
         * 
         * @param moduleValue Module value to convert
         * @param html Format for HTML?
         * @return String of unit 
         */
        QString getUnitString(MODULES::STANDARD::VALUES::moduleValue_t moduleValue, bool html = false) const;

        /**
         * @brief Helper function - Get the string value of a scaled unit
         * 
         * @param scale Scale to apply
         * @param module Module value to convert
         * @param html Format for HTML?
         * @return QString 
         */
        QString getUnitString(MODULES::STANDARD::PositionModule_t::scale_t scale, MODULES::STANDARD::VALUES::moduleValue_t module, bool html = false) const;

    }; // class Component
}; // namespace OTP

#endif // COMPONENT_H
