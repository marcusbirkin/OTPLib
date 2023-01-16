/**
 * @file        otp.hpp
 * @brief       Public include for library
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
 * @mainpage %OTPLib - A QT interface for E1.59 (Entertainment Technology Object Transform Protocol (%OTP))
 *  <hr/>
 *
 *  Extract (emphasis added) from E1.59:
 *  
 *  <i>This standard describes a mechanism to transfer object transform information such as position, 
 *  rotation and velocity over an IP network. 
 *  It covers data format, data protocol, data addressing, and network management. 
 *  It does not require real-world location or any association between multiple objects represented in the same message.
 *  
 *  <strong>Data transmitted is intended to coordinate visual and audio elements of a production and should not be
 *  used for safety critical applications.</strong></i>
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

/**
 * @brief Details about the library
 */
namespace OTPLib {
    /**
     * @brief Get library version number
     * 
     * @return Dotted string version
     */
    OTP_LIB_EXPORT QString getVersion();
    /**
     * @brief Get contents for displaying in about screens/message boxes
     * 
     * @return About message string
     */
    OTP_LIB_EXPORT QString getAbout();
    /**
     * @brief Display message box containing about details for library
     * 
     * @param parent Parent object
     */
    OTP_LIB_EXPORT void displayAbout(QWidget *parent);
}

/**
 * @brief Main interface into the OTP network
 * 
 */
namespace OTP
{
    /**
     * @brief OTP Producer component
     * @details <b>Producer:</b> A Producer is any network device transmitting OTP Transform Messages.
     * 
     */
    class OTP_LIB_EXPORT Producer : public Component
    {
        Q_OBJECT
    public:
        /**
         * @brief Create a Producer object
         * 
         * @param iface Network interface to utilise
         * @param transport Network transport
         * @param CID Component IDentifier
         * @param name Human readable name
         * @param transformRate Transform messgage transmission interval\n
         *      Allowed ranges OTP::OTP_TRANSFORM_TIMING_MIN -> OTP::OTP_TRANSFORM_TIMING_MAX
         * @param parent Parent object
         */
        explicit Producer(
                QNetworkInterface iface,
                QAbstractSocket::NetworkLayerProtocol transport,
                cid_t CID = cid_t::createUuid(),
                name_t name = QCoreApplication::applicationName(),
                std::chrono::milliseconds transformRate = OTP_TRANSFORM_TIMING_MAX,
                QObject *parent = nullptr);
        ~Producer() {};

    /** 
     * @name Transmission Rates
     * 
     *@{
     */  
    public:
        /**
         * @brief Get producers current transform messgage transmission rate
         * @details Rate of transmission for OTP Folio of OTP Transform Messages for each System\n
         *          Allowed ranges OTP::OTP_TRANSFORM_TIMING_MIN -> OTP::OTP_TRANSFORM_TIMING_MAX
         * @return Current interval
         */
        std::chrono::milliseconds getTransformMsgRate() const
            { return std::chrono::milliseconds(transformMsgTimer.interval()); }

        /**
         * @brief Set producers new transform messgage transmission rate
         * @details Rate of transmission for OTP Folio of OTP Transform Messages for each System\n
         *          Allowed ranges OTP::OTP_TRANSFORM_TIMING_MIN -> OTP::OTP_TRANSFORM_TIMING_MAX
         * 
         * @param value New interval
         */
        void setTransformMsgRate(std::chrono::milliseconds value)
            { transformMsgTimer.setInterval(std::clamp(value, OTP_TRANSFORM_TIMING_MIN, OTP_TRANSFORM_TIMING_MAX)); }
    /**@}*/ // Transmission Rates

    /** 
     * @name Local Groups
     * 
     * @{
     */  
    public:
        /**
         * @brief Get list active groups for this producers system
         * 
         * @param system Local system to query
         * @return List of groups
         */
        QList<group_t> getLocalGroups(system_t system) const;

        /**
         * @brief Add a new active group to this producer
         * 
         * @param system Local sytem to contain the new group
         * @param group New group to add
         */
        void addLocalGroup(system_t system, group_t group);

        /**
         * @brief Remove an active group from this producer
         * 
         * @param system Local system containing group
         * @param group Group to remove from system
         */
        void removeLocalGroup(system_t system, group_t group);

    signals:
        /**
         * @brief Emitted when a new group is added to the producer
         * 
         * @param system System containing new group
         * @param group New group
         */
        void newLocalGroup(OTP::system_t system, OTP::group_t group);

