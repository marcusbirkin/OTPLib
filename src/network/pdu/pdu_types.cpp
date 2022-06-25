/*
    OTPLib
    E1.59 (Entertainment Technology Object Transform Protocol (OTP)) QT interface
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
#include "pdu_types.hpp"
#include "pdu_const.hpp"
#include "../../const.hpp"
#include "esta_mfcrids.hpp"
#include <QtEndian>

namespace OTP::PDU {

    PDUByteArray& operator<<(PDUByteArray &l, const quint8 &r)
    {
        l.append(static_cast<char>(r));
        return l;
    }
    PDUByteArray& operator<<(PDUByteArray &l, const quint16 &r)
    {
        return l << quint8(r >> 8) << quint8(r);
    }
    PDUByteArray& operator<<(PDUByteArray &l, const quint32 &r)
    {
        return l << quint16(r >> 16) << quint16(r);
    }
    PDUByteArray& operator<<(PDUByteArray &l, const quint64 &r)
    {
        return l << quint32(r >> 32) << quint32(r);
    }


    PDUByteArray& operator<<(PDUByteArray &l, const qint8 &r)
    {
        l.append(static_cast<char>(r));
        return l;
    }
    PDUByteArray& operator<<(PDUByteArray &l, const qint16 &r)
    {
        return l << qint8(r >> 8) << qint8(r);
    }
    PDUByteArray& operator<<(PDUByteArray &l, const qint32 &r)
    {
        return l << qint16(r >> 16) << qint16(r);
    }
    PDUByteArray& operator<<(PDUByteArray &l,  qint64 &r)
    {
        return l << qint32(r >> 32) << qint32(r);
    }

    PDUByteArray& operator>>(PDUByteArray &l, quint8 &r)
    {
        r = static_cast<quint8>(l.front());
        l.remove(0, sizeof(quint8));
        return l;
    }
    PDUByteArray& operator>>(PDUByteArray &l, quint16 &r)
    {
        r = qFromBigEndian<quint16>(l.data());
        l.remove(0, sizeof(quint16));
        return l;
    }
    PDUByteArray& operator>>(PDUByteArray &l, quint32 &r)
    {
        r = qFromBigEndian<quint32>(l.data());
        l.remove(0, sizeof(quint32));
        return l;
    }
    PDUByteArray& operator>>(PDUByteArray &l, quint64 &r)
    {
        r = qFromBigEndian<quint64>(l.data());
        l.remove(0, sizeof(quint64));
        return l;
    }

    PDUByteArray& operator>>(PDUByteArray &l, qint8 &r)
    {
        r = static_cast<qint8>(l.front());
        l.remove(0, sizeof(qint8));
        return l;
    }
    PDUByteArray& operator>>(PDUByteArray &l, qint16 &r)
    {
        r = qFromBigEndian<qint16>(l.data());
        l.remove(0, sizeof(qint16));
        return l;
    }
    PDUByteArray& operator>>(PDUByteArray &l, qint32 &r)
    {
        r = qFromBigEndian<qint32>(l.data());
        l.remove(0, sizeof(qint32));
        return l;
    }
    PDUByteArray& operator>>(PDUByteArray &l, qint64 &r)
    {
        r = qFromBigEndian<qint64>(l.data());
        l.remove(0, sizeof(qint64));
        return l;
    }

    name_t::name_t() : QByteArray() { fill(0, NAME_LENGTH); }
    name_t::name_t(const QByteArray &ba) : name_t()
    {
        replace(0,
            std::min(static_cast<size_t>(ba.size()), NAME_LENGTH),
            ba.mid(0, NAME_LENGTH));
    }
    name_t::name_t(const QString &s) : name_t() { fromString(s); }

    int name_t::maxSize() { return NAME_LENGTH; };
    bool name_t::isValid() const {
        if (static_cast<size_t>(toString().toUtf8().size()) > NAME_LENGTH)
            return false;
        if (this->data() != toString().toUtf8())
            return false;
        return true;
    }
    QString name_t::toString() const
    {
        return QString::fromUtf8(this->data());
    }
    void name_t::fromString(const QString &s)
    {
        replace(0,
            std::min(static_cast<size_t>(s.toUtf8().size()), NAME_LENGTH),
            s.toUtf8().mid(0, NAME_LENGTH));
    }
    PDUByteArray& operator<<(PDUByteArray &l, const name_t &r)
    {
        Q_ASSERT(r.size() == NAME_LENGTH);
        l.append(r);
        return l;
    }
    PDUByteArray& operator>>(PDUByteArray &l, name_t &r)
    {
        if (l.size() < NAME_LENGTH)
            return l;
        r = l.left(NAME_LENGTH);
        l.remove(0, NAME_LENGTH);
        return l;
    }

    namespace OTPLayer {
        PDUByteArray& operator<<(PDUByteArray &l, const otpIdent_t &r)
        {
            auto r_copy = r;
            r_copy.resize(OTP_PACKET_IDENT.size());
            l.append(r_copy);
            return l;
        }
        PDUByteArray& operator>>(PDUByteArray &l, otpIdent_t &r)
        {
            r = l.left(OTP_PACKET_IDENT.size());
            l.remove(0, OTP_PACKET_IDENT.size());
            return l;
        }

        cid_t cid_t::createUuid()
        {
            return cid_t(QUuid::createUuid());
        }

        PDUByteArray& operator<<(PDUByteArray &l, const cid_t &r)
        {
            l.append(r.toRfc4122());
            return l;
        }
        PDUByteArray& operator>>(PDUByteArray &l, cid_t &r)
        {
            r = QUuid::fromRfc4122(l.left(r.toRfc4122().length()));
            l.remove(0, r.toRfc4122().length());
            return l;
        }

        bool folio_t::checkSequence(folio_t value)
        {
            folio_t::type A = data;
            folio_t::type B = value;
            quint32 test = A-B;

            return test != std::clamp(
                        test,
                        static_cast<decltype(test)>(0),
                        static_cast<decltype(test)>(63335));;
        }

        PDUByteArray& operator>>(PDUByteArray &l, folio_t &r)
        {
            l >> r.data;
            return l;
        }
        folio_t& folio_t::operator++()
        {
            data++;
            return *this;
        }
        folio_t folio_t::operator++(int)
        {
            auto tmp(*this);
            operator++();
            return tmp;
        }
    }

    namespace OTPTransformLayer {
        system_t::system_t() : system_t(getMin() - 1) {}
        bool system_t::isValid() const { return RANGES::System.isValid(data); }
        system_t system_t::getMin() { return static_cast<system_t>(RANGES::System.getMin()); }
        system_t system_t::getMax() { return static_cast<system_t>(RANGES::System.getMax()); }
        PDUByteArray& operator>>(PDUByteArray &l, system_t &r)
        {
            l >> r.data;
            return l;
        }
        OTPModuleLayer::additional_t& operator>>(OTPModuleLayer::additional_t &l, system_t &r)
        {
            l >> r.data;
            return l;
        }
        system_t& system_t::operator++()
        {
            if (data >= getMax())
                data = getMin();
            else
                data++;
            return *this;
        }
        system_t system_t::operator++(int)
        {
            system_t tmp(*this);
            operator++();
            return tmp;
        }
        system_t& system_t::operator--()
        {
            if (data <= getMin())
                data = getMax();
            else
                data--;
            return *this;
        }
        system_t system_t::operator--(int)
        {
            system_t tmp(*this);
            operator--();
            return tmp;
        }
    }

    namespace OTPPointLayer {
        bool priority_t::isValid() const { return RANGES::Priority.isValid(data); }
        priority_t priority_t::getMin() { return static_cast<priority_t>(RANGES::Priority.getMin()); }
        priority_t priority_t::getMax() { return static_cast<priority_t>(RANGES::Priority.getMax()); }
        PDUByteArray& operator>>(PDUByteArray &l, priority_t &r)
        {
            l >> r.data;
            return l;
        }
        priority_t& priority_t::operator++()
        {
            if (data >= getMax())
                data = getMin();
            else
                data++;
            return *this;
        }
        priority_t priority_t::operator++(int)
        {
            priority_t tmp(*this);
            operator++();
            return tmp;
        }
        priority_t& priority_t::operator--()
        {
            if (data <= getMin())
                data = getMax();
            else
                data--;
            return *this;
        }
        priority_t priority_t::operator--(int)
        {
            priority_t tmp(*this);
            operator--();
            return tmp;
        }

        group_t::group_t() : group_t(getMin() - 1) {}
        bool group_t::isValid() const { return RANGES::Group.isValid(data); }
        group_t group_t::getMin() { return static_cast<group_t>(RANGES::Group.getMin()); }
        group_t group_t::getMax() { return static_cast<group_t>(RANGES::Group.getMax()); }
        PDUByteArray& operator>>(PDUByteArray &l, group_t &r)
        {
            l >> r.data;
            return l;
        }
        OTPModuleLayer::additional_t& operator>>(OTPModuleLayer::additional_t &l, group_t &r)
        {
            l >> r.data;
            return l;
        }
        group_t& group_t::operator++()
        {
            if (data >= getMax())
                data = getMin();
            else
                data++;
            return *this;
        }
        group_t group_t::operator++(int)
        {
            group_t tmp(*this);
            operator++();
            return tmp;
        }
        group_t& group_t::operator--()
        {
            if (data <= getMin())
                data = getMax();
            else
                data--;
            return *this;
        }
        group_t group_t::operator--(int)
        {
            group_t tmp(*this);
            operator--();
            return tmp;
        }

        point_t::point_t() : point_t(RANGES::Point.getMin() - 1) {}
        bool point_t::isValid() const { return RANGES::Point.isValid(data); }
        point_t point_t::getMin() { return static_cast<point_t>(RANGES::Point.getMin()); }
        point_t point_t::getMax() { return static_cast<point_t>(RANGES::Point.getMax()); }
        PDUByteArray& operator>>(PDUByteArray &l, point_t &r)
        {
            l >> r.data;
            return l;
        }
        OTPModuleLayer::additional_t& operator>>(OTPModuleLayer::additional_t &l, point_t &r)
        {
            l >> r.data;
            return l;
        }
        point_t& point_t::operator++()
        {
            if (data >= getMax())
                data = getMin();
            else
                data++;
            return *this;
        }
        point_t point_t::operator++(int)
        {
            point_t tmp(*this);
            operator++();
            return tmp;
        }
        point_t& point_t::operator--()
        {
            if (data <= getMin())
                data = getMax();
            else
                data--;
            return *this;
        }
        point_t point_t::operator--(int)
        {
            point_t tmp(*this);
            operator--();
            return tmp;
        }
    }

    namespace OTPModuleLayer {
        bool manufacturerID_t::isValid(bool allowPrototype, bool allowUnkown) const {
            using namespace ESTA::ManufacturerIDs;
            if (!allowUnkown)
                if (Manufacturers.find(data) != Manufacturers.end())
                    return false;
            if (!allowPrototype)
                if (data >= PROTOTYPE_BEGIN && data <= PROTOTYPE_END)
                    return false;
            return true;
        }
        PDUByteArray& operator>>(PDUByteArray &l, manufacturerID_t &r)
        {
            l >> r.data;
            return l;
        }

        bool moduleNumber_t::isValid() const {
            return true;
        }
        PDUByteArray& operator>>(PDUByteArray &l, moduleNumber_t &r)
        {
            l >> r.data;
            return l;
        }

        bool ident_t::isValid() const {
            return ManufacturerID.isValid() &&
                    ModuleNumber.isValid();
        }
        size_t ident_t::getSize() const {
            PDUByteArray temp;
            temp << *this;
            return temp.size();
        }
        PDUByteArray& operator<<(PDUByteArray &l, const ident_t &r)
        {
            return l << r.ManufacturerID << r.ModuleNumber;
        }
        PDUByteArray& operator>>(PDUByteArray &l, ident_t &r)
        {
            l >> r.ManufacturerID >> r.ModuleNumber;
            return l;
        }

        PDUByteArray& operator<<(PDUByteArray &l, const additional_t &r)
        {
            l.append(r);
            return l;
        }
        PDUByteArray& operator>>(PDUByteArray &l, additional_t &r)
        {
            r.clear();
            r.append(l);
            l.clear();
            return l;
        }

        additional_t& operator<<(additional_t &l, const quint8 &r)
        {
            l.append(static_cast<char>(r));
            return l;
        }
        additional_t& operator<<(additional_t &l, const quint16 &r)
        {
            return l << quint8(r >> 8) << quint8(r);
        }
        additional_t& operator<<(additional_t &l, const quint32 &r)
        {
            return l << quint16(r >> 16) << quint16(r);
        }
        additional_t& operator<<(additional_t &l, const quint64 &r)
        {
            return l << quint32(r >> 32) << quint32(r);
        }

        additional_t& operator<<(additional_t &l, const qint8 &r)
        {
            l.append(static_cast<char>(r));
            return l;
        }
        additional_t& operator<<(additional_t &l, const qint16 &r)
        {
            return l << qint8(r >> 8) << qint8(r);
        }
        additional_t& operator<<(additional_t &l, const qint32 &r)
        {
            return l << qint16(r >> 16) << qint16(r);
        }
        additional_t& operator<<(additional_t &l, const qint64 &r)
        {
            return l << qint32(r >> 32) << qint32(r);
        }

        additional_t& operator>>(additional_t &l, quint8 &r)
        {
            r = qFromBigEndian<quint8>(l.data());
            l.remove(0, sizeof(r));
            return l;
        }
        additional_t& operator>>(additional_t &l, quint16 &r)
        {
            r = qFromBigEndian<quint16>(l.data());
            l.remove(0, sizeof(r));
            return l;
        }
        additional_t& operator>>(additional_t &l, quint32 &r)
        {
            r = qFromBigEndian<quint32>(l.data());
            l.remove(0, sizeof(r));
            return l;
        }
        additional_t& operator>>(additional_t &l, quint64 &r)
        {
            r = qFromBigEndian<quint64>(l.data());
            l.remove(0, sizeof(r));
            return l;
        }

        additional_t& operator>>(additional_t &l, qint8 &r)
        {
            r = qFromBigEndian<qint8>(l.data());
            l.remove(0, sizeof(r));
            return l;
        }
        additional_t& operator>>(additional_t &l, qint16 &r)
        {
            r = qFromBigEndian<qint16>(l.data());
            l.remove(0, sizeof(r));
            return l;
        }
        additional_t& operator>>(additional_t &l, qint32 &r)
        {
            r = qFromBigEndian<qint32>(l.data());
            l.remove(0, sizeof(r));
            return l;
        }
        additional_t& operator>>(additional_t &l, qint64 &r)
        {
            r = qFromBigEndian<qint64>(l.data());
            l.remove(0, sizeof(r));
            return l;
        }
    }

    namespace OTPModuleAdvertisementLayer {
        PDUByteArray& operator<<(PDUByteArray &l, const list_t &r)
        {
            for (const auto &item : r)
            {
                l << item;
            }
            return l;
        }
        PDUByteArray& operator>>(PDUByteArray &l, list_t &r)
        {
            while (l.size() >= list_t::value_type().getSize())
            {
                list_t::value_type item;
                l >> item;
                r.append(item);
            }
            return l;
        }
    }

    namespace OTPNameAdvertisementLayer  {
        bool addressPointDescriptions_t::isValid() const {
            return System.isValid() &&
                    Group.isValid() &&
                    Point.isValid() &&
                    PointName.isValid();
        }
        size_t addressPointDescriptions_t::getSize() const {
            PDUByteArray temp;
            temp << *this;
            return temp.size();
        }
        PDUByteArray& operator<<(PDUByteArray &l, const addressPointDescriptions_t &r)
        {
            l << r.System << r.Group << r.Point << r.PointName;
            return l;
        }
        PDUByteArray& operator>>(PDUByteArray &l, addressPointDescriptions_t &r)
        {
            r.Group = 0;
            l >> r.System >> r.Group >> r.Point >> r.PointName;
            return l;
        }

        PDUByteArray& operator<<(PDUByteArray &l, const list_t &r)
        {
            for (const auto &item : r)
            {
                l << item;
            }
            return l;
        }
        PDUByteArray& operator>>(PDUByteArray &l, list_t &r)
        {
            while (l.size() >= list_t::value_type().getSize())
            {
                list_t::value_type item;
                l >> item;
                r.append(item);
            }
            return l;
        }
    }

    namespace OTPSystemAdvertisementLayer {
        PDUByteArray& operator<<(PDUByteArray &l, const list_t &r)
        {
            for (const auto &item : r)
            {
                l << item;
            }
            return l;
        }
        PDUByteArray& operator>>(PDUByteArray &l, list_t &r)
        {
            while (l.size())
            {
                list_t::value_type item;
                l >> item;
                r.append(item);
            }
            return l;
        }
    }
}
