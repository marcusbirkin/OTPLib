/**
 * @file        types.hpp
 * @brief       Custom types for OTP
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
 *  but WITHOUT ANYs WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef TYPES_HPP
#define TYPES_HPP

#include "network/pdu/pdu_types.hpp"
#include "network/modules/modules_types.hpp"
#include <memory>
#include <QMap>
#include <QList>
#include <QVector>
#include <QHostAddress>
#include <QDateTime>
#include <QTimer>
#include <QNetworkDatagram>

namespace OTP
{
    /*! Creates a type name for OTP::PDU::name_t */ 
    typedef OTP::PDU::name_t name_t;
    /*! Creates a type name for OTP::PDU::OTPLayer::cid_t */ 
    typedef OTP::PDU::OTPLayer::cid_t cid_t;
    /*! Creates a type name for OTP::PDU::OTPPointLayer::point_t */ 
    typedef OTP::PDU::OTPPointLayer::point_t point_t;
    /*! Creates a type name for OTP::PDU::OTPPointLayer::group_t */ 
    typedef OTP::PDU::OTPPointLayer::group_t group_t;
    /*! Creates a type name for OTP::PDU::OTPPointLayer::priority_t */ 
    typedef OTP::PDU::OTPPointLayer::priority_t priority_t;
    /*! Creates a type name for OTP::PDU::OTPTransformLayer::system_t */ 
    typedef OTP::PDU::OTPTransformLayer::system_t system_t;
    /*! Creates a type name for OTP::PDU::OTPTransformLayer::timestamp_t */ 
    typedef OTP::PDU::OTPTransformLayer::timestamp_t timestamp_t;
    /*! Creates a type name for OTP::MESSAGES::OTPModuleAdvertisementMessage::list_t */ 
    typedef OTP::MESSAGES::OTPModuleAdvertisementMessage::list_t moduleList_t;
    /*! Creates a type name for OTP::MODULES::STANDARD::axis_t */ 
    typedef OTP::MODULES::STANDARD::axis_t axis_t;

    class Container;

    /**
     * @brief Container type for all infomation about a component
     * 
     */
    typedef struct component_t {
        /// @cond DOXYGEN_IGNORE
        friend Container;
        /// @endcond
    public:
        component_t() {}

        /**
         * @brief Component operating type
         * 
         */
        typedef enum type_e
        {
            consumer, /**< A Consumer is the intended target of information from a Producer. */
            produder /**< A Producer is any network device transmitting OTP Transform Messages. */
        } type_t;

        /**
         * @brief Get the components operating type
         * 
         * @return Consumer or Producer
         */
        type_t getType() const { return type; }

        /**
         * @internal
         * @brief Set the components operating type
         * 
         * @param value Consumer or Producer
         */
        void setType(type_t value) { type = value; }

        /**
         * @brief Get the components human readable name
         * 
         * @return Components name 
         */
        name_t getName() const { return name; }

        /**
         * @internal
         * @brief Set the components human readable name
         * 
         * @param value New component name 
         */
        void setName(name_t value) { name = value; updateLastSeen(); }

        /**
         * @brief Get the components IP Address
         * 
         * @return Components IP Address 
         */
        QHostAddress getIPAddr() const { return ipAddr; }

        /**
         * @internal
         * @brief Set the components IP Address
         * 
         * @param value New IP Address
         */
        void setIPAddr(QHostAddress value) { ipAddr = value; updateLastSeen(); }

        /*! Creates a type name for OTP::MESSAGES::OTPModuleAdvertisementMessage::item_t */ 
        typedef OTP::MESSAGES::OTPModuleAdvertisementMessage::item_t ModuleItem_t;
        /*! Creates a type name for OTP::MESSAGES::OTPModuleAdvertisementMessage::list_t */ 
        typedef OTP::MESSAGES::OTPModuleAdvertisementMessage::list_t ModuleList_t;

        /**
         * @brief Get the components modules
         * 
         * @return List of modules 
         */
        ModuleList_t getModuleList() const {
            return moduleList.keys();
        }

        /**
         * @brief Add a module to the component
         * 
         * @param item Module item to add
         */
        void addModuleItem(const ModuleItem_t &item) {
            moduleList[item] = QDateTime::currentDateTime();
            updateLastSeen();
        }

        /**
         * @brief Remove a module from the component
         * 
         * @param item Module to remove
         */
        void removeModuleItem(const ModuleItem_t &item) {
            moduleList.remove(item);
        }

        /**
         * @brief Has the module expired
         * 
         * @param item Module to query
         * @return true Module has expired
         * @return false Module has not expired
         */
        bool isExpired(ModuleItem_t item) const;

        /**
         * @brief Get Human readable Module Identifier (Manufacturer ID and Module Name)
         * 
         * @param item Module to query
         * @param includeManf Include Manufacters name
         * @return Module Description 
         */
        static QString getModuleString(ModuleItem_t item, bool includeManf = true);

        /**
         * @brief Get the time this component was last heard from
         * 
         * @return Last activity time
         */
        QDateTime getLastSeen() const { return lastSeen; };

        /**
         * @brief Is the component still active
         * 
         * @return true Component has expired and is offline
         * @return false Component is still active and online
         */
        bool isExpired() const;

    protected:
        /**
         * @internal
         * @brief Update the last seen time for this component
         * @details Used for component expiry checking
         * 
         */
        void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }

    private:
        name_t name;
        QHostAddress ipAddr;
        QDateTime lastSeen;
        QMap<ModuleItem_t, QDateTime> moduleList;
        type_t type;
    } component_t;
    /**
     * @internal
     * @brief Container of all known components, indexed by Component IDentifier
     * 
     */
    typedef QMap<cid_t, component_t> componentMap_t;

    /**
     * @internal
     * @brief Folio container
     * @details Used to assemble and access a folios split over several datagram
     * 
     * <b>OTP Folio:</b> An OTP Folio is a set of OTP Messages with the same folio number. 
     * Together they make up a snapshot of information which due to its size has to be split across multiple messages.
     * 
     */
    typedef struct folioMap_t {
    private: auto noSystem() { return system_t().getMin() - 1; }
    public:
        /**
         * @brief Check Folio sequence
         * @details 
         * The Folio Number for the relevant OTP Advertisement Message type shall be incremented by one 
         * for every OTP Folio sent of that type. 
         * The Folio Number for the relevant System shall be incremented by one for every OTP Folio sent for that System.
         * Folio Numbers shall roll over from 0xFFFFFFFF to 0x00000000.
         * 
         * @param cid Component IDentifier of folio source
         * @param vector OTP Advertisement Message type
         * @param value Folio sequence to compare against previos folio message
         * @return true Value is next in sequence
         * @return false Value is out of sequence
         */
        bool checkSequence(cid_t cid, vector_t vector, OTPLayer::folio_t value)
        {
            return checkSequence(cid, noSystem(), vector, value);
        }

        /**
         * @brief Check Folio sequence, for a specific system
         * @details 
         * The Folio Number for the relevant OTP Advertisement Message type shall be incremented by one 
         * for every OTP Folio sent of that type. 
         * The Folio Number for the relevant System shall be incremented by one for every OTP Folio sent for that System.
         * Folio Numbers shall roll over from 0xFFFFFFFF to 0x00000000.
         * 
         * @param cid Component IDentifier of folio source
         * @param system System to query
         * @param vector OTP Advertisement Message type
         * @param value Folio sequence to compare against previos folio message
         * @return true Value is next in sequence
         * @return false Value is out of sequence
         */
        bool checkSequence(cid_t cid, system_t system, vector_t vector, OTPLayer::folio_t value)
        {
            auto folio = folioMap[{cid,{system, vector}}];
            auto ret = folio.checkSequence(value);
            return ret;
        }

        /**
         * @brief Add a new folio page to the container
         * 
         * @param cid Component IDentifier of folio source
         * @param vector OTP Advertisement Message type
         * @param folio Folio page to add
         * @param page Page number
         * @param datagram Raw network datagram
         */
        void addPage(
                cid_t cid,
                vector_t vector,
                OTPLayer::folio_t folio,
                OTPLayer::page_t page,
                QNetworkDatagram datagram)
        {
            addPage(cid, noSystem(), vector, folio, page, datagram);
        }

        /**
         * @brief Add a new folio page to the container, for a specfic system
         * 
         * @param cid Component IDentifier of folio source
         * @param system System of folio source
         * @param vector OTP Advertisement Message type
         * @param folio Folio page to add
         * @param page Page number
         * @param datagram Raw network datagram
         */
        void addPage(
                cid_t cid,
                system_t system,
                vector_t vector,
                OTPLayer::folio_t folio,
                OTPLayer::page_t page,
                QNetworkDatagram datagram);

        /**
         * @brief Check we have all the expected pages for this folio
         * 
         * @param cid Component IDentifier of folio source
         * @param vector OTP Advertisement Message type
         * @param folio Folio to check
         * @param lastPage Last page number
         * @return true All pages are present
         * @return false Pages are missing
         */
        bool checkAllPages(
                cid_t cid,
                vector_t vector,
                OTPLayer::folio_t folio,
                OTPLayer::page_t lastPage)
        {
            return checkAllPages(cid, noSystem(), vector, folio, lastPage);
        }

        /**
         * @brief Check we have all the expected pages for this folio, for a specfic system
         * 
         * @param cid Component IDentifier of folio source
         * @param system System of folio source
         * @param vector OTP Advertisement Message type
         * @param folio Folio to check
         * @param lastPage Last page number
         * @return true All pages are present
         * @return false Pages are missing
         */
        bool checkAllPages(
                cid_t cid,
                system_t system,
                vector_t vector,
                OTPLayer::folio_t folio,
                OTPLayer::page_t lastPage);

        /**
         * @brief Get the Datagrams for this Folio
         * 
         * @param cid Component IDentifier of folio source
         * @param vector OTP Advertisement Message type
         * @param folio Folio to retrieve
         * @return List of datagrams
         */
        QVector<QNetworkDatagram> getDatagrams(
                cid_t cid,
                vector_t vector,
                OTPLayer::folio_t folio)
        {
            return getDatagrams(cid, noSystem(), vector, folio);
        }

        /**
         * @brief Get the Datagrams for this Folio
         * 
         * @param cid Component IDentifier of folio source
         * @param system System of folio source
         * @param vector OTP Advertisement Message type
         * @param folio Folio to retrieve
         * @return List of datagrams
         */
        QVector<QNetworkDatagram> getDatagrams(
                cid_t cid,
                system_t system,
                vector_t vector,
                OTPLayer::folio_t folio);

        /**
         * @brief Remove all folios owned by a specfic component
         * 
         * @param cid Component IDentifier of folio source
         */
        void removeComponent(cid_t cid);

    private:
        typedef std::pair<cid_t, std::pair<system_t, vector_t>> key_t;
        struct folioMapPrivate_t {
            folioMapPrivate_t() : folio(0) {}
            explicit folioMapPrivate_t(OTPLayer::folio_t folio)
                : folio(folio) {}

            bool checkSequence(OTPLayer::folio_t value) { return folio.checkSequence(value); }

            OTPLayer::folio_t folio;
            QVector<OTPLayer::page_t> pages;
            QVector<QNetworkDatagram> datagrams;
        };
        /**
         * @internal
         * @brief Underlying container of all known folios
         * 
         */
        QMap<key_t, folioMapPrivate_t> folioMap; 
    } folioMap_t;

    /**
     * @internal
     * @brief Details on a point
     * @details Address details are indended to be carried by a parent container
     * 
     */
    class pointDetails
    {
    public:
        pointDetails() : lastSeen(QDateTime()) {}
        /**
         * @brief Construct a new point details object
         * 
         * @param priority Points priority
         */
        pointDetails(priority_t priority) :
            lastSeen(QDateTime::currentDateTime()),
            priority(priority) {}

        /**
         * @brief Construct a new point details object
         * 
         * @param name Points human readable name
         * @param priority Points priority 
         */
        pointDetails(const QString &name, priority_t priority) :
            name(name),
            lastSeen(QDateTime::currentDateTime()),
            priority(priority) {}

        /**
         * @brief Get the points human readable name
         * 
         * @return Points name 
         */
        name_t getName() const { return name; }

        /**
         * @brief Set the points human readable name
         * 
         * @param value New point name 
         */
        void setName(name_t value) { name = value; updateLastSeen(); }

        /**
         * @brief Get the time this point was last active or updated
         * 
         * @return Time of last activity 
         */
        QDateTime getLastSeen() const { return std::max(lastSeen, standardModules.getLastSeen()); }

        /**
         * @brief Update the last active time for the point
         * 
         */
        void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }

        /**
         * @brief Is the point still active
         * 
         * @return true point is still active
         * @return false point has expired
         */
        bool isExpired() const;

        /**
         * @brief Get the points priority
         * 
         * @return Point priority 
         */
        priority_t getPriority() const { return priority; }

        /**
         * @brief Set the points priority
         * 
         * @param value New point priority
         */
        void setPriority(priority_t value) { priority = value; updateLastSeen(); }

        /**
         * @brief Details on standard modules for a single point
         * @details 
         * These Standard Modules each define different sets of information about a Point.
         * There are no requirements as to what Modules a Producer may support. For any given Point, Producers may only support some Modules.
         * 
         */
        typedef struct standardModules {
        public:
            /**
             * @brief Get the most recent module timestamp for the point
             * @details The Timestamp is the time, in microseconds since the Time Origin, 
             * that the information in this message was sampled from the Point. 
             * Unless identical to the generation time, it shall not be the same as time that this message was sent
             * 
             * @return Most recent timestamp 
             */
            timestamp_t getTimestamp() const
            {
                timestamp_t ret(0);
                ret = std::max(ret, position.getTimestamp());
                ret = std::max(ret, positionVelAcc.getTimestamp());
                ret = std::max(ret, rotation.getTimestamp());
                ret = std::max(ret, rotationVelAcc.getTimestamp());
                ret = std::max(ret, scale.getTimestamp());
                ret = std::max(ret, referenceFrame.getTimestamp());
                return ret;
            }

            /**
             * @brief et the most recent module activity for the point
             * 
             * @return Most recent activity time 
             */
            QDateTime getLastSeen() const
            {
                QDateTime ret;
                ret = std::max(ret, position.getLastSeen());
                ret = std::max(ret, positionVelAcc.getLastSeen());
                ret = std::max(ret, rotation.getLastSeen());
                ret = std::max(ret, rotationVelAcc.getLastSeen());
                ret = std::max(ret, scale.getLastSeen());
                ret = std::max(ret, referenceFrame.getLastSeen());
                return ret;
            }

            /**
             * @internal
             * @brief Get the most recent module data of type
             * 
             * @tparam T Module type to retrieve
             * @return T& Module data
             */
            template <class T>
            T &getModule() {
                using namespace MODULES::STANDARD;
                if constexpr (std::is_base_of<PositionModule_t, T>())
                    return position;

                if constexpr (std::is_base_of<PositionVelAccModule_t, T>())
                    return positionVelAcc;

                if constexpr (std::is_base_of<RotationModule_t, T>())
                    return rotation;

                if constexpr (std::is_base_of<RotationVelAccModule_t, T>())
                    return rotationVelAcc;

                if constexpr (std::is_base_of<ScaleModule_t, T>())
                    return scale;

                if constexpr (std::is_base_of<ReferenceFrameModule_t, T>())
                    return referenceFrame;
            }

            /**
             * @brief Get the most recent module data of type
             * 
             * @tparam T Module type to retrieve
             * @return T& Module data
             */
            template <class T>
            const T &getModule() const {
                using namespace MODULES::STANDARD;
                if constexpr (std::is_base_of<PositionModule_t, T>())
                    return position;

                if constexpr (std::is_base_of<PositionVelAccModule_t, T>())
                    return positionVelAcc;

                if constexpr (std::is_base_of<RotationModule_t, T>())
                    return rotation;

                if constexpr (std::is_base_of<RotationVelAccModule_t, T>())
                    return rotationVelAcc;

                if constexpr (std::is_base_of<ScaleModule_t, T>())
                    return scale;

                if constexpr (std::is_base_of<ReferenceFrameModule_t, T>())
                    return referenceFrame;
            }

            /**
             * @brief Most recent Position Module data
             * 
             */
            MODULES::STANDARD::PositionModule_t position;
            /**
             * @brief Most recent Position Velocity/Acceleration Module data
             * 
             */
            MODULES::STANDARD::PositionVelAccModule_t positionVelAcc;
            /**
             * @brief Most recent Rotation Module data
             * 
             */
            MODULES::STANDARD::RotationModule_t rotation;
            /**
             * @brief Most recent Rotation Velocity/Acceleration Module data
             * 
             */
            MODULES::STANDARD::RotationVelAccModule_t rotationVelAcc;
            /**
             * @brief Most recent Scale data
             * 
             */
            MODULES::STANDARD::ScaleModule_t scale;
            /**
             * @brief Most recent Reference Frame data
             * 
             */
            MODULES::STANDARD::ReferenceFrameModule_t referenceFrame;
        } standardModules_t;

        /**
         * @brief Standard module data for this point
         * 
         */
        standardModules_t standardModules;

    private:
        QString name;
        QDateTime lastSeen;
        priority_t priority;
    };
    /**
     * @internal
     * @brief Point details type
     * 
     */
    typedef std::shared_ptr<pointDetails> pointDetails_t;

    /**
     * @brief A collection of System, Group, and point forming an Address
     * @details
     * <b>Address:</b> The combination of System Number (OTPTransformLayer::system_t),
     * Group Number (OTPPointLayer::group_t),
     * and Point Number (OTPPointLayer::point_t) make up the Address which identifies a Point.
     */
    typedef struct OTP_LIB_EXPORT address_t {
        address_t() :
            system(),
            group(),
            point() {}
        /**
         * @brief Construct a new address object
         * 
         * @param system System of address
         * @param group Group of adderss
         * @param point Point of address
         */
        address_t(system_t system, group_t group, point_t point) :
            system(system),
            group(group),
            point(point) {}

        /**
         * @brief Is the address valid
         * @details Does the System, Group, and point; all fall within allowed ranges
         * 
         * @return true Address is valid
         * @return false Address is not valid
         */
        bool isValid();

        /**
         * @brief Convert the address to standard string format
         * @details 
         * Addresses shall be notated...using a ‘/’ to separate each number. 
         * 
         * System Number | Group Number | Point Number | Address
         * :-----------: | :----------: | :----------: | :-----:
         * 1             | 1            | 1            | 1/1/1
         * 1             | 1            | 2            | 1/1/2
         * 1             | 2            | 3            | 1/2/3
         * 
         * @return QString 
         */
        QString toString() { return QString("%1/%2/%3").arg(system).arg(group).arg(point); }

        /**
         * @brief System number
         * 
         */
        system_t system;

        /**
         * @brief Group number
         * 
         */
        group_t group;

        /**
         * @brief Point number
         * 
         */
        point_t point;
    } address_t;
    inline bool operator< (const address_t& l, const address_t& r){
        return ((l.system < r.system) || (l.group < r.group) || (l.point < r.point));
    }
    inline bool operator> (const address_t& l, const address_t& r){ return r < l; }
    inline bool operator<=(const address_t& l, const address_t& r){ return !(l > r); }
    inline bool operator>=(const address_t& l, const address_t& r){ return !(l < r); }
    inline bool operator==(const address_t& l, const address_t& r)
        { return ((l.system == r.system) && (l.group == r.group) && (l.point == r.point)); }
    inline bool operator!=(const address_t& l, const address_t& r) { return !(l == r); }
    inline uint qHash(const address_t &key, uint seed = 0)
    {
        #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            auto address = {static_cast<unsigned int>(key.system),
                            static_cast<unsigned int>(key.group),
                            static_cast<unsigned int>(key.point)};
            return qHashRange(address.begin(), address.end(), seed);
        #else
            return qHashMulti(seed, key.system, key.group, key.point);
        #endif
    }

    /**
     * @internal
     * @brief Map type of point details by point
     * 
     */
    typedef QHash<point_t, pointDetails_t> pointMap_t;

    /**
     * @internal
     * @brief Map type of points by group
     * 
     */
    typedef QHash<group_t, pointMap_t> groupMap_t;

    /**
     * @internal
     * @brief Map type of groups by system
     * 
     */
    typedef QHash<system_t, groupMap_t> systemMap_t;

    /**
     * @internal
     * @brief Map type of systems by CID
     * 
     */
    typedef QHash<cid_t, systemMap_t> addressMap_t;

    /**
     * @internal
     * @brief Range type
     * @details Container and verification of type allowed ranges
     * 
     */
    typedef struct range_t {
        range_t() : min(0), max(0) {}

        /**
         * @brief Construct a new range t object
         * 
         * @param min Minimum value allowed in range
         * @param max Maximum value allowed in range
         */
        range_t(qint64 min, qint64 max) : min(min), max(max) {}

        /**
         * @brief Get the minimum allowed value
         * 
         * @return Minimum value in range 
         */
        qint64 getMin() const { return min; }

        /**
         * @brief Get the maximum allowed value
         * 
         * @return Maximum value in range 
         */
        qint64 getMax() const { return max; }

        /**
         * @brief Check value is within range
         * 
         * @param value Value to check
         * @return true Within allowed range
         * @return false Not within allowed range
         */
        bool isValid(const qint64 value) const
        {
            return ((value >= min) && (value <= max));
        }
    private:
        qint64 min, max;

    } range_t;
}

#endif // TYPES_HPP
