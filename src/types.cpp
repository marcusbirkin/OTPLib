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
#include "types.hpp"
#include "const.hpp"
#include "network/modules/modules_const.hpp"

namespace OTP
{
    void folioMap_s::addPage(
        cid_t cid,
        system_t system,
        OTP::PDU::vector_t vector,
        OTP::PDU::OTPLayer::folio_t folio,
        OTP::PDU::OTPLayer::page_t page,
        QNetworkDatagram datagram)
    {
        key_t key = {cid, {system, vector}};
        if (folioMap[key].folio != folio)
        {
            folioMap[key].pages.clear();
            folioMap[key].datagrams.clear();
            folioMap[key].folio = folio;
        }
        folioMap[key].pages.append(page);
        folioMap[key].datagrams.append(datagram);
    }

    bool folioMap_s::checkAllPages(
            cid_t cid,
            system_t system,
            OTP::PDU::vector_t vector,
            OTP::PDU::OTPLayer::folio_t folio,
            OTP::PDU::OTPLayer::page_t lastPage)
    {
        key_t key = {cid, {system, vector}};
        if (folioMap[key].folio != folio) return false;
        if (folioMap[key].pages.count() != (lastPage + 1)) return false;
        for (int page = 0; page <= lastPage; page++)
        {
            if (!folioMap[key].pages.contains(page)) return false;
        }
        return true;
    }

    QVector<QNetworkDatagram> folioMap_s::getDatagrams(
            cid_t cid,
            system_t system,
            OTP::PDU::vector_t vector,
            OTP::PDU::OTPLayer::folio_t folio)
    {
        key_t key = {cid, {system, vector}};
        if (folioMap[key].folio != folio) return QVector<QNetworkDatagram>();
        return folioMap[key].datagrams;
    }

    void folioMap_s::removeComponent(cid_t cid)
    {
        const auto keys = folioMap.keys();
        for (const auto &key : keys) {
            if (key.first == cid)
                folioMap.remove(key);
        }
    }

    bool pointDetails::isExpired() const {
        return (QDateTime::currentDateTime()
                > getLastSeen().addMSecs(
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        OTP_TRANSFORM_DATA_LOSS_TIMEOUT).count()));
    }

    bool address_t::isValid()
    {
        if (!system.isValid()) return false;
        if (!group.isValid()) return false;
        if (!point.isValid()) return false;
        return true;
    }

    bool component_s::isExpired(ModuleItem_t item) const
    {
        if (!moduleList.contains(item)) return true;
        return (QDateTime::currentDateTime()
                > moduleList.value(item).addMSecs(
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        OTP_ADVERTISEMENT_TIMEOUT).count()));
    }

    bool component_s::isExpired() const {
        return (QDateTime::currentDateTime()
                > getLastSeen().addMSecs(
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        OTP_COMPONENT_TIMEOUT).count()));
    }

    QString component_s::getModuleString(ModuleItem_t item, bool includeManf) {
        for (const auto &module : OTP::MODULES::getSupportedModules())
        {
            QString ret;
            if (module == item)
            {
                auto moduleStrings = MODULES::getModuleDescription(module);
                if (includeManf) ret.append(QString("%1 ").arg(moduleStrings.Manufactuer));
                ret.append(moduleStrings.Name);
                return ret;
            }
        }
        return QString("Unknown Manuf: 0x%1 Module: 0x%2")
                .arg(item.ManufacturerID, 4, 16, QChar(0))
                .arg(item.ModuleNumber, 4, 16, QChar(0));
    }
}
