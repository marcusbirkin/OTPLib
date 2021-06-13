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
#ifndef PDU_TYPES_HPP
#define PDU_TYPES_HPP

#include "../../bugs.hpp"
#include <QByteArray>
#include <QUuid>
#include <QtEndian>
#include <QHostAddress>
#include <bitset>

#if defined MAKE_OTP_LIB
    #define OTP_LIB_EXPORT Q_DECL_EXPORT
#else
    #define OTP_LIB_EXPORT Q_DECL_IMPORT
#endif

namespace OTP::PDU
{
    class PDUByteArray;
    namespace OTPModuleLayer{
        class additional_t;
    }

    typedef quint16 pduLength_t;
    typedef quint16 vector_t;

    class OTP_LIB_EXPORT name_t : public QByteArray
    {
    public:
        name_t();
        name_t(const QByteArray &ba);
        name_t(const QString &s);
        static int maxSize();
        QString toString() const;
        void fromString(const QString &s);
    };
    PDUByteArray& operator<<(PDUByteArray &l, const name_t &r);
    PDUByteArray& operator>>(PDUByteArray &l, name_t &r);
    inline bool operator==(const name_t &l, const name_t &r) {
        return (l.toString().toUtf8() == r.toString().toUtf8());
    }
    inline bool operator!=(const name_t &l, const name_t &r) { return !(l == r); }


    class PDUByteArray : public QByteArray
    {
    public:
        PDUByteArray() : QByteArray() {}
        inline pduLength_t size() const {
            return static_cast<pduLength_t>(QByteArray::size());
        }
    };
    PDUByteArray& operator<<(PDUByteArray &l, const quint8 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const quint16 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const quint32 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const quint64 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const qint8 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const qint16 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const qint32 &r);
    PDUByteArray& operator<<(PDUByteArray &l, const qint64 &r);
    PDUByteArray& operator>>(PDUByteArray &l, quint8 &r);
    PDUByteArray& operator>>(PDUByteArray &l, quint16 &r);
    PDUByteArray& operator>>(PDUByteArray &l, quint32 &r);
    PDUByteArray& operator>>(PDUByteArray &l, quint64 &r);
    PDUByteArray& operator>>(PDUByteArray &l, qint8 &r);
    PDUByteArray& operator>>(PDUByteArray &l, qint16 &r);
    PDUByteArray& operator>>(PDUByteArray &l, qint32 &r);
    PDUByteArray& operator>>(PDUByteArray &l, qint64 &r);

    namespace OTPLayer {
        class otpIdent_t : public QByteArray
        {
        public:
            otpIdent_t() : QByteArray() {}
            otpIdent_t(const QByteArray &ba) : QByteArray(ba) {}
        };
        PDUByteArray& operator<<(PDUByteArray &l, const otpIdent_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, otpIdent_t &r);
        class cid_t : public QUuid
        {
        public:
            cid_t() : QUuid() {}
            cid_t(const QUuid &id) : QUuid(id) {}
        };
        PDUByteArray& operator<<(PDUByteArray &l, const cid_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, cid_t &r);

        class footer_t {
            typedef quint8 flags_t;
            typedef quint8 length_t;
            typedef QByteArray type;
        public:
            footer_t() : flags(0) {}
            footer_t(flags_t flags, length_t length = 0) : flags(flags) { footer.resize(length); }
            operator type() const { return footer; }

            flags_t &Flags() { return flags; }
            const flags_t &constFlags() const { return flags; }
            length_t getLength() const { return footer.size(); }
            void setLength(length_t value) { footer.resize(value); }
        private:
            flags_t flags;
            type footer;
        };

        class folio_t {
            typedef quint32 type;
        public:
            folio_t() : data(0) {}
            folio_t(type value) : data(value) {}
            operator type() const { return data; }
            bool checkSequence(folio_t value);
            friend PDUByteArray& operator>>(PDUByteArray &l, folio_t &r);
            folio_t& operator++();
            folio_t operator++(int);
        private:
            type data;
        };

