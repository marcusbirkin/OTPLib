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
#ifndef OTP_SYSTEM_ADVERTISEMENT_MESSAGE_HPP
#define OTP_SYSTEM_ADVERTISEMENT_MESSAGE_HPP

#include <QObject>
#include <QNetworkDatagram>
#include "message_types.hpp"
#include "message_const.hpp"
#include "../pdu/pdu.hpp"

namespace OTP::MESSAGES::OTPSystemAdvertisementMessage
{

class Message : public QObject
{
    Q_OBJECT
public:
    explicit Message(
            OTP::mode_e mode,
            cid_t CID,
            name_t ComponentName,
            list_t SystemList,
            QObject *parent = nullptr);
    explicit Message(
            QNetworkDatagram message,
            QObject *parent = nullptr);
    bool isValid();
    QNetworkDatagram toQNetworkDatagram(
            QHostAddress destAddr,
            folio_t folio,
            page_t thisPage,
            page_t lastPage);
    QList<QNetworkDatagram> toQNetworkDatagrams(
            QAbstractSocket::NetworkLayerProtocol transport,
            folio_t folio,
            page_t thisPage,
            page_t lastPage)
    {
        QList<QNetworkDatagram> ret;
        if ((transport == QAbstractSocket::IPv4Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
            ret.append(toQNetworkDatagram(
                            OTP_Advertisement_Message_IPv4,
                            folio,
                            thisPage,
                            lastPage));
        if ((transport == QAbstractSocket::IPv6Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
            ret.append(toQNetworkDatagram(
                            OTP_Advertisement_Message_IPv6,
                            folio,
                            thisPage,
                            lastPage));
        return ret;
    }

    bool addItem(item_t value) {
        auto ret = systemAdvertisementLayer->addItem(value);
        updatePduLength();
        return ret;
    }

    std::shared_ptr<OTP::PDU::OTPLayer::Layer> getOTPLayer() { return otpLayer;}
    std::shared_ptr<OTP::PDU::OTPAdvertisementLayer::Layer> getAdvertisementLayer() { return advertisementLayer; }
    std::shared_ptr<OTP::PDU::OTPSystemAdvertisementLayer::Layer> getSystemAdvertisementLayer() {return systemAdvertisementLayer; }

private:
    void updatePduLength();
    QByteArray toByteArray();

    std::shared_ptr<OTP::PDU::OTPLayer::Layer> otpLayer;
    std::shared_ptr<OTP::PDU::OTPAdvertisementLayer::Layer> advertisementLayer;
    std::shared_ptr<OTP::PDU::OTPSystemAdvertisementLayer::Layer> systemAdvertisementLayer;
};

} // namespace

#endif // OTP_SYSTEM_ADVERTISEMENT_MESSAGE_HPP
