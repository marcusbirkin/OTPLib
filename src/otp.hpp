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
#ifndef OTP_H
#define OTP_H

#include "component.hpp"
#include "bugs.hpp"
#include <QCoreApplication>
#include <QObject>
#include <memory>
#include "types.hpp"
#include "network/messages/messages.hpp"
#include "network/modules/modules.hpp"

#if defined MAKE_OTP_LIB
    #define OTP_LIB_EXPORT Q_DECL_EXPORT
#else
    #define OTP_LIB_EXPORT Q_DECL_IMPORT
#endif

namespace OTPLib {
    OTP_LIB_EXPORT QString getVersion();
    OTP_LIB_EXPORT QString getAbout();
    OTP_LIB_EXPORT void displayAbout(QWidget *parent);
}

namespace OTP
{
    class OTP_LIB_EXPORT Producer : public Component
    {
        Q_OBJECT
    public:
        /*
         * Create a OTP Producer object
         *
         * iface - Network interface to utilise
         * name - Producer, human readable, name
         * CID - Component Identifier
         * Transform messgage trasnmission rate, allowed ranges OTP_TRANSFORM_TIMING_MIN -> OTP_TRANSFORM_TIMING_MAX
        */
        explicit Producer(
                QNetworkInterface iface,
                QAbstractSocket::NetworkLayerProtocol transport,
                cid_t CID = cid_t::createUuid(),
                name_t name = QCoreApplication::applicationName(),
                std::chrono::milliseconds transformRate = OTP_TRANSFORM_TIMING_MAX,
                QObject *parent = nullptr);
        ~Producer() {};

        /* Transmission Rates */
        public:
        std::chrono::milliseconds getTransformMsgRate() const
            { return std::chrono::milliseconds(transformMsgTimer.interval()); }
        void setTransformMsgRate(std::chrono::milliseconds value)
            { transformMsgTimer.setInterval(std::clamp(value, OTP_TRANSFORM_TIMING_MIN, OTP_TRANSFORM_TIMING_MAX)); }

        /* Local Groups */
        public:
            QList<group_t> getLocalGroups(system_t) const;
            void addLocalGroup(system_t, group_t);
            void removeLocalGroup(system_t, group_t);
        signals:
            void newLocalGroup(system_t, group_t);
            void removedLocalGroup(system_t, group_t);

        /* Local Points */
        public:
            QList<point_t> getLocalPoints(system_t, group_t) const;
            void addLocalPoint(address_t, priority_t);
            void addLocalPoint(system_t system, group_t group, point_t point, priority_t priority)
                { addLocalPoint(address_t(system, group, point), priority); }
            void removeLocalPoint(address_t);
            void removeLocalPoint(system_t system, group_t group, point_t point)
                { removeLocalPoint(address_t(system, group, point)); }
            void moveLocalPoint(address_t oldAddress, address_t newAddress);
            void moveLocalPoint(
                    system_t oldSystem, group_t oldGroup, point_t oldPoint,
                    system_t newSystem, group_t newGroup, point_t newPoint)
                { moveLocalPoint({oldSystem, oldGroup, oldPoint}, {newSystem, newGroup, newPoint}); }
        signals:
            void newLocalPoint(system_t, group_t, point_t);
            void removedLocalPoint(system_t, group_t, point_t);

        public:
            QString getLocalPointName(address_t) const;
            QString getLocalPointName(system_t system, group_t group, point_t point)
                { return getLocalPointName(address_t(system, group, point)); }
            void setLocalPointName(address_t, QString);
            void setLocalPointName(system_t system, group_t group, point_t point, QString name)
                { setLocalPointName(address_t(system, group, point), name); }
        signals:
            void updatedLocalPointName(address_t);

        public:
            priority_t getLocalPointPriority(address_t) const;
            priority_t getLocalPointPriority(system_t system, group_t group, point_t point)
                { return getLocalPointPriority(address_t(system, group, point)); }
            void setLocalPointPriority(address_t, priority_t);
            void setLocalPointPriority(system_t system, group_t group, point_t point, priority_t priority)
                { setLocalPointPriority(address_t(system, group, point), priority); }
        signals:
            void updatedLocalPointPriority(address_t);