        typedef quint16 page_t;
        typedef quint8 options_t;
        typedef quint32 reserved_t;
    }

    namespace OTPTransformLayer {
        class OTP_LIB_EXPORT system_t
        {
            typedef quint8 type;
        public:
            system_t();
            system_t(type value) : data(value) {}
            explicit system_t(quint64 value) : data(static_cast<type>(value)) {}
            explicit system_t(quint32 value) : data(static_cast<type>(value)) {}
            explicit system_t(quint16 value) : data(static_cast<type>(value)) {}
            explicit system_t(qint64 value) : data(static_cast<type>(value)) {}
            explicit system_t(qint32 value) : data(static_cast<type>(value)) {}
            explicit system_t(qint16 value) : data(static_cast<type>(value)) {}
            explicit system_t(qint8 value) : data(static_cast<type>(value)) {}
            bool isValid() const;
            static system_t getMin();
            static system_t getMax();
            size_t getSize() const { return sizeof(type); }
            operator type() const { return data; }
            friend PDUByteArray& operator>>(PDUByteArray &l, system_t &r);
            friend OTPModuleLayer::additional_t& operator>>(OTPModuleLayer::additional_t &l, system_t &r);
            system_t& operator++();
            system_t operator++(int);
            system_t& operator--();
            system_t operator--(int);
        private:
            type data;
        };
        inline Q_IPV6ADDR operator+(Q_IPV6ADDR l, const system_t& r)
        {
            l.c[15] += r;
            return l;
        }

        typedef quint64 timestamp_t;
        typedef struct options_s
        {
            options_s() : data(0) {}
            options_s(bool FullPointSet) : data(0) {
                setFullPointSet(FullPointSet);
            }
            bool isFullPointSet() const { return data[FULL_POINT_SET_BIT]; }
            void setFullPointSet(bool value) { data[FULL_POINT_SET_BIT] = value; }
            friend PDUByteArray& operator<<(PDUByteArray &l, const options_s &r) {
                l << type(r.data.to_ulong());
                return l;
            }
            friend PDUByteArray& operator>>(PDUByteArray &l, options_s &r) {
                type temp;
                l >> temp;
                r.data = std::bitset<bitWidth>(temp);
                return l;
            }
        private:
            typedef quint8 type;
            static const quint8 bitWidth = sizeof(type) * 8;
            enum {
                FULL_POINT_SET_BIT = 7
            };
            std::bitset<bitWidth> data;
        } options_t;
        typedef quint32 reserved_t;
    }


    namespace OTPPointLayer {
        class OTP_LIB_EXPORT priority_t
        {
            typedef quint8 type;
        public:
            priority_t() : data(defaultValue) {}
            priority_t(type value) : data(value) {}
            explicit priority_t(quint64 value) : data(static_cast<type>(value)) {}
            explicit priority_t(quint32 value) : data(static_cast<type>(value)) {}
            explicit priority_t(qint64 value) : data(static_cast<type>(value)) {}
            explicit priority_t(qint32 value) : data(static_cast<type>(value)) {}
            explicit priority_t(qint16 value) : data(static_cast<type>(value)) {}
            bool isValid() const;
            static priority_t getMin();
            static priority_t getMax();
            size_t getSize() const { return sizeof(type); }
            operator type() const { return data; }
            friend PDUByteArray& operator>>(PDUByteArray &l, priority_t &r);
            priority_t& operator++();
            priority_t operator++(int);
            priority_t& operator--();
            priority_t operator--(int);
        private:
            static const type defaultValue = 100;
            type data;
        };

        class OTP_LIB_EXPORT group_t
        {
            typedef quint16 type;
        public:
            group_t();
            group_t(type value) : data(value) {}
            explicit group_t(quint64 value) : data(static_cast<type>(value)) {}
            explicit group_t(quint32 value) : data(static_cast<type>(value)) {}
            explicit group_t(qint64 value) : data(static_cast<type>(value)) {}
            explicit group_t(qint32 value) : data(static_cast<type>(value)) {}
            explicit group_t(qint16 value) : data(static_cast<type>(value)) {}
            bool isValid() const;
            static group_t getMin();
            static group_t getMax();
            size_t getSize() const { return sizeof(type); }
            operator type() const { return data; }
            friend PDUByteArray& operator>>(PDUByteArray &l, group_t &r);
            friend OTPModuleLayer::additional_t& operator>>(OTPModuleLayer::additional_t &l, group_t &r);
            group_t& operator++();
            group_t operator++(int);
            group_t& operator--();
            group_t operator--(int);
        private:
            type data;
        };


