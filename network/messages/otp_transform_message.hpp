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
#ifndef OTP_TRANSFORM_MESSAGE_HPP
#define OTP_TRANSFORM_MESSAGE_HPP

#include <QObject>
#include <QNetworkDatagram>
#include "message_types.hpp"
#include "message_const.hpp"
#include "../pdu/pdu.hpp"

namespace OTP::MESSAGES::OTPTransformMessage
{

class Message : public QObject
{
    Q_OBJECT
public:
    explicit Message(
            cid_t CID,
            name_t ComponentName,
            system_t System,
            bool FullPointSet,
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
                            QHostAddress(OTP_Transform_Message_IPv4.toIPv4Address()
                                         + transformLayer->getSystem()),
                            folio,
                            thisPage,
                            lastPage));
        if ((transport == QAbstractSocket::IPv6Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
            ret.append(toQNetworkDatagram(
                            QHostAddress(OTP_Transform_Message_IPv6.toIPv6Address()
                                        + transformLayer->getSystem()),
                            folio,
                            thisPage,
                            lastPage));
        return ret;
    }

    typedef enum {
        OK,
        InvalidSystem,
        InvalidAdditional,
        InvalidTimestamp,
        MessageToBig
    } addModule_ret;
    typedef struct {
        OTP::priority_t priority;
        OTP::address_t address;
        OTP::timestamp_t sampleTime;
        OTP::PDU::OTPModuleLayer::ident_t ident;
        OTP::PDU::OTPModuleLayer::additional_t additional;
    } addModule_t;
    addModule_ret addModule(addModule_t &moduleData);

    std::shared_ptr<OTP::PDU::OTPLayer::Layer> getOTPLayer() { return otpLayer; }
    std::shared_ptr<OTP::PDU::OTPTransformLayer::Layer> getTransformLayer() { return transformLayer; }
    QMap<address_t, std::shared_ptr<OTP::PDU::OTPPointLayer::Layer>> getPointLayers() { return pointLayers; }
    QMultiMap<address_t, std::shared_ptr<OTP::PDU::OTPModuleLayer::Layer>> getModuleLayers() { return moduleLayers; }

private:
    void updatePduLength();
    QByteArray toByteArray();

    std::shared_ptr<OTP::PDU::OTPLayer::Layer> otpLayer;
    std::shared_ptr<OTP::PDU::OTPTransformLayer::Layer> transformLayer;
    QMap<address_t, std::shared_ptr<OTP::PDU::OTPPointLayer::Layer>> pointLayers;
    QMultiMap<address_t, std::shared_ptr<OTP::PDU::OTPModuleLayer::Layer>> moduleLayers;
};

} // namespace

#endif // OTP_NAME_ADVERTISEMENT_MESSAGE_HPP
