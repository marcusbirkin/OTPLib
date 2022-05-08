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
    typedef OTP::PDU::name_t name_t;
    typedef OTP::PDU::OTPLayer::cid_t cid_t;
    typedef OTP::PDU::OTPPointLayer::point_t point_t;
    typedef OTP::PDU::OTPPointLayer::group_t group_t;
    typedef OTP::PDU::OTPPointLayer::priority_t priority_t;
    typedef OTP::PDU::OTPTransformLayer::system_t system_t;
    typedef OTP::PDU::OTPTransformLayer::timestamp_t timestamp_t;
    typedef OTP::MESSAGES::OTPModuleAdvertisementMessage::list_t moduleList_t;
    typedef OTP::MODULES::STANDARD::axis_t axis_t;

    typedef struct component_s {
    public:
        component_s() {}
        typedef enum type_e
        {
            consumer,
            produder
        } type_t;
        type_t getType() { return type; }
        void setType(type_t value) { type = value; }

        name_t getName() const {return name;}
        void setName(name_t value) { name = value; updateLastSeen(); }

        QHostAddress getIPAddr() const {return ipAddr;}
        void setIPAddr(QHostAddress value) { ipAddr = value; updateLastSeen(); }

        typedef OTP::MESSAGES::OTPModuleAdvertisementMessage::item_t ModuleItem_t;
        typedef OTP::MESSAGES::OTPModuleAdvertisementMessage::list_t ModuleList_t;
        ModuleList_t getModuleList() const {
            return moduleList.keys();
        }
        void addModuleItem(const ModuleItem_t &item) {
            moduleList[item] = QDateTime::currentDateTime();
            updateLastSeen();
        }
        void removeModuleItem(const ModuleItem_t &item) {
            moduleList.remove(item);
        }
        bool isExpired(ModuleItem_t item) const;
        static QString getModuleString(ModuleItem_t item, bool includeManf = true);

        QDateTime getLastSeen() const { return lastSeen; }

    private:
        void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
        name_t name;
        QHostAddress ipAddr;
        QDateTime lastSeen;
        QMap<ModuleItem_t, QDateTime> moduleList;
        type_t type;
    } component_t;
    typedef QMap<cid_t, component_t> componentMap_t;

    typedef struct folioMap_s {
    private: auto noSystem() { return system_t().getMin() - 1; }
    public:
        bool checkSequence(cid_t cid, vector_t vector, OTPLayer::folio_t value)
        {
            return checkSequence(cid, noSystem(), vector, value);
        }

        bool checkSequence(cid_t cid, system_t system, vector_t vector,  OTPLayer::folio_t value)
        {
            auto folio = folioMap[{cid,{system, vector}}];
            auto ret = folio.checkSequence(value);
            return ret;
        }

        void addPage(
                cid_t cid,
                vector_t vector,
                OTPLayer::folio_t folio,
                OTPLayer::page_t page,
                QNetworkDatagram datagram)
        {
            addPage(cid, noSystem(), vector, folio, page, datagram);
        }

        void addPage(
                cid_t cid,
                system_t system,
                vector_t vector,
                OTPLayer::folio_t folio,
                OTPLayer::page_t page,
                QNetworkDatagram datagram);

        bool checkAllPages(
                cid_t cid,
                vector_t vector,
                OTPLayer::folio_t folio,
                OTPLayer::page_t lastPage)
        {
            return checkAllPages(cid, noSystem(), vector, folio, lastPage);
        }

        bool checkAllPages(
                cid_t cid,
                system_t system,
                vector_t vector,
                OTPLayer::folio_t folio,
                OTPLayer::page_t lastPage);

        QVector<QNetworkDatagram> getDatagrams(
                cid_t cid,
                vector_t vector,
                OTPLayer::folio_t folio)
        {
            return getDatagrams(cid, noSystem(), vector, folio);
        }

        QVector<QNetworkDatagram> getDatagrams(
                cid_t cid,
                system_t system,
                vector_t vector,
                OTPLayer::folio_t folio);

    private:
        typedef std::pair<cid_t, std::pair<system_t, vector_t>> key_t;
        struct folioMapPrivate_s {
            folioMapPrivate_s() : folio(0) {}
            folioMapPrivate_s(OTPLayer::folio_t folio)
                : folio(folio) {}

            bool checkSequence(OTPLayer::folio_t value) { return folio.checkSequence(value); }

            OTPLayer::folio_t folio;
            QVector<OTPLayer::page_t> pages;
            QVector<QNetworkDatagram> datagrams;
        };
        QMap<key_t, folioMapPrivate_s> folioMap;
    } folioMap_t;

    class pointDetails
    {
    public:
        pointDetails() : lastSeen(QDateTime::currentDateTime()) {}
        pointDetails(priority_t priority) :
            lastSeen(QDateTime::currentDateTime()),
            priority(priority) {}
        pointDetails(QString name, priority_t priority) :
            name(name),
            lastSeen(QDateTime::currentDateTime()),
            priority(priority) {}

        name_t getName() const { return name; }
        void setName(name_t value) { name = value; updateLastSeen(); }

        QDateTime getLastSeen() const { return std::max(lastSeen, standardModules.getLastSeen()); }
        void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
        bool isExpired();

        priority_t getPriority() const { return priority; }
        void setPriority(priority_t value) { priority = value; updateLastSeen(); }

        typedef struct standardModules {
        public:
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

            MODULES::STANDARD::PositionModule_t position;
            MODULES::STANDARD::PositionVelAccModule_t positionVelAcc;
            MODULES::STANDARD::RotationModule_t rotation;
            MODULES::STANDARD::RotationVelAccModule_t rotationVelAcc;
            MODULES::STANDARD::ScaleModule_t scale;
            MODULES::STANDARD::ReferenceFrameModule_t referenceFrame;
        } standardModules_t;

        standardModules_t standardModules;

    private:
        QString name;
        QDateTime lastSeen;
        priority_t priority;
    };
    typedef std::shared_ptr<pointDetails> pointDetails_t;

    typedef struct OTP_LIB_EXPORT address_s {
        address_s() :
            system(),
            group(),
            point() {}
        address_s(system_t system, group_t group, point_t point) :
            system(system),
            group(group),
            point(point) {}

        bool isValid();
        QString toString() { return QString("%1/%2/%3").arg(system).arg(group).arg(point); }

        system_t system;
        group_t group;
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

    typedef QHash<point_t, pointDetails_t> pointMap_t;
    typedef QHash<group_t, pointMap_t> groupMap_t;
    typedef QHash<system_t, groupMap_t> systemMap_t;
    typedef QHash<cid_t, systemMap_t> addressMap_t;

    typedef struct range_s {
        range_s() : min(0), max(0) {}
        range_s(qint64 min, qint64 max) : min(min), max(max) {}

        qint64 getMin() const { return min; }
        qint64 getMax() const { return max; }
        bool isValid(const qint64 value) const
        {
            return ((value >= min) && (value <= max));
        }
    private:
        qint64 min, max;

    } range_t;
}

#endif // TYPES_HPP
