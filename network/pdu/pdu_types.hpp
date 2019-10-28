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

#include <QByteArray>
#include <QDataStream>
#include <QUuid>
#include <QtEndian>
#include <QHostAddress>

namespace ACN::OTP::PDU
{
    class PDUByteArray;

    typedef struct {
        typedef quint8 flags_t;
        typedef quint32 pduLength_t;
        flags_t Flags;
        pduLength_t PDULength;
    } flags_length_t;
    PDUByteArray& operator<<(PDUByteArray &l, const flags_length_t &r);
    PDUByteArray& operator>>(PDUByteArray &l, flags_length_t &r);
    typedef quint32 vector_t;

    class name_t : public QByteArray
    {
    public:
        name_t() : QByteArray() {}
        name_t(const QByteArray &ba) : QByteArray(ba) {}
        name_t(const QString &s) : QByteArray() { fromString(s); }
        static int maxSize();
        name_t& operator=(const name_t& r);
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
        inline flags_length_t::pduLength_t size() const {
            return static_cast<flags_length_t::pduLength_t>(QByteArray::size());
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

    namespace OTPRootLayer {
        typedef quint16 ambleSize_t;
        class acnIdent_t : public QByteArray
        {
        public:
            acnIdent_t() : QByteArray() {}
            acnIdent_t(const QByteArray &ba) : QByteArray(ba) {}
        };
        PDUByteArray& operator<<(PDUByteArray &l, const acnIdent_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, acnIdent_t &r);
        class cid_t : public QUuid
        {
        public:
            cid_t() : QUuid() {}
            cid_t(const QUuid &id) : QUuid(id) {}
        };
        PDUByteArray& operator<<(PDUByteArray &l, const cid_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, cid_t &r);
    }

    namespace OTPLayer {
        typedef quint16 protocol_t;

        class sequence_t {
            typedef quint16 type;
        public:
            sequence_t() : data(0) {}
            sequence_t(type value) : data(value) {}
            operator type() const { return data; }
            bool checkSequence(sequence_t value);
            friend PDUByteArray& operator>>(PDUByteArray &l, sequence_t &r);
            sequence_t& operator++();
            sequence_t operator++(int);
        private:
            type data;
        };
        typedef quint16 folio_t;
        typedef quint8 page_t;
        typedef quint8 options_t;
        typedef quint32 reserved_t;
    }

    namespace OTPTransformLayer {
        class system_t
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
            bool isValid();
            static system_t getMin();
            static system_t getMax();
            size_t getSize();
            operator type() const { return data; }
            friend PDUByteArray& operator>>(PDUByteArray &l, system_t &r);
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

        /* TODO Check this value size with ratified standard
         *
         * For some unholy reason, a timestamp is 16 bytes aka 128bit
         * 2_128 = 340,282,366,920,938,463,463,374,607,431,768,211,456uS
         * Wolfram tells me that ≈ 7.8×10^14 × age of the universe
         */
        #ifdef __SIZEOF_INT128__ // GCC and Clang Extension
            typedef struct timestamp_s
            {
                typedef __uint128_t type;
            public:
                timestamp_s() : data(0){}
                timestamp_s(type value) : data(value) {}
                operator type() const { return data; }
                operator QString() const { return QString::number(quint64(data)); } // Hacky, but enough
                friend PDUByteArray& operator<<(PDUByteArray &l, const timestamp_s &r)
                {
                    return l << quint64(r >> 64) << quint64(r);
                }
                friend PDUByteArray& operator>>(PDUByteArray &l, timestamp_s &r)
                {
                    __uint128_t hi = qFromBigEndian<quint64>(l.data()) << (sizeof(quint64) / 2);
                    l.remove(0, sizeof(quint64));
                    __uint128_t lo = qFromBigEndian<quint64>(l.data());
                    l.remove(0, sizeof(quint64));
                    r = hi | lo;
                    return l;
                }
            private:
                type data;
            } timestamp_t;
        #else
            typedef struct timestamp_s
            {
                typedef quint64 type;
            public:
                timestamp_s() : hi(0), lo(0) {}
                timestamp_s(type value) : hi(0), lo(value) {}
                timestamp_s(type hi, type lo) : hi(hi), lo(lo) {}
                operator type() const { return lo; }
                operator QString() const { return QString::number(lo); } // Hacky, but enough
                friend PDUByteArray& operator<<(PDUByteArray &l, const timestamp_s &r)
                {
                    return l << r.hi << r.lo;
                }
                friend PDUByteArray& operator>>(PDUByteArray &l, timestamp_s &r)
                {
                    l >> r.hi;
                    l >> r.lo;
                    return l;
                }
            private:
                type hi;
                type lo;
            } timestamp_t;
        #endif

        typedef quint8 options_t;
        typedef quint32 reserved_t;
    }


    namespace OTPPointLayer {
        class group_t
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
            bool isValid();
            static group_t getMin();
            static group_t getMax();
            operator type() const { return data; }
            friend PDUByteArray& operator>>(PDUByteArray &l, group_t &r);
            group_t& operator++();
            group_t operator++(int);
            group_t& operator--();
            group_t operator--(int);
        private:
            type data;
        };


        class point_t
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
            operator type() const { return data; }
            friend PDUByteArray& operator>>(PDUByteArray &l, point_t &r);
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
        typedef struct vector_s
        {
            typedef quint16 manufacturerID_t;
            typedef quint16 moduleNumber_t;
            vector_s() :
                ManufacturerID(0),
                ModuleNumber(0) {}
            vector_s(manufacturerID_t ManufacturerID, moduleNumber_t ModuleNumber) :
                ManufacturerID(ManufacturerID),
                ModuleNumber(ModuleNumber) {}
            size_t getSize();
            manufacturerID_t ManufacturerID;
            moduleNumber_t ModuleNumber;
        } vector_t;
        PDUByteArray& operator<<(PDUByteArray &l, const vector_t &r);
        PDUByteArray& operator>>(PDUByteArray &l, vector_t &r);
        inline bool operator==(const vector_t &l, const vector_t &r) {
            return (
                (l.ModuleNumber == r.ModuleNumber) &&
                (l.ManufacturerID == r.ManufacturerID));
        }
        inline bool operator!=(const vector_t &l, const vector_t &r) { return !(l == r); };
        inline bool operator< (const vector_t& l, const vector_t& r){
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
        typedef OTPModuleLayer::vector_t item_t;
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
            options_s() : options(0) {}
            bool isRequest() const {return ((options & REQUEST_RESPONSE_MASK) >> REQUEST_RESPONSE_SHIFT) == REQUEST;}
            void setRequest() {options &= ~REQUEST_RESPONSE_MASK; }
            bool isResponse() const { return ((options & REQUEST_RESPONSE_MASK) >> REQUEST_RESPONSE_SHIFT) == RESPONSE;}
            void setResponse() {options |= REQUEST_RESPONSE_MASK; }
            friend PDUByteArray& operator<<(PDUByteArray &l, const options_s &r) { l << r.options; return l; }
            friend PDUByteArray& operator>>(PDUByteArray &l, options_s &r) { l >> r.options; return l; }
            options_s& operator=(const options_s& r) {this->options = r.options; return *this; }
        private:
            const quint8 REQUEST_RESPONSE_SHIFT = 7;
            const quint8 REQUEST_RESPONSE_MASK = 1 << REQUEST_RESPONSE_SHIFT;
            enum : quint8
            {
                REQUEST = 0,
                RESPONSE = 1
            };
            quint8 options;
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
            size_t getSize();
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
        inline bool operator!=(const addressPointDescriptions_t &l, const addressPointDescriptions_t &r) { return !(l == r); }
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