        /**
         * @brief Emitted when a group is removed from the producer
         * 
         * @param system System containing removed group
         * @param group Removed group
         */
        void removedLocalGroup(OTP::system_t system, OTP::group_t group);

    /**@}*/ // Local Groups

    /** 
     * @name Local Point
     * 
     * @{
     */  
    public:
        /**
         * @brief Get list active points for this producers group
         * 
         * @param system Local system to query
         * @param group Local group within system to query
         * @return List of points
         */
        QList<point_t> getLocalPoints(system_t system, group_t group) const;

        /**
         * @brief Add a new active point to this producer
         * 
         * @param address Address of new point
         * @param priority Priority of new point
         */
        void addLocalPoint(address_t address, priority_t priority);

        /**
         * @brief Add a new active point to this producer
         * 
         * @param system System containing new point
         * @param group Group within system containing new point
         * @param point New point
         * @param priority Priority of new point
         */
        void addLocalPoint(system_t system, group_t group, point_t point, priority_t priority)
            { addLocalPoint(address_t(system, group, point), priority); }

        /**
         * @brief Remove an active point from this producer
         * 
         * @param address Address of point to remove
         */
        void removeLocalPoint(address_t address);
        
        /**
         * @brief Remove an active point from this producer
         * 
         * @param system System containing point to remove
         * @param group Group within system containing point to remove
         * @param point Point to remove
         */
        void removeLocalPoint(system_t system, group_t group, point_t point)
            { removeLocalPoint(address_t(system, group, point)); }

        /**
         * @brief Move active point
         * 
         * @param oldAddress Source address of point
         * @param newAddress Destination address of point
         */
        void moveLocalPoint(address_t oldAddress, address_t newAddress);

        /**
         * @brief Move active point
         * 
         * @param oldSystem Source system containing point
         * @param oldGroup Source group witin source system containing point
         * @param oldPoint Source point
         * @param newSystem Destination system for point
         * @param newGroup Destination group within destination system for point
         * @param newPoint Destination point
         */
        void moveLocalPoint(
                system_t oldSystem, group_t oldGroup, point_t oldPoint,
                system_t newSystem, group_t newGroup, point_t newPoint)
            { moveLocalPoint({oldSystem, oldGroup, oldPoint}, {newSystem, newGroup, newPoint}); }

    signals:
        /**
         * @brief Emitted when a new point is added to the producer
         * 
         * @param system System containing new point
         * @param group Group within system containing new point
         * @param point New point
         */
        void newLocalPoint(OTP::system_t system, OTP::group_t group, OTP::point_t point);

        /**
         * @brief Emitted when a group is removed from the producer
         * 
         * @param system System containing removed point
         * @param group Group within system containing removed point
         * @param point Removed point
         */
        void removedLocalPoint(OTP::system_t system, OTP::group_t group, OTP::point_t point);

    /**@}*/ // Local Points

    /** 
     * @name Local Point Name
     * 
     * @{
     */ 
    public:
        /**
         * @brief Get a local points descriptive name
         * 
         * @param address Point address
         * @return Point name
         */
        QString getLocalPointName(address_t address) const;

        /**
         * @brief Get a local points descriptive name
         * 
         * @param system System containing point
         * @param group Group within system containing point
         * @param point Local point within group to query
         * @return Point name 
         */
        QString getLocalPointName(system_t system, group_t group, point_t point)
            { return getLocalPointName(address_t(system, group, point)); }

        /**
         * @brief Set a local points descriptive name
         * 
         * @param address Point address
         * @param name New name
         */
        void setLocalPointName(address_t address, QString name);

        /**
         * @brief Set a local points descriptive name
         * 
         * @param system System containing point
         * @param group Group within system containing point
         * @param point Local point within group to set new name
         * @param name New name
         */
        void setLocalPointName(system_t system, group_t group, point_t point, const QString &name)
            { setLocalPointName(address_t(system, group, point), name); }

    signals:
        /**
         * @brief Emitted when point is given a new descriptive name
         * 
         * @param address Point address
         */
        void updatedLocalPointName(OTP::address_t address);

    /**@}*/ // Local Point Name

    /** 
     * @name Local Point Priority
     * 
     * @{
     */ 
    public:
        /**
         * @brief Get a local points priority
         * 
         * @param address Point address
         * @return Point priority 
         */
        priority_t getLocalPointPriority(address_t address) const;

        /**
         * @brief Get a local points priority
         * 
         * @param system System containing point
         * @param group Group within system containing point
         * @param point Local point within group to query
         * @return Point priority  
         */
        priority_t getLocalPointPriority(system_t system, group_t group, point_t point)
            { return getLocalPointPriority(address_t(system, group, point)); }

