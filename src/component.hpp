/*
    OTPLib
    A QT interface for E1.59 (Entertainment Technology Object Transform Protocol (OTP))
    Copyright (C) 2020 Marcus Birkin

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
    #define OTP_LIB_EXPORT Q_DECL_EXPORT
#else
    #define OTP_LIB_EXPORT Q_DECL_IMPORT
#endif

namespace OTP
{
    class OTP_LIB_EXPORT Component : public QObject
    {
        Q_OBJECT
    protected:
        explicit Component(
                QNetworkInterface iface,
                QAbstractSocket::NetworkLayerProtocol transport,
                cid_t CID,
                name_t name,
                QObject *parent = nullptr);

    /* OTP Network */
    public:
        /*
         * Clear local copy of network
         */
        void ClearOTPMap();
    protected:
        std::unique_ptr<Container> otpNetwork;

    /* Network Interface */
    public:
        QNetworkInterface getNetworkInterface() const { return iface; }
        void setNetworkInterface(QNetworkInterface value);
        QAbstractSocket::NetworkLayerProtocol getNetworkTransport() const { return transport; }
        void setNetworkTransport(QAbstractSocket::NetworkLayerProtocol transport);
        QAbstractSocket::SocketState getNetworkinterfaceState(QAbstractSocket::NetworkLayerProtocol transport) const
        {
            if (!sockets.contains(transport)) return QAbstractSocket::UnconnectedState;
            return sockets.value(transport)->state();
        }
    signals:
        void newNetworkInterface(QNetworkInterface);
        void newNetworkTransport(QAbstractSocket::NetworkLayerProtocol);
        void stateChangedNetworkInterface(QAbstractSocket::SocketState);

    private slots:
        virtual void newDatagram(QNetworkDatagram);

    private:
        virtual bool receiveOTPTransformMessage(const QNetworkDatagram &datagram) = 0;
        virtual bool receiveOTPModuleAdvertisementMessage(const QNetworkDatagram &datagram)  = 0;
        virtual bool receiveOTPNameAdvertisementMessage(const QNetworkDatagram &datagram) = 0;
        virtual bool receiveOTPSystemAdvertisementMessage(const QNetworkDatagram &datagram) = 0;

    protected:
        folioMap_t folioMap;

    protected:
        virtual void setupListener();

        QNetworkInterface iface;
        QAbstractSocket::NetworkLayerProtocol transport;
        QList<QMetaObject::Connection> listenerConnections;
        QMap<QAbstractSocket::NetworkLayerProtocol, QSharedPointer<SocketManager>> sockets;

        /* Local CID */
        public:
            cid_t getLocalCID() const { return CID; }
            void setLocalCID(OTP::cid_t value);
        signals:
            void newLocalCID(OTP::cid_t);
        protected:
            cid_t CID;

        /* Local Name */
        public:
            name_t getLocalName() const { return name; }
            void setLocalName(OTP::name_t);
        signals:
            void newLocalName(OTP::name_t);
        protected:
            name_t name;

        /* Local Systems */
        public:
            QList<system_t> getLocalSystems() const;
            virtual void addLocalSystem(system_t);
            virtual void removeLocalSystem(system_t);
        signals:
            void newLocalSystem(OTP::system_t);
            void removedLocalSystem(OTP::system_t);

        /* Local Modules */
        public:
            moduleList_t getLocalModules() const;
            void addLocalModule(moduleList_t::value_type);
            void removeLocalModule(moduleList_t::value_type);
        signals:
            void newLocalModule(moduleList_t::value_type);
            void removedLocalModule(moduleList_t::value_type);

        /* Components */
        public:
            QList<cid_t> getComponents() const { return otpNetwork->getComponentList(); }
            component_t getComponent(cid_t cid) const { return otpNetwork->getComponent(cid); }
            bool isComponentExpired(cid_t cid) const;
        signals:
            void newComponent(OTP::cid_t);
            void removedComponent(OTP::cid_t);
            void updatedComponent(const OTP::cid_t&, const OTP::name_t&);
            void updatedComponent(const OTP::cid_t&, const QHostAddress&);
            void updatedComponent(const OTP::cid_t&, const OTP::moduleList_t &);
            void updatedComponent(const OTP::cid_t&, OTP::component_t::type_t);

        /* Systems */
        public:
            QList<system_t> getSystems() const { return otpNetwork->getSystemList(); }
            QList<system_t> getSystems(cid_t cid) const { return otpNetwork->getSystemList(cid); }
        signals:
            void newSystem(OTP::cid_t, OTP::system_t);
            void removedSystem(OTP::cid_t, OTP::system_t);

        /* Groups */
        public:
            QList<group_t> getGroups(system_t system) const { return otpNetwork->getGroupList(system); }
            QList<group_t> getGroups(cid_t cid, system_t system) const { return otpNetwork->getGroupList(cid, system); }
            void addGroup(cid_t cid, system_t system, group_t group) {  otpNetwork->addGroup(cid, system, group); }
            bool isGroupExpired(cid_t cid, system_t system, group_t group) const;
            bool isGroupExpired(system_t system, group_t group) const
                { return isGroupExpired(cid_t(), system, group); }
        signals:
            void newGroup(OTP::cid_t, OTP::system_t, OTP::group_t);
            void removedGroup(OTP::cid_t, OTP::system_t, OTP::group_t);

        /* Points */
        public:
            QList<point_t> getPoints(system_t system, group_t group) const { return otpNetwork->getPointList(system, group); }
            QList<point_t> getPoints(cid_t cid, system_t system, group_t group) const { return otpNetwork->getPointList(cid, system, group); }
            bool isPointValid(address_t) const;
            bool isPointValid(cid_t, address_t) const;
            QString getPointName(cid_t, address_t) const;
            QString getPointName(cid_t cid, system_t system, group_t group, point_t point) const
                { return getPointName(cid, address_t(system, group, point)); }
            QString getPointName(address_t address) const
                { return getPointName(cid_t(), address); }
            QString getPointName(system_t system, group_t group, point_t point) const
                { return getPointName(address_t(system, group, point)); }
            QDateTime getPointLastSeen(cid_t, address_t) const;
            QDateTime getPointLastSeen(cid_t cid, system_t system, group_t group, point_t point) const
                { return getPointLastSeen(cid, address_t(system, group, point)); }
            QDateTime getPointLastSeen(address_t address) const
                { return getPointLastSeen(cid_t(), address); }
            QDateTime getPointLastSeen(system_t system, group_t group, point_t point) const
                { return getPointLastSeen(address_t(system, group, point)); }
            bool isPointExpired(cid_t, address_t) const;
            bool isPointExpired(cid_t cid, system_t system, group_t group, point_t point) const
                { return isPointExpired(cid, address_t(system, group, point)); }
            bool isPointExpired(address_t address) const
                { return isPointExpired(cid_t(), address); }
            bool isPointExpired(system_t system, group_t group, point_t point) const
                { return isPointExpired(address_t(system, group, point)); }
        signals:
            void newPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);
            void removedPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);
            void updatedPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);
            void expiredPoint(OTP::cid_t, OTP::system_t, OTP::group_t, OTP::point_t);

        /* Addresses */
        public:
            QList<address_t> getAddresses();
            QList<address_t> getAddresses(system_t);
            QList<address_t> getAddresses(system_t, group_t);

        /* Standard Modules */
        public:
            QString getScaleString(MODULES::STANDARD::PositionModule_t::scale_t, bool html = false) const;
            QString getUnitString(MODULES::STANDARD::VALUES::moduleValue_t, bool html = false) const;
            QString getUnitString(MODULES::STANDARD::PositionModule_t::scale_t, MODULES::STANDARD::VALUES::moduleValue_t, bool html = false) const;

    }; // class Component
}; // namespace OTP

#endif // COMPONENT_H