        class OTP_LIB_EXPORT point_t
        {
            typedef quint32 type;
        public:
            point_t();
            point_t(type value) : data(value) {}
            explicit point_t(quint64 value) : data(static_cast<type>(value)) {}
            explicit point_t(qint64 value) : data(static_cast<type>(value)) {}
            explicit point_t(qint32 value) : data(static_cast<type>(value)) {}
            bool isValid();
            static point_t getMin();
            static point_t getMax();
            size_t getSize() const { return sizeof(type); }
            operator type() const { return data; }
            friend PDUByteArray& operator>>(PDUByteArray &l, point_t &r);
            friend OTPModuleLayer::additional_t& operator>>(OTPModuleLayer::additional_t &l, point_t &r);
            point_t& operator++();
            point_t operator++(int);
            point_t& operator--();
            point_t operator--(int);
        private:
            type data;
        };

        typedef quint64 timestamp_t;
        typedef quint8 options_t;
        typedef quint32 reserved_t;
    }

    namespace OTPModuleLayer {
        typedef struct ident_s
        {
            typedef quint16 manufacturerID_t;
            typedef quint16 moduleNumber_t;
            ident_s() :
                ManufacturerID(0),
                ModuleNumber(0) {}
            ident_s(manufacturerID_t ManufacturerID, moduleNumber_t ModuleNumber) :
                ManufacturerID(ManufacturerID),
                ModuleNumber(ModuleNumber) {}
            size_t getSize();
            manufacturerID_t ManufacturerID;
            moduleNumber_t ModuleNumber;
        } ident_t;
        PDUByteArray& operator<<(PDUByteArray &l, const ident_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, ident_t &r);
        inline bool operator==(const ident_t &l, const ident_t &r) {
            return (
                (l.ModuleNumber == r.ModuleNumber) &&
                (l.ManufacturerID == r.ManufacturerID));
        }
        inline bool operator!=(const ident_t &l, const ident_t &r) { return !(l == r); };
        inline bool operator< (const ident_t& l, const ident_t& r){
            return ((l.ManufacturerID < r.ManufacturerID) || (l.ModuleNumber < r.ModuleNumber));
        }

        class additional_t : public QByteArray
        {
        public:
            additional_t() : QByteArray() {}
            additional_t(const QByteArray &ba) : QByteArray(ba) {}
        };
        PDUByteArray& operator<<(PDUByteArray &l, const additional_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, additional_t &r);
        additional_t& operator<<(additional_t &l, const quint8 &r);
        additional_t& operator<<(additional_t &l, const quint16 &r);
        additional_t& operator<<(additional_t &l, const quint32 &r);
        additional_t& operator<<(additional_t &l, const quint64 &r);
        additional_t& operator<<(additional_t &l, const qint8 &r);
        additional_t& operator<<(additional_t &l, const qint16 &r);
        additional_t& operator<<(additional_t &l, const qint32 &r);
        additional_t& operator<<(additional_t &l, const qint64 &r);
        additional_t& operator>>(additional_t &l, quint8 &r);
        additional_t& operator>>(additional_t &l, quint16 &r);
        additional_t& operator>>(additional_t &l, quint32 &r);
        additional_t& operator>>(additional_t &l, quint64 &r);
        additional_t& operator>>(additional_t &l, qint8 &r);
        additional_t& operator>>(additional_t &l, qint16 &r);
        additional_t& operator>>(additional_t &l, qint32 &r);
        additional_t& operator>>(additional_t &l, qint64 &r);
    }