        /**
         * @brief Set a local points priority
         * 
         * @param address Point address
         * @param priority New priority
         */
        void setLocalPointPriority(address_t address, priority_t priority);

        /**
         * @brief Set a local points priority
         * 
         * @param system System containing point
         * @param group Group within system containing point
         * @param point Local point within group to set new Priority
         * @param priority New priority
         */
        void setLocalPointPriority(system_t system, group_t group, point_t point, priority_t priority)
            { setLocalPointPriority(address_t(system, group, point), priority); }

    signals:
        /**
         * @brief Emitted when point is given a new priority
         * 
         * @param address Point address
         */
        void updatedLocalPointPriority(OTP::address_t address);

    /**@}*/ // Local Point Priority

    /** 
     * @name Local Addresses
     * 
     * @{
     */ 
    public:
        /**
         * @brief Get list active address for this producers
         * 
         * @return List of active address
         */
        QList<address_t> getLocalAddresses();

        /**
         * @brief Get list active address for this producers, filtered by system number
         * 
         * @param system Query only this system
         * @return List of active address
         */
        QList<address_t> getLocalAddresses(system_t system);

        /**
         * @brief Get list active address for this producers, filtered by system and group number
         * 
         * @param system Query only this system
         * @param group Query only this group within the system
         * @return List of active address
         */
        QList<address_t> getLocalAddresses(system_t system, group_t group);

    /**@}*/ // Local Point Priority

    /** 
     * @name Standard Modules - Position 
     * 
     * @{
     */  
    public:
        /**
         * @brief Point position
         * 
         */
        typedef struct PositionValue_s
        {
            MODULES::STANDARD::PositionModule_t::position_t value = 0; /**< Position value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = static_cast<timestamp_t>(QDateTime::currentDateTime().toMSecsSinceEpoch()); /**< Value sample time */
            MODULES::STANDARD::PositionModule_t::scale_t scale; /**< Value scale */
        } PositionValue_t;

        /**
         * @brief Get a local points current position
         * 
         * @param address Point address
         * @param axis Axis of point to query
         * @return Current position of points axis
         */
        PositionValue_t getLocalPosition(address_t address, axis_t axis) const;

        /**
         * @brief Set a points current position
         * 
         * @param address Point address
         * @param axis Axis of point to alter
         * @param position New position
         */
        void setLocalPosition(address_t address, axis_t axis, PositionValue_t position);

    signals:
        /**
         * @brief Emitted when point has an updated position
         * 
         * @param address Point address
         * @param axis Updated axis
         */
        void updatedPosition(OTP::address_t address, OTP::axis_t axis);

    /**@}*/ // Standard Modules - Position 

