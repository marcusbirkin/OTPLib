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
#ifndef OTP_HPP
#define OTP_HPP

#include <QObject>
#include <QNetworkInterface>
#include <QAbstractSocket>
#include <QMessageBox>
#include <memory>
#include "socket.hpp"
#include "types.hpp"
#include "network/pdu/pdu_types.hpp"
#include "network/messages/messages.hpp"
#include "network/modules/modules.hpp"

namespace OTPLib {
    QString getVersion();
    QString getAbout();
    void displayAbout(QWidget *parent);
}

namespace ACN::OTP
{
    class Container;

    class Producer : public QObject
    {
        Q_OBJECT
    public:
        /*
         * Create a OTP Producer object
         *
         * iface - Network interface to utilise
         * name - Producer, human readable, name
         * CID - Component Identifier
        */
        explicit Producer(
                QNetworkInterface iface,
                QAbstractSocket::NetworkLayerProtocol transport,
                name_t name = QApplication::applicationName(),
                cid_t CID = cid_t::createUuid(),
                QObject *parent = nullptr);
        ~Producer();

        /*
         * Clear local copy of network
         */
        void ClearOTPMap();

        /* Producer CID */
        public:
            cid_t getProducerCID() const { return CID; }
            void setProducerCID(cid_t);
        signals:
            void newProducerCID(cid_t);

        /* Producer Name */
        public:
            name_t getProducerName() const { return name; }
            void setProducerName(name_t);
        signals:
            void newProducerName(name_t);

        /* Producer Network Interface */
        public:
            QNetworkInterface getProducerNetworkInterface() const { return iface; }
            void setProducerNetworkInterface(QNetworkInterface value);
            QAbstractSocket::NetworkLayerProtocol getProducerNetworkTransport() const { return transport; }
            void setProducerNetworkTransport(QAbstractSocket::NetworkLayerProtocol transport);
            QAbstractSocket::SocketState getProducerNetworkinterfaceState(QAbstractSocket::NetworkLayerProtocol transport) const
                { return SocketManager::getInstance(getProducerNetworkInterface(), transport)->state(); }
        signals:
            void newProducerNetworkInterface(QNetworkInterface);
            void newProducerNetworkTransport(QAbstractSocket::NetworkLayerProtocol);

        /* Producer Systems */
        public:
            QList<system_t> getProducerSystems() const;
            void addProducerSystem(system_t);
            void removeProducerSystem(system_t);
        signals:
            void newProducerSystem(system_t);
            void removedProducerSystem(system_t);

        /* Producer Groups */
        public:
            QList<group_t> getProducerGroups(system_t) const;
            void addProducerGroup(system_t, group_t);
            void removeProducerGroup(system_t, group_t);
        signals:
            void newProducerGroup(system_t, group_t);
            void removedProducerGroup(system_t, group_t);

        /* Producer Points */
        public:
            QList<point_t> getProducerPoints(system_t, group_t) const;
            void addProducerPoint(address_t);
            void addProducerPoint(system_t system, group_t group, point_t point)
                { addProducerPoint(address_t(system, group, point)); }
            void removeProducerPoint(address_t);
            void removeProducerPoint(system_t system, group_t group, point_t point)
                { removeProducerPoint(address_t(system, group, point)); }
        signals:
            void newProducerPoint(system_t, group_t, point_t);
            void removedProducerPoint(system_t, group_t, point_t);

        public:
            QString getProducerPointName(address_t) const;
            QString getProducerPointName(system_t system, group_t group, point_t point)
                { return getProducerPointName(address_t(system, group, point)); }
            void setProducerPointName(address_t, QString);
            void setProducerPointName(system_t system, group_t group, point_t point, QString name)
                { setProducerPointName(address_t(system, group, point), name); }
        signals:
            void newProducerPointName(name_t);

        /* Producer Addresses */
        public:
            QList<address_t> getProducerAddresses();
            QList<address_t> getProducerAddresses(system_t);
            QList<address_t> getProducerAddresses(system_t, group_t);