        /* Local Addresses */
        public:
            QList<address_t> getLocalAddresses();
            QList<address_t> getLocalAddresses(system_t);
            QList<address_t> getLocalAddresses(system_t, group_t);

        /* Standard Modules */
        /* -Position */
        public:
            typedef struct PositionValue_s
            {
                MODULES::STANDARD::PositionModule_t::position_t value = 0;
                QString unit;
                timestamp_t timestamp = static_cast<timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch());
                MODULES::STANDARD::PositionModule_t::scale_t scale;
            } PositionValue_t;
            PositionValue_t getLocalPosition(address_t, axis_t) const;
            void setLocalPosition(address_t, axis_t, PositionValue_t);
        signals:
            void updatedPosition(address_t, axis_t);

        /* -Position Velocity/Acceleration */
        public:
            typedef struct PositionVelocity_s
            {
                MODULES::STANDARD::PositionVelAccModule_t::velocity_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
            } PositionVelocity_t;
            PositionVelocity_t getLocalPositionVelocity(address_t, axis_t) const;
            void setLocalPositionVelocity(address_t, axis_t, PositionVelocity_t);

            typedef struct PositionAcceleration_s
            {
                MODULES::STANDARD::PositionVelAccModule_t::acceleration_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
            } PositionAcceleration_t;
            PositionAcceleration_t getLocalPositionAcceleration( address_t, axis_t) const;
            void setLocalPositionAcceleration(address_t, axis_t, PositionAcceleration_t);
        signals:
            void updatedPositionVelocity(address_t, axis_t);
            void updatedPositionAcceleration(address_t, axis_t);

        /* -Rotation */
        public:
            typedef struct RotationValue_s
            {
                MODULES::STANDARD::RotationModule_t::rotation_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
            } RotationValue_t;
            RotationValue_t getLocalRotation(address_t, axis_t) const;
            void setLocalRotation(address_t, axis_t, RotationValue_t);
        signals:
            void updatedRotation(address_t, axis_t);

        /* -Rotation Velocity/Acceleration */
        public:
            typedef struct RotationVelocity_s
            {
                MODULES::STANDARD::RotationVelAccModule_t::velocity_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
            } RotationVelocity_t;
            RotationVelocity_t getLocalRotationVelocity(address_t, axis_t) const;
            void setLocalRotationVelocity(address_t, axis_t, RotationVelocity_t);

            typedef struct RotationAcceleration_s
            {
                MODULES::STANDARD::RotationVelAccModule_t::acceleration_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
            } RotationAcceleration_t;
            RotationAcceleration_t getLocalRotationAcceleration(address_t, axis_t) const;
            void setLocalRotationAcceleration(address_t, axis_t, RotationAcceleration_t);
        signals:
            void updatedRotationVelocity(address_t, axis_t);
            void updatedRotationAcceleration(address_t, axis_t);

        /* -Scale */
        public:
        typedef struct Scale_s
            {
                MODULES::STANDARD::ScaleModule_t::scale_t value = 0;
                MODULES::STANDARD::ScaleModule_t::percent_t getPercent() { return MODULES::STANDARD::ScaleModule_t::toPercent(value); }
                operator QString() { return MODULES::STANDARD::ScaleModule_t::toPercentString(value).append("%"); }
                void setPercent(MODULES::STANDARD::ScaleModule_t::percent_t value) { value = MODULES::STANDARD::ScaleModule_t::fromPercent(value); }
                timestamp_t timestamp = 0;
            } Scale_t;
            Scale_t getLocalScale(address_t, axis_t) const;
            void setLocalScale(address_t, axis_t, Scale_t);
        signals:
            void updatedScale(address_t, axis_t);

        /* -Reference Frame */
        public:
        typedef struct ReferenceFrame_t
            {
                address_t value;
                timestamp_t timestamp = 0;
                cid_t sourceCID;
            } ReferenceFrame_t;
            ReferenceFrame_t getLocalReferenceFrame(address_t) const;
            void setLocalReferenceFrame(address_t, ReferenceFrame_t);
        signals:
            void updatedReferenceFrame(address_t);

    private:
        void setupSender(std::chrono::milliseconds transformRate);
        QTimer transformMsgTimer;