    /** 
     * @name Standard Modules - Position Velocity/Acceleration 
     * 
     * @{
     */  
    public:
        /**
         * @brief Point Position Velocity
         * 
         */
        typedef struct PositionVelocity_s
        {
            MODULES::STANDARD::PositionVelAccModule_t::velocity_t value = 0; /**< Position velocity value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
        } PositionVelocity_t;

        /**
         * @brief Get a local points current velocity
         * 
         * @param address Point address
         * @param axis Axis of point to query
         * @return Current position velocity of points axis
         */
        PositionVelocity_t getLocalPositionVelocity(address_t address, axis_t axis) const;

        /**
         * @brief Set a points current velocity
         * 
         * @param address Point address
         * @param axis Axis of point to alter
         * @param velocity New velocity
         */
        void setLocalPositionVelocity(address_t address, axis_t axis, PositionVelocity_t velocity);

        /**
         * @brief Point Position Acceleration
         * 
         */
        typedef struct PositionAcceleration_s
        {
            MODULES::STANDARD::PositionVelAccModule_t::acceleration_t value = 0; /**< Acceleration value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
        } PositionAcceleration_t;

        /**
         * @brief Get a local points current acceleration
         * 
         * @param address Point address
         * @param axis Axis of point to query
         * @return Current position acceleration of points axis
         */
        PositionAcceleration_t getLocalPositionAcceleration(address_t address, axis_t axis) const;

        /**
         * @brief Set a points current acceleration
         * 
         * @param address Point address
         * @param axis Axis of point to alter
         * @param acceleration New acceleration
         */
        void setLocalPositionAcceleration(address_t address, axis_t axis, PositionAcceleration_t acceleration);
    signals:
        /**
         * @brief Emitted when point has an updated position velocity
         * 
         * @param address Point address
         * @param axis Updated axis
         */
        void updatedPositionVelocity(OTP::address_t address, OTP::axis_t axis);

        /**
         * @brief Emitted when point has an updated position acceleration
         * 
         * @param address Point address
         * @param axis Updated axis
         */
        void updatedPositionAcceleration(OTP::address_t address, OTP::axis_t axis);

    /**@}*/ // Standard Modules - Position Velocity/Acceleration  

    /** 
     * @name Standard Modules - Rotation
     * 
     * @{
     */  
    public:
        /**
         * @brief Point Rotation
         * 
         */
        typedef struct RotationValue_s
        {
            MODULES::STANDARD::RotationModule_t::rotation_t value = 0; /**< Rotation value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
        } RotationValue_t;

            /**
         * @brief Get a local points current rotation
         * 
         * @param address Point address
         * @param axis Axis of point to query
         * @return Current rotation of points axis
         */
        RotationValue_t getLocalRotation(address_t address, axis_t axis) const;

        /**
         * @brief Set a points current rotation
         * 
         * @param address Point address
         * @param axis Axis of point to alter
         * @param rotation New rotation
         */
        void setLocalRotation(address_t address, axis_t axis, RotationValue_t rotation);

    signals:
        /**
         * @brief Emitted when point has an updated rotation
         * 
         * @param address Point address
         * @param axis Updated axis
         */
        void updatedRotation(OTP::address_t address, OTP::axis_t axis);

    /**@}*/ // Standard Modules - Rotation  

    /** 
     * @name Standard Modules - Rotation Velocity/Acceleration 
     * 
     * @{
     */  
    public:
        /**
         * @brief Point Rotation Velocity
         * 
         */
        typedef struct RotationVelocity_s
        {
            MODULES::STANDARD::RotationVelAccModule_t::velocity_t value = 0; /**< Rotation Velocity value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
        } RotationVelocity_t;

        /**
         * @brief Get a local points current rotation velocity
         * 
         * @param address Point address
         * @param axis Axis of point to query
         * @return Current rotation velocity of points axis
         */
        RotationVelocity_t getLocalRotationVelocity(address_t address, axis_t axis) const;

        /**
         * @brief Set a points current rotation velocity
         * 
         * @param address Point address
         * @param axis Axis of point to alter
         * @param velocity New rotation velocity
         */
        void setLocalRotationVelocity(address_t address, axis_t axis, RotationVelocity_t velocity);

        /**
         * @brief Point Rotation Acceleration
         * 
         */
        typedef struct RotationAcceleration_s
        {
            MODULES::STANDARD::RotationVelAccModule_t::acceleration_t value = 0; /**< Rotation Acceleration value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
        } RotationAcceleration_t;

        /**
         * @brief Get a local points current rotation acceleration
         * 
         * @param address Point address
         * @param axis Axis of point to query
         * @return Current acceleration of points axis
         */
        RotationAcceleration_t getLocalRotationAcceleration(address_t address, axis_t axis) const;

        /**
         * @brief Set a points current rotation acceleration
         * 
         * @param address Point address
         * @param axis Axis of point to alter
         * @param acceleration New rotation acceleration
         */
        void setLocalRotationAcceleration(address_t address, axis_t axis, RotationAcceleration_t acceleration);
    signals:
        /**
         * @brief Emitted when point has an updated rotation velocity
         * 
         * @param address Point address
         * @param axis Updated axis
         */
        void updatedRotationVelocity(OTP::address_t address, OTP::axis_t axis);

        /**
         * @brief Emitted when point has an updated rotation acceleration
         * 
         * @param address Point address
         * @param axis Updated axis
         */
        void updatedRotationAcceleration(OTP::address_t address, OTP::axis_t axis);

    /**@}*/ // Standard Modules - Rotation Velocity/Acceleration  

    /** 
     * @name Standard Modules - Scale
     * 
     * @{
     */  
    public:
        /**
         * @brief Point Scale
         * 
         */
        typedef struct Scale_s
        {
            MODULES::STANDARD::ScaleModule_t::scale_t value = 0; /**< Scale value */

            /**
             * @brief Return the value as a percentage
             * 
             * @return Percentage
             */
            MODULES::STANDARD::ScaleModule_t::percent_t getPercent() { 
                return MODULES::STANDARD::ScaleModule_t::toPercent(value); }

            /**
             * @brief Return the value as a percentage string
             * 
             * @return Percentage string 
             */
            operator QString() { 
                return MODULES::STANDARD::ScaleModule_t::toPercentString(value).append("%"); 
            }

            /**
             * @brief Set the value from a percentage
             * 
             * @param value Percentage
             */
            void setPercent(MODULES::STANDARD::ScaleModule_t::percent_t value) {
                Scale_t::value = MODULES::STANDARD::ScaleModule_t::fromPercent(value); 
            }

            timestamp_t timestamp = 0; /**< Value sample time */
        } Scale_t;

        /**
         * @brief Get a local points current scale
         * 
         * @param address Point address
         * @param axis Axis of point to query
         * @return Current scale of points axis
         */
        Scale_t getLocalScale(address_t address, axis_t axis) const;

        /**
         * @brief Set a points current scale
         * 
         * @param address Point address
         * @param axis Axis of point to alter
         * @param scale New scale
         */
        void setLocalScale(address_t address, axis_t axis, Scale_t scale);
    signals:
        /**
         * @brief Emitted when point has an updated scale
         * 
         * @param address Point address
         * @param axis Updated axis
         */
        void updatedScale(OTP::address_t address, OTP::axis_t axis);

    /**@}*/ // Standard Modules - Scale  

    /** 
     * @name Standard Modules - Reference Frame
     * 
     * @{
     */  
    public:
        /**
         * @brief Point Reference Frame
         * 
         */
        typedef struct ReferenceFrame_t
            {
                address_t value; /**< Point Reference Frame value */
                timestamp_t timestamp = 0; /**< Value sample time */
                cid_t sourceCID; /**< Componet IDenifier of reference frame source @todo Redundant? */
            } ReferenceFrame_t;

            /**
             * @brief Get a local points reference frame
             * 
             * @param address Point address
             * @return Current reference frame of point
             */
            ReferenceFrame_t getLocalReferenceFrame(address_t address) const;

            /**
             * @brief Set the local points reference frame
             * 
             * @param address Point address
             * @param reference New reference frame
             * 
             */
            void setLocalReferenceFrame(address_t address, ReferenceFrame_t reference);
        signals:
            /**
             * @brief Emitted when point has an updated reference frame
             * 
             * @param address Point address
             */
            void updatedReferenceFrame(OTP::address_t address);

    /**@}*/ // Standard Modules - Reference Frame  

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

    }; // OTP Producer component

    /**
     * @brief OTP Consumer component
     * @details <b>Consumer:</b> A Consumer is the intended target of information from a Producer.
     * 
     */
    class OTP_LIB_EXPORT Consumer : public Component
    {
        Q_OBJECT
    public:
        /**
         * @brief Create a Consumer object
         * 
         * @param iface Network interface to utilise
         * @param transport Network transport
         * @param systems List of systems to monitor
         * @param CID Component IDentifier
         * @param name Human readable name
         * @param parent Parent object
         */
        explicit Consumer(
                QNetworkInterface iface,
                QAbstractSocket::NetworkLayerProtocol transport,
                QList<system_t> systems,
                cid_t CID = cid_t::createUuid(),
                name_t name = QCoreApplication::applicationName(),
                QObject *parent = nullptr);
        ~Consumer() {};

        /**
         * @brief Send a network request for systems and point descriptions
         * @details New systems and points emitted by Component
         */
        void UpdateOTPMap();

    /** 
     * @name Local Systems
     * 
     * @{
     */  
    public:
        void addLocalSystem(system_t system) override;
        void removeLocalSystem(system_t system) override;

    /**@}*/ // Local Systems

    /** 
     * @name Standard Modules - Helper Functions
     * 
     * @{
     */  
    private:
    /**
         * @internal
         * @brief Helper function - Get module details and values
         * @details Common items are directly reurned, module type specfics are returned by reference
         * 
         * @tparam T Return type
         * @tparam T2 Module details type
         * @param[out] module Module type specfics details
         * @param cid Componet IDenifier
         * @param address Address to query
         * @param respectRelative Respect reference frames, if any
         * @return T Details with common details
         */
        template <class T, class T2>
        T getValueHelper(T2 &, cid_t cid, address_t address, bool respectRelative = true) const;

        /**
         * @internal
         * @brief Helper function - Get a points current T from all known sources
         *
         * @tparam T Point detail type
         * @param getFunc Funtion to obtain individual componets values
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @param excludeWinner Exclude winning source?
         * @return Map of values index by Component IDentifier
         */
        template <class T>
        QMap<cid_t, T> getValuesHelper(
            std::function<T(cid_t cid, address_t address, axis_t axis, bool respectRelative)> getFunc,
            address_t address, 
            axis_t axis, 
            bool respectRelative = true, 
            bool excludeWinner = false) const;

    /**@}*/ // Standard Modules

    /** 
     * @name Standard Modules - Position
     * 
     * @{
     */  
    public:
        /**
         * @brief Point Position
         * 
         */
        typedef struct PositionValue_s
        {
            MODULES::STANDARD::PositionModule_t::position_t value = 0; /**< Position value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
            MODULES::STANDARD::PositionModule_t::scale_t scale; /**< Value scale */
            cid_t sourceCID; /**< Source component of value */
            priority_t priority; /**< Point priority @todo Redundant? */
        } PositionValue_t;

        /**
         * @brief Get the points current position, for specfic component
         * 
         * @param cid Component IDenifier to query
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current position
         */
        PositionValue_t getPosition(cid_t cid, address_t address, axis_t axis, bool respectRelative = true) const;

        /**
         * @brief Get the winning points current position
         * 
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current position
         */
        PositionValue_t getPosition(address_t address, axis_t axis, bool respectRelative = true) const;

        /**
         * @brief Get all points current position from all known sources
         * 
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @param excludeWinner Exclude winning source?
         * @return List of point positions, indexed by CID
         */
        QMap<cid_t, PositionValue_t> getPositions(address_t address, axis_t axis, bool respectRelative = true, bool excludeWinner = false) const;

    signals:  
        /**
         * @brief Emitted when the point has an updated position
         * 
         * @param cid Component IDenifier containing point
         * @param address Address of point
         * @param axis Updated axis
         */
        void updatedPosition(OTP::cid_t cid, OTP::address_t address, OTP::axis_t axis);

    /**@}*/ // Standard Modules - Position

    /** 
     * @name Standard Modules - Position Velocity/Acceleration
     * 
     * @{
     */  
    public:
        /**
         * @brief Point Position Velocity
         * 
         */
        typedef struct PositionVelocity_s
        {
            MODULES::STANDARD::PositionVelAccModule_t::velocity_t value = 0; /**< Position Velocity value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
            cid_t sourceCID; /**< Source component of value */
            priority_t priority; /**< Point priority @todo Redundant? */
        } PositionVelocity_t;

        /**
         * @brief Get the points current velocity, for specfic component
         * 
         * @param cid Component IDenifier to query
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current velocity
         */
        PositionVelocity_t getPositionVelocity(cid_t cid, address_t address, axis_t axis, bool respectRelative = true) const;
        
        /**
         * @brief Get the winning points current velocity
         * 
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current velocity
         */
        PositionVelocity_t getPositionVelocity(address_t address, axis_t axis, bool respectRelative = true) const;
        
        /**
         * @brief Get all points current velocity from all known sources
         * 
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @param excludeWinner Exclude winning source?
         * @return List of point position velocitys, indexed by CID
         */
        QMap<cid_t, PositionVelocity_t> getPositionVelocitys(address_t address, axis_t axis, bool respectRelative = true, bool excludeWinner = false) const;

        /**
         * @brief Point Position Acceleration
         * 
         */
        typedef struct PositionAcceleration_s
        {
            MODULES::STANDARD::PositionVelAccModule_t::acceleration_t value = 0; /**< Position Acceleration value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
            cid_t sourceCID; /**< Source component of value */
            priority_t priority; /**< Point priority @todo Redundant? */
        } PositionAcceleration_t;

        /**
         * @brief Get the points current acceleration, for specfic component
         * 
         * @param cid Component IDenifier to query
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current acceleration
         */
        PositionAcceleration_t getPositionAcceleration(cid_t cid, address_t address, axis_t axis, bool respectRelative = true) const;
        
        /**
         * @brief Get the winning points current acceleration
         * 
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current acceleration
         */
        PositionAcceleration_t getPositionAcceleration(address_t address, axis_t axis, bool respectRelative = true) const;
        
        /**
         * @brief Get all points current acceleration from all known sources
         * 
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @param excludeWinner Exclude winning source?
         * @return List of point position accelerations, indexed by CID
         */
        QMap<cid_t, PositionAcceleration_t> getPositionAccelerations(address_t address, axis_t axis, bool respectRelative = true, bool excludeWinner = false) const;
    signals:
        /**
         * @brief Emitted when the point has an updated position velocity
         * 
         * @param cid Component IDenifier containing point
         * @param address Address of point
         * @param axis Updated axis
         */
        void updatedPositionVelocity(OTP::cid_t cid, OTP::address_t address, OTP::axis_t axis);

        /**
         * @brief Emitted when the point has an updated position acceleration
         * 
         * @param cid Component IDenifier containing point
         * @param address Address of point
         * @param axis Updated axis
         */
        void updatedPositionAcceleration(OTP::cid_t cid, OTP::address_t address, OTP::axis_t axis);

    /**@}*/ // Standard Modules - Position Velocity/Acceleration

    /** 
     * @name Standard Modules - Rotation
     * 
     * @{
     */  
    public:
        /**
         * @brief Point Rotation
         * 
         */
        typedef struct RotationValue_s
        {
            MODULES::STANDARD::RotationModule_t::rotation_t value = 0; /**< Rotation value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
            cid_t sourceCID; /**< Source component of value */
            priority_t priority; /**< Point priority @todo Redundant? */
        } RotationValue_t;

        /**
         * @brief Get the points current rotation, for specfic component
         * 
         * @param cid Component IDenifier to query
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current rotation
         */
        RotationValue_t getRotation(cid_t cid, address_t address, axis_t axis, bool respectRelative = true) const;
        
        /**
         * @brief Get the winning points current rotation
         * 
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current rotation
         */
        RotationValue_t getRotation(address_t address, axis_t axis, bool respectRelative = true) const;
        
        /**
         * @brief Get all points current rotation from all known sources
         * 
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @param excludeWinner Exclude winning source?
         * @return List of point rotation, indexed by CID
         */
        QMap<cid_t, RotationValue_t> getRotations(address_t address, axis_t axis, bool respectRelative = true, bool excludeWinner = false) const;
    signals:
        /**
         * @brief Emitted when the point has an updated rotation
         * 
         * @param cid Component IDenifier containing point
         * @param address Address of point
         * @param axis Updated axis
         */
        void updatedRotation(OTP::cid_t cid, OTP::address_t address, OTP::axis_t axis);

    /**@}*/ // Standard Modules - Rotation

    /** 
     * @name Standard Modules - Rotation Velocity/Acceleration
     * 
     * @{
     */  
    public:
        /**
         * @brief Rotation Velocity
         * 
         */
        typedef struct RotationVelocity_s
        {
            MODULES::STANDARD::RotationVelAccModule_t::velocity_t value = 0; /**< Rotation Velocity value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
            cid_t sourceCID; /**< Source component of value */
            priority_t priority; /**< Point priority @todo Redundant? */
        } RotationVelocity_t;

        /**
         * @brief Get the points current rotation velocity, for specfic component
         *
         * @param cid Component IDenifier to query
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current rotation velocity
         */
        RotationVelocity_t getRotationVelocity(cid_t cid, address_t address, axis_t axis, bool respectRelative = true) const;

        /**
         * @brief Get the winning points current rotation velocity
         *
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current rotation velocity
         */
        RotationVelocity_t getRotationVelocity(address_t address, axis_t axis, bool respectRelative = true) const;

        /**
         * @brief Get all points current rotation velocity from all known sources
         *
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @param excludeWinner Exclude winning source?
         * @return List of point rotation velocities, indexed by CID
         */
        QMap<cid_t, RotationVelocity_t> getRotationVelocitys(address_t address, axis_t axis, bool respectRelative = true, bool excludeWinner = false) const;

        /**
         * @brief Get all points current rotation velocity from all known sources
         *
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @param excludeWinner Exclude winning source?
         * @return List of point rotation velocities, indexed by CID
         */
        QMap<cid_t, RotationVelocity_t> getRotationVelocities(address_t address, axis_t axis, bool respectRelative = true, bool excludeWinner = false) const
        { return getRotationVelocitys(address, axis, respectRelative, excludeWinner);}

        /**
         * @brief Rotation Acceleration
         * 
         */
        typedef struct RotationAcceleration_s
        {
            MODULES::STANDARD::RotationVelAccModule_t::acceleration_t value = 0; /**< Rotation Acceleration value */
            QString unit; /**< String representation of value unit */
            timestamp_t timestamp = 0; /**< Value sample time */
            cid_t sourceCID; /**< Source component of value */
            priority_t priority; /**< Point priority @todo Redundant? */
        } RotationAcceleration_t;

        /**
         * @brief Get the points current rotation acceleration, for specfic component
         *
         * @param cid Component IDenifier to query
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current rotation acceleration
         */
        RotationAcceleration_t getRotationAcceleration(cid_t cid, address_t address, axis_t axis, bool respectRelative = true) const;

        /**
         * @brief Get the winning points current rotation acceleration
         *
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @return Points current rotation acceleration
         */
        RotationAcceleration_t getRotationAcceleration(address_t address, axis_t axis, bool respectRelative = true) const;

        /**
         * @brief Get all points current rotation acceleration from all known sources
         *
         * @param address Address to query
         * @param axis Axis to query
         * @param respectRelative Respect reference frames?
         * @param excludeWinner Exclude winning source?
         * @return List of point rotation accelerations, indexed by CID
         */
        QMap<cid_t, RotationAcceleration_t> getRotationAccelerations(address_t address, axis_t axis, bool respectRelative = true, bool excludeWinner = false) const;
    signals:
        /**
         * @brief Emitted when the point has an updated rotation velocity
         *
         * @param cid Component IDenifier containing point
         * @param address Address of point
         * @param axis Updated axis
         */
        void updatedRotationVelocity(OTP::cid_t cid, OTP::address_t address, OTP::axis_t axis);

        /**
         * @brief Emitted when the point has an updated rotation acceleration
         *
         * @param cid Component IDenifier containing point
         * @param address Address of point
         * @param axis Updated axis
         */
        void updatedRotationAcceleration(OTP::cid_t cid, OTP::address_t address, OTP::axis_t axis);

    /**@}*/ // Standard Modules - Rotation Velocity/Acceleration

    /** 
     * @name Standard Modules - Scale
     * 
     * @{
     */  
    public:
        /**
         * @brief Scale
         * 
         */
        typedef struct Scale_s
        {
            MODULES::STANDARD::ScaleModule_t::scale_t value = 0; /**< Scale value */

            /**
             * @brief Return the value as a percentage
             * 
             * @return Percentage
             */
            MODULES::STANDARD::ScaleModule_t::percent_t getPercent() { 
                return MODULES::STANDARD::ScaleModule_t::toPercent(value); 
            }
            QString unit; /**< String representation of value unit */

            /**
             * @brief Return the value as a percentage string
             * 
             * @return Percentage string 
             */
            operator QString() const { 
                return MODULES::STANDARD::ScaleModule_t::toPercentString(value).append("%"); 
            }

            timestamp_t timestamp = 0; /**< Value sample time */
            cid_t sourceCID; /**< Source component of value */
            priority_t priority; /**< Point priority @todo Redundant? */
        } Scale_t;

        /**
         * @brief Get the points current scale, for specfic component
         *
         * @param cid Component IDenifier to query
         * @param address Address to query
         * @param axis Axis to query
         * @return Points current scale
         */
        Scale_t getScale(cid_t cid, address_t address, axis_t axis) const;

        /**
         * @brief Get the winning points current scale
         *
         * @param address Address to query
         * @param axis Axis to query
         * @return Points current scale
         */
        Scale_t getScale(address_t address, axis_t axis) const;

        /**
         * @brief Get all points current scale from all known sources
         *
         * @param address Address to query
         * @param axis Axis to query
         * @param excludeWinner Exclude winning source?
         * @return List of point scales, indexed by CID
         */
        QMap<cid_t, Scale_t> getScales(address_t address, axis_t axis, bool excludeWinner = false) const;
    signals:
        /**
         * @brief Emitted when the point has an updated scale
         *
         * @param cid Component IDenifier containing point
         * @param address Address of point
         * @param axis Updated axis
         */
        void updatedScale(OTP::cid_t cid, OTP::address_t address, OTP::axis_t axis);

    /**@}*/ // Standard Modules - Rotation Velocity/Acceleration

    /** 
     * @name Standard Modules - Reference Frame
     * 
     * @{
     */  
    public:
        /**
         * @brief Reference Frame
         * 
         */
        typedef struct ReferenceFrame_t
        {
            address_t value; /**< Reference Frame value */
            timestamp_t timestamp = 0; /**< Value sample time */
            cid_t sourceCID; /**< Source component of value */
            priority_t priority; /**< Point priority @todo Redundant? */
        } ReferenceFrame_t;

        /**
         * @brief Get the addresses current reference frame, for specfic component
         *
         * @param cid Component IDenifier to query
         * @param address Address to query
         * @return Addresses reference frame
         */
        ReferenceFrame_t getReferenceFrame(cid_t cid, address_t address) const;

        /**
         * @brief Get the winning addresses current reference frame
         *
         * @param address Address to query
         * @return Addresses reference frame
         */
        ReferenceFrame_t getReferenceFrame(address_t address) const;
    signals:
        /**
         * @brief Emitted when the address has an updated reference frame
         *
         * @param cid Component IDenifier containing point
         * @param address Updated address
         */
        void updatedReferenceFrame(OTP::cid_t cid, OTP::address_t address);

    /**@}*/ // Standard Modules - Reference Frame

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