        /* Components */
        public:
            QList<cid_t> getComponents() const;
            component_t getComponent(cid_t) const;
        signals:
            void newComponent(cid_t);
            void removedComponent(cid_t);
            void updatedComponent(const ACN::OTP::cid_t&, const ACN::OTP::name_t&);
            void updatedComponent(const ACN::OTP::cid_t&, const QHostAddress&);
            void updatedComponent(const ACN::OTP::cid_t&, const ACN::OTP::moduleList_t &);
            void updatedComponent(const ACN::OTP::cid_t&, ACN::OTP::component_t::type_t);

        /* Systems */
        public:
            QList<system_t> getSystems() const;
            void addSystem(cid_t, system_t);
        signals:
            void newSystem(cid_t, system_t);
            void removedSystem(cid_t, system_t);

        /* Groups */
        public:
            QList<group_t> getGroups(system_t) const;
            void addGroup(cid_t, system_t, group_t);
        signals:
            void newGroup(cid_t, system_t, group_t);
            void removedGroup(cid_t, system_t, group_t);

        /* Points */
        public:
            QList<point_t> getPoints(system_t, group_t) const;

            QString getPointName(cid_t, address_t) const;
            QString getPointName(cid_t cid, system_t system, group_t group, point_t point) const
                { return getPointName(cid, address_t(system, group, point)); }
            QString getPointName(address_t address) const
                { return getPointName(cid_t(), address); }
            QString getPointName(system_t system, group_t group, point_t point) const
                { return getPointName(address_t(system, group, point)); }
        signals:
            void newPoint(cid_t, system_t, group_t, point_t);
            void removedPoint(cid_t, system_t, group_t, point_t);

        /* Addresses */
        public:
            QList<address_t> getAddresses();
            QList<address_t> getAddresses(system_t);
            QList<address_t> getAddresses(system_t, group_t);

        /* Standard Modules */
        public:
            QString getScaleString(address_t, MODULES::STANDARD::VALUES::moduleValue_t) const;
            QString getUnitString(address_t, MODULES::STANDARD::VALUES::moduleValue_t) const;

        /* -Position */
        public:
            MODULES::STANDARD::PositionModule_t::scale_t getPositionScale(address_t) const;
            void setPositionScale(address_t, MODULES::STANDARD::PositionModule_t::scale_t);
            QString getPositionScaleString(address_t address) const
                {
                    return getScaleString(address, MODULES::STANDARD::VALUES::POSITION);
                }
            MODULES::STANDARD::PositionModule_t::location_t getPositionLocation(address_t, axis_t) const;
            void setPositionLocation(
                    address_t,
                    axis_t,
                    MODULES::STANDARD::PositionModule_t::location_t,
                    timestamp_t = static_cast<timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch()));
            QString getPositionLocationUnit(address_t address) const
                {
                    return getUnitString(address, MODULES::STANDARD::VALUES::POSITION);
                }
        signals:
            void updatedPosition(address_t, axis_t);