    namespace OTPAdvertisementLayer {
        typedef quint32 reserved_t;
    }

    namespace OTPModuleAdvertisementLayer {
        typedef quint32 reserved_t;
        typedef OTPModuleLayer::ident_t item_t;
        typedef QList<item_t> list_t;
        PDUByteArray& operator<<(PDUByteArray &l, const list_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, list_t &r);
        inline bool operator==(const list_t &l, const list_t &r) {
            if (l.count() != r.count()) return false;
            for (int n = 0; n < l.count(); n++)
                if (l.at(n) != r.at(n)) return false;
            return true;
        }
    }

    namespace OTPNameAdvertisementLayer {
        typedef struct options_s
        {
            options_s() : data(0) {}
            bool isRequest() const { return data[REQUEST_RESPONSE_BIT] == REQUEST; }
            void setRequest() { data[REQUEST_RESPONSE_BIT] = REQUEST; }
            bool isResponse() const { return data[REQUEST_RESPONSE_BIT] == RESPONSE;}
            void setResponse() { data[REQUEST_RESPONSE_BIT] = RESPONSE; }
            friend PDUByteArray& operator<<(PDUByteArray &l, const options_s &r) {
                l << type(r.data.to_ulong());
                return l;
            }
            friend PDUByteArray& operator>>(PDUByteArray &l, options_s &r) {
                type temp;
                l >> temp;
                r.data = std::bitset<bitWidth>(temp);
                return l;
            }
            friend bool operator==(const options_s &l, const options_s &r) {
                return l.data == r.data;
            }
        private:
            typedef quint8 type;
            static const quint8 bitWidth = sizeof(type) * 8;
            enum {
                REQUEST_RESPONSE_BIT = 7
            };
            enum
            {
                REQUEST = 0,
                RESPONSE = 1
            };
            std::bitset<bitWidth> data;
        } options_t;

        typedef quint32 reserved_t;
        typedef struct addressPointDescriptions_s
        {
            addressPointDescriptions_s() :
                System(0),
                Group(0),
                Point(0),
                PointName(QString("")) {}
            addressPointDescriptions_s(
                    OTPTransformLayer::system_t System,
                    OTPPointLayer::group_t Group,
                    OTPPointLayer::point_t Point,
                    name_t PointName) :
                System(System),
                Group(Group),
                Point(Point),
                PointName(PointName) {}
            size_t getSize() const;
            OTPTransformLayer::system_t System;
            OTPPointLayer::group_t Group;
            OTPPointLayer::point_t Point;
            name_t PointName;
        } addressPointDescriptions_t;
        PDUByteArray& operator<<(PDUByteArray &l, const addressPointDescriptions_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, addressPointDescriptions_t &r);
        inline bool operator==(const addressPointDescriptions_t &l, const addressPointDescriptions_t &r) {
            PDUByteArray la, ra;
            la << l;
            ra << r;
            return la == ra;
        }
        inline bool operator!=(const addressPointDescriptions_t &l, const addressPointDescriptions_t &r) {
            return !(l == r);
        }
        inline bool operator<(const addressPointDescriptions_t &l, const addressPointDescriptions_t &r) {
            return (
              l.System < r.System &&
              l.Group < r.Group &&
              l.Point < r.Point
            );
        }
        inline bool operator>(const addressPointDescriptions_t &l, const addressPointDescriptions_t &r) {
            return (
              l.System > r.System &&
              l.Group > r.Group &&
              l.Point > r.Point
            );
        }
        typedef addressPointDescriptions_t item_t;
        typedef QList<item_t> list_t;
        PDUByteArray& operator<<(PDUByteArray &l, const list_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, list_t &r);
    }

    namespace OTPSystemAdvertisementLayer {
        typedef OTPNameAdvertisementLayer::options_t options_t;
        typedef quint32 reserved_t;
        typedef OTPTransformLayer::system_t item_t;
        typedef QList<item_t> list_t;
        PDUByteArray& operator<<(PDUByteArray &l, const list_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, list_t &r);
     }
};

#endif // PDU_TYPES_HPP
