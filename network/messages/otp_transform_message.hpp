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

namespace ACN::OTP::MESSAGES::OTPTransformMessage
{

class Message : public QObject
{
    Q_OBJECT
public:
    explicit Message(
            cid_t CID,
            name_t ProducerName,
            system_t System,
            QObject *parent = nullptr);
    explicit Message(
            QNetworkDatagram message,
            QObject *parent = nullptr);
    bool isValid();
    QNetworkDatagram toQNetworkDatagram(sequence_t sequenceNumber, folio_t folio);

    void addModule(
            address_t address,
            timestamp_t sampleTime,
            OTP::PDU::OTPModuleLayer::vector_t vector,
            OTP::PDU::OTPModuleLayer::additional_t additional);

    std::shared_ptr<OTP::PDU::OTPRootLayer::Layer> getRootLayer() { return rootLayer; }
    std::shared_ptr<OTP::PDU::OTPLayer::Layer> getOTPLayer() { return otpLayer; }
    std::shared_ptr<OTP::PDU::OTPTransformLayer::Layer> getTransformLayer() { return transformLayer; }
    QMap<address_t, std::shared_ptr<ACN::OTP::PDU::OTPPointLayer::Layer>> getPointLayers() { return pointLayers; }
    QMultiMap<address_t, std::shared_ptr<ACN::OTP::PDU::OTPModuleLayer::Layer>> getModuleLayers() { return moduleLayers; }

private:
    void updatePduLength();
    QByteArray toByteArray();

    std::shared_ptr<OTP::PDU::OTPRootLayer::Layer> rootLayer;
    std::shared_ptr<OTP::PDU::OTPLayer::Layer> otpLayer;
    std::shared_ptr<OTP::PDU::OTPTransformLayer::Layer> transformLayer;
    QMap<address_t, std::shared_ptr<ACN::OTP::PDU::OTPPointLayer::Layer>> pointLayers;
    QMultiMap<address_t, std::shared_ptr<ACN::OTP::PDU::OTPModuleLayer::Layer>> moduleLayers;
};

} // namespace

#endif // OTP_NAME_ADVERTISEMENT_MESSAGE_HPP