        bool receiveOTPTransformMessage(const QNetworkDatagram &datagram) override;
        bool receiveOTPModuleAdvertisementMessage(const QNetworkDatagram &datagram) override;
        bool receiveOTPNameAdvertisementMessage(const QNetworkDatagram &datagram) override;
        bool receiveOTPSystemAdvertisementMessage(const QNetworkDatagram &datagram) override;

        QTimer* getBackoffTimer(std::chrono::milliseconds maximum);
        void sendOTPNameAdvertisementMessage(QHostAddress destinationAddr, MESSAGES::OTPNameAdvertisementMessage::folio_t folio);
        void sendOTPSystemAdvertisementMessage(QHostAddress destinationAddr, MESSAGES::OTPNameAdvertisementMessage::folio_t folio);
        void sendOTPTransformMessage(system_t system);
        PDU::OTPLayer::folio_t TransformMessage_Folio = 0;
    };

    class OTP_LIB_EXPORT Consumer : public Component
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
                cid_t CID = cid_t::createUuid(),
                name_t name = QCoreApplication::applicationName(),
                QObject *parent = nullptr);
        ~Consumer() {};

        /*
         * Send a network request for systems and point descriptions
         * Signals emitted by Container
         */
        void UpdateOTPMap();

        /* Local Systems */
        public:
            void addLocalSystem(system_t) override;
            void removeLocalSystem(system_t) override;

        /* Standard Modules */
        private:
            template <class T, class T2>
            T getValueHelper(T2 &, cid_t, address_t, bool respectRelative = true) const;
            template <class T>
            QMap<cid_t, T> getValuesHelper(std::function<T(cid_t, address_t, axis_t, bool)>, address_t, axis_t, bool respectRelative = true, bool excludeWinner = false) const;
        /* -Position */
        public:
            typedef struct PositionValue_s
            {
                MODULES::STANDARD::PositionModule_t::position_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
                MODULES::STANDARD::PositionModule_t::scale_t scale;
                cid_t sourceCID;
                priority_t priority;
            } PositionValue_t;
            PositionValue_t getPosition(cid_t, address_t, axis_t, bool respectRelative = true) const;
            PositionValue_t getPosition(address_t, axis_t, bool respectRelative = true) const;
            QMap<cid_t, PositionValue_t> getPositions(address_t, axis_t, bool respectRelative = true, bool excludeWinner = false) const;

        signals:
            void updatedPosition(cid_t, address_t, axis_t);

        /* -Position Velocity/Acceleration */
        public:
            typedef struct PositionVelocity_s
            {
                MODULES::STANDARD::PositionVelAccModule_t::velocity_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
                cid_t sourceCID;
                priority_t priority;
            } PositionVelocity_t;
            PositionVelocity_t getPositionVelocity(cid_t, address_t, axis_t, bool respectRelative = true) const;
            PositionVelocity_t getPositionVelocity(address_t, axis_t, bool respectRelative = true) const;
            QMap<cid_t, PositionVelocity_t> getPositionVelocitys(address_t, axis_t, bool respectRelative = true, bool excludeWinner = false) const;

            typedef struct PositionAcceleration_s
            {
                MODULES::STANDARD::PositionVelAccModule_t::acceleration_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
                cid_t sourceCID;
                priority_t priority;
            } PositionAcceleration_t;
            PositionAcceleration_t getPositionAcceleration(cid_t, address_t, axis_t, bool respectRelative = true) const;
            PositionAcceleration_t getPositionAcceleration(address_t, axis_t, bool respectRelative = true) const;
            QMap<cid_t, PositionAcceleration_t> getPositionAccelerations(address_t, axis_t, bool respectRelative = true, bool excludeWinner = false) const;
        signals:
            void updatedPositionVelocity(cid_t, address_t, axis_t);
            void updatedPositionAcceleration(cid_t, address_t, axis_t);

        /* -Rotation */
        public:
            typedef struct RotationValue_s
            {
                MODULES::STANDARD::RotationModule_t::rotation_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
                cid_t sourceCID;
                priority_t priority;
            } RotationValue_t;
            RotationValue_t getRotation(cid_t, address_t, axis_t, bool respectRelative = true) const;
            RotationValue_t getRotation(address_t, axis_t, bool respectRelative = true) const;
            QMap<cid_t, RotationValue_t> getRotations(address_t, axis_t, bool respectRelative = true, bool excludeWinner = false) const;
        signals:
            void updatedRotation(cid_t, address_t, axis_t);

        /* -Position Velocity/Acceleration */
        public:
            typedef struct RotationVelocity_s
            {
                MODULES::STANDARD::RotationVelAccModule_t::velocity_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
                cid_t sourceCID;
                priority_t priority;
            } RotationVelocity_t;
            RotationVelocity_t getRotationVelocity(cid_t, address_t, axis_t, bool respectRelative = true) const;
            RotationVelocity_t getRotationVelocity(address_t, axis_t, bool respectRelative = true) const;
            QMap<cid_t, RotationVelocity_t> getRotationVelocitys(address_t, axis_t, bool respectRelative = true, bool excludeWinner = false) const;

            typedef struct RotationAcceleration_s
            {
                MODULES::STANDARD::RotationVelAccModule_t::acceleration_t value = 0;
                QString unit;
                timestamp_t timestamp = 0;
                cid_t sourceCID;
                priority_t priority;
            } RotationAcceleration_t;
            RotationAcceleration_t getRotationAcceleration(cid_t, address_t, axis_t, bool respectRelative = true) const;
            RotationAcceleration_t getRotationAcceleration(address_t, axis_t, bool respectRelative = true) const;
            QMap<cid_t, RotationAcceleration_t> getRotationAccelerations(address_t, axis_t, bool respectRelative = true, bool excludeWinner = false) const;
        signals:
            void updatedRotationVelocity(cid_t, address_t, axis_t);
            void updatedRotationAcceleration(cid_t, address_t, axis_t);

        /* -Scale */
        public:
            typedef struct Scale_s
            {
                MODULES::STANDARD::ScaleModule_t::scale_t value = 0;
                MODULES::STANDARD::ScaleModule_t::percent_t getPercent() { return MODULES::STANDARD::ScaleModule_t::toPercent(value); }
                QString unit;
                operator QString() const { return MODULES::STANDARD::ScaleModule_t::toPercentString(value).append("%"); }
                void setPercent(MODULES::STANDARD::ScaleModule_t::percent_t value) { value = MODULES::STANDARD::ScaleModule_t::fromPercent(value); }
                timestamp_t timestamp = 0;
                cid_t sourceCID;
                priority_t priority;
            } Scale_t;
            Scale_t getScale(cid_t, address_t, axis_t) const;
            Scale_t getScale(address_t, axis_t) const;
            QMap<cid_t, Scale_t> getScales(address_t, axis_t, bool excludeWinner = false) const;
        signals:
            void updatedScale(cid_t, address_t, axis_t);

        /* -Reference Frame */
        public:
            typedef struct ReferenceFrame_t
            {
                address_t value;
                timestamp_t timestamp = 0;
                cid_t sourceCID;
                priority_t priority;
            } ReferenceFrame_t;
            ReferenceFrame_t getReferenceFrame(cid_t, address_t) const;
            ReferenceFrame_t getReferenceFrame(address_t) const;
        signals:
            void updatedReferenceFrame(cid_t, address_t);

    private:
        void setupListener() override;

        bool receiveOTPTransformMessage(const QNetworkDatagram &datagram) override;
        bool receiveOTPModuleAdvertisementMessage(const QNetworkDatagram &datagram) override;
        bool receiveOTPNameAdvertisementMessage(const QNetworkDatagram &datagram) override;
        bool receiveOTPSystemAdvertisementMessage(const QNetworkDatagram &datagram) override;

        void sendOTPModuleAdvertisementMessage();
        PDU::OTPLayer::folio_t ModuleAdvertisementMessage_Folio = 0;
        void sendOTPNameAdvertisementMessage();
        PDU::OTPLayer::folio_t NameAdvertisementMessage_Folio = 0;
        void sendOTPSystemAdvertisementMessage();
        PDU::OTPLayer::folio_t SystemAdvertisementMessage_Folio = 0;
    };
};

#endif // OTP_H