        /* -Position Velocity/Acceleration */
        public:
            MODULES::STANDARD::PositionVelAccModule_t::velocity_t getPositionVelocity(address_t, axis_t) const;
            void setPositionVelocity(
                    address_t,
                    axis_t,
                    MODULES::STANDARD::PositionVelAccModule_t::velocity_t,
                    timestamp_t = static_cast<timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch()));
            QString getPositionVelocitynUnit(address_t address) const
                {
                    return getUnitString(address, MODULES::STANDARD::VALUES::POSITION_VELOCITY);
                }

            MODULES::STANDARD::PositionVelAccModule_t::acceleration_t getPositionAcceleration(address_t, axis_t) const;
            void setPositionAcceleration(
                    address_t, axis_t,
                    MODULES::STANDARD::PositionVelAccModule_t::acceleration_t,
                    timestamp_t = static_cast<timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch()));
            QString getPositionAccelerationUnit(address_t address) const
                {
                    return getUnitString(address, MODULES::STANDARD::VALUES::POSITION_ACCELERATION);
                }
        signals:
            void updatedPositionVelocity(address_t, axis_t);
            void updatedPositionAcceleration(address_t, axis_t);

        /* -Rotation */
        public:
            MODULES::STANDARD::RotationModule_t::rotation_t getRotation(address_t, axis_t) const;
            void setRotation(
                    address_t,
                    axis_t,
                    MODULES::STANDARD::RotationModule_t::rotation_t,
                    timestamp_t = static_cast<timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch()));
            QString getRotationUnit(address_t address) const
                {
                    return getUnitString(address, MODULES::STANDARD::VALUES::ROTATION);
                }
        signals:
            void updatedRotation(address_t, axis_t);

        /* -Position Velocity/Acceleration */
        public:
            MODULES::STANDARD::RotationVelAccModule_t::velocity_t getRotationVelocity(address_t, axis_t) const;
            void setRotationVelocity(
                    address_t,
                    axis_t,
                    MODULES::STANDARD::RotationVelAccModule_t::velocity_t,
                    timestamp_t = static_cast<timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch()));
            QString getRotationVelocityUnit(address_t address) const
                {
                    return getUnitString(address, MODULES::STANDARD::VALUES::ROTATION_VELOCITY);
                }

            MODULES::STANDARD::RotationVelAccModule_t::acceleration_t getRotationAcceleration(address_t, axis_t) const;
            void setRotationAcceleration(
                    address_t,
                    axis_t,
                    MODULES::STANDARD::RotationVelAccModule_t::acceleration_t,
                    timestamp_t = static_cast<timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch()));
            QString getRotationAccelerationUnit(address_t address) const
                {
                    return getUnitString(address, MODULES::STANDARD::VALUES::ROTATION_ACCELERATION);
                }
        signals:
            void updatedRotationVelocity(address_t, axis_t);
            void updatedRotationAcceleration(address_t, axis_t);

    signals:
        void newCID(cid_t);

    private slots:
        void setupSender();
        void newDatagram(QNetworkDatagram datagram);

    private:
        void setupListener();

        void sendOTPNameAdvertisementMessage(QHostAddress destinationAddr, MESSAGES::OTPNameAdvertisementMessage::folio_t folio);
        void sendOTPSystemAdvertisementMessage(QHostAddress destinationAddr, MESSAGES::OTPNameAdvertisementMessage::folio_t folio);
        void sendOTPTransformMessage(system_t system);
        PDU::OTPLayer::folio_t TransformMessage_Folio = 0;

        QMap<cid_t, sequenceMap_t> sequenceMap;

        std::unique_ptr<Container> otpNetwork;
        QNetworkInterface iface;
        QAbstractSocket::NetworkLayerProtocol transport;
        QList<QMetaObject::Connection> listenerConnections;

        cid_t CID;
        name_t name;
    };

    class Consumer : public QObject
    {
        Q_OBJECT
    public:
        /*
         * Create a OTP Consumer object
         *
         * iface - Network interface to utilise
         * systems - List of systems to monitor
         * CID - Component Identifier
        */
        explicit Consumer(
                QNetworkInterface iface,
                QAbstractSocket::NetworkLayerProtocol transport,
                QList<system_t> systems,
                name_t name = QApplication::applicationName(),
                cid_t CID = cid_t::createUuid(),
                QObject *parent = nullptr);
        ~Consumer();

        /*
         * Clear local copy of network
         */
        void ClearOTPMap();

        /*
         * Send a network request for systems and point descriptions
         * Signals emitted by Container
         */
        void UpdateOTPMap();

        /* Consumer CID */
        public:
            cid_t getConsumerCID() const { return CID; }
            void setConsumerCID(cid_t);
        signals:
            void newConsumerCID(cid_t);

        /* Consumer Name */
        public:
            name_t getConsumerName() const { return name; }
            void setConsumerName(name_t);
        signals:
            void newConsumerName(name_t);

        /* Consumer Network Interface */
        public:
            QNetworkInterface getConsumerNetworkInterface() const { return iface; }
            void setConsumerNetworkInterface(QNetworkInterface value);
            QAbstractSocket::NetworkLayerProtocol getConsumerNetworkTransport() const { return transport; }
            void setConsumerNetworkTransport(QAbstractSocket::NetworkLayerProtocol value);
            QAbstractSocket::SocketState getConsumerNetworkinterfaceState(QAbstractSocket::NetworkLayerProtocol transport) const
                { return SocketManager::getInstance(getConsumerNetworkInterface(), transport)->state(); }
        signals:
            void newConsumerNetworkInterface(QNetworkInterface);
            void newConsumerNetworkTransport(QAbstractSocket::NetworkLayerProtocol);

        /* Consumer Systems */
        public:
            QList<system_t> getConsumerSystems() const;
            void addConsumerSystem(system_t);
            void removeConsumerSystem(system_t);
        signals:
            void newConsumerSystem(system_t);
            void removedConsumerSystem(system_t);

        /* Components */
        public:
            QList<cid_t> getComponents() const;
            component_t getComponent(cid_t) const;
        signals:
            void newComponent(cid_t);
            void removedComponent(cid_t);
            void updatedComponent(const ACN::OTP::cid_t&, const ACN::OTP::name_t&);
            void updatedComponent(const ACN::OTP::cid_t&, const QHostAddress&);
            void updatedComponent(const ACN::OTP::cid_t&, const ACN::OTP::moduleList_t &);
            void updatedComponent(const ACN::OTP::cid_t&, ACN::OTP::component_t::type_t);

        /* Systems */
        public:
            QList<system_t> getSystems() const;
            QList<system_t> getSystems(cid_t) const;
        signals:
            void newSystem(cid_t, system_t);
            void removedSystem(cid_t, system_t);

        /* Groups */
        public:
            QList<group_t> getGroups(system_t) const;
            QList<group_t> getGroups(cid_t, system_t) const;

            bool isGroupExpired(cid_t cid, system_t system, group_t group) const;
            bool isGroupExpired(system_t system, group_t group) const
                { return isGroupExpired(cid_t(), system, group); }
        signals:
            void newGroup(cid_t, system_t, group_t);
            void removedGroup(cid_t, system_t, group_t);

        /* Points */
        public:
            QList<point_t> getPoints(system_t, group_t) const;
            QList<point_t> getPoints(cid_t, system_t, group_t) const;

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

        /* Addresses */
        public:
            QList<address_t> getAddresses();
            QList<address_t> getAddresses(system_t);
            QList<address_t> getAddresses(system_t, group_t);


        signals:
            void newPoint(cid_t, system_t, group_t, point_t);
            void updatedPoint(cid_t, system_t, group_t, point_t);
            void expiredPoint(cid_t, system_t, group_t, point_t);
            void removedPoint(cid_t, system_t, group_t, point_t);

        /* Standard Modules */
        public:
            enum multipleProducerResolution_e
            {
                Newest,
                Largest,
                Smallest
            };
            QString getScaleString(MODULES::STANDARD::PositionModule_t::scale_t) const;
            QString getUnitString(MODULES::STANDARD::VALUES::moduleValue_t) const;
            QString getUnitString(MODULES::STANDARD::PositionModule_t::scale_t, MODULES::STANDARD::VALUES::moduleValue_t) const;
        /* -Position */
        public:
            typedef struct PositionValue_s
            {
                MODULES::STANDARD::PositionModule_t::location_t value = 0;
                QString unit;
                timestamp_t timestamp;
                MODULES::STANDARD::PositionModule_t::scale_t scale;
                cid_t sourceCID;
            } PositionValue_t;
            PositionValue_t getPosition(cid_t, address_t, axis_t) const;
            PositionValue_t getPosition(address_t, axis_t, multipleProducerResolution_e) const;

        signals:
            void updatedPosition(cid_t, address_t, axis_t);

        /* -Position Velocity/Acceleration */
        public:
            typedef struct PositionVelocity_s
            {
                MODULES::STANDARD::PositionVelAccModule_t::velocity_t value = 0;
                QString unit;
                timestamp_t timestamp;
                cid_t sourceCID;
            } PositionVelocity_t;
            PositionVelocity_t getPositionVelocity(cid_t, address_t, axis_t) const;
            PositionVelocity_t getPositionVelocity(address_t, axis_t, multipleProducerResolution_e) const;

            typedef struct PositionAcceleration_s
            {
                MODULES::STANDARD::PositionVelAccModule_t::acceleration_t value = 0;
                QString unit;
                timestamp_t timestamp;
                cid_t sourceCID;
            } PositionAcceleration_t;
            PositionAcceleration_t getPositionAcceleration(cid_t, address_t, axis_t) const;
            PositionAcceleration_t getPositionAcceleration(address_t, axis_t, multipleProducerResolution_e) const;
        signals:
            void updatedPositionVelocity(cid_t, address_t, axis_t);
            void updatedPositionAcceleration(cid_t, address_t, axis_t);

        /* -Rotation */
        public:
            typedef struct RotationValue_s
            {
                MODULES::STANDARD::RotationModule_t::rotation_t value = 0;
                QString unit;
                timestamp_t timestamp;
                cid_t sourceCID;
            } RotationValue_t;
            RotationValue_t getRotation(cid_t, address_t, axis_t) const;
            RotationValue_t getRotation(address_t, axis_t, multipleProducerResolution_e) const;
        signals:
            void updatedRotation(cid_t, address_t, axis_t);

        /* -Position Velocity/Acceleration */
        public:
            typedef struct RotationVelocity_s
            {
                MODULES::STANDARD::RotationVelAccModule_t::velocity_t value = 0;
                QString unit;
                timestamp_t timestamp;
                cid_t sourceCID;
            } RotationVelocity_t;
            RotationVelocity_t getRotationVelocity(cid_t, address_t, axis_t) const;
            RotationVelocity_t getRotationVelocity(address_t, axis_t, multipleProducerResolution_e) const;
            typedef struct RotationAcceleration_s
            {
                MODULES::STANDARD::RotationVelAccModule_t::acceleration_t value = 0;
                QString unit;
                timestamp_t timestamp;
                cid_t sourceCID;
            } RotationAcceleration_t;
            RotationAcceleration_t getRotationAcceleration(cid_t, address_t, axis_t) const;
            RotationAcceleration_t getRotationAcceleration(address_t, axis_t, multipleProducerResolution_e) const;
        signals:
            void updatedRotationVelocity(cid_t, address_t, axis_t);
            void updatedRotationAcceleration(cid_t, address_t, axis_t);

    private slots:
        void newDatagram(QNetworkDatagram datagram);

    private:
        void setupListener();

        void sendOTPModuleAdvertisementMessage();
        PDU::OTPLayer::folio_t ModuleAdvertisementMessage_Folio = 0;
        void sendOTPNameAdvertisementMessage();
        PDU::OTPLayer::folio_t NameAdvertisementMessage_Folio = 0;
        void sendOTPSystemAdvertisementMessage();
        PDU::OTPLayer::folio_t SystemAdvertisementMessage_Folio = 0;

        QMap<cid_t, sequenceMap_t> sequenceMap;
        folioMap_t folioMap;

        std::unique_ptr<Container> otpNetwork;
        QNetworkInterface iface;
        QAbstractSocket::NetworkLayerProtocol transport;
        QList<QMetaObject::Connection> listenerConnections;

        cid_t CID;
        name_t name;
    };
};

#endif // OTP_HPP
