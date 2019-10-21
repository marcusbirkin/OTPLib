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
#include "otp_name_advertisement_message.hpp"

using namespace ACN::OTP::MESSAGES::OTPNameAdvertisementMessage;

Message::Message(
        ACN::OTP::mode_e mode,
        ACN::OTP::cid_t CID,
        ACN::OTP::name_t ProducerName,
        ACN::OTP::PDU::OTPNameAdvertisementLayer::list_t NameList,
        QObject *parent) :
    QObject(parent),
    rootLayer(
        new OTP::PDU::OTPRootLayer::Layer(
            0, CID, this)),
    otpLayer(
        new OTP::PDU::OTPLayer::Layer(
            0, ACN::OTP::PDU::VECTOR_OTP_ADVERTISEMENT_MESSAGE, 0, 0, 0, 0, ProducerName, this)),
    advertisementLayer(
        new OTP::PDU::OTPAdvertisementLayer::Layer(
            0, ACN::OTP::PDU::VECTOR_OTP_ADVERTISEMENT_NAME, this)),
    nameAdvertisementLayer(
        new OTP::PDU::OTPNameAdvertisementLayer::Layer(
            0, ACN::OTP::PDU::OTPNameAdvertisementLayer::options_t(), NameList, this))
{
    ACN::OTP::PDU::OTPNameAdvertisementLayer::options_t options;
    switch (mode) {
        case Producer: options.setResponse(); break;
        case Consumer: options.setRequest(); break;
    }
    nameAdvertisementLayer->setOptions(options);

    updatePduLength();
}

Message::Message(
        QNetworkDatagram message,
        QObject *parent) :
    QObject(parent),
    rootLayer(new OTP::PDU::OTPRootLayer::Layer()),
    otpLayer(new OTP::PDU::OTPLayer::Layer()),
    advertisementLayer(new OTP::PDU::OTPAdvertisementLayer::Layer()),
    nameAdvertisementLayer(new OTP::PDU::OTPNameAdvertisementLayer::Layer())
{
    int idx = 0;
    // Root layer
    {
        PDU::PDUByteArray layer;
        idx += layer.append(message.data().mid(idx, rootLayer->toPDUByteArray().size())).size();
        rootLayer->fromPDUByteArray(layer);
        if (!rootLayer->isValid()) return;
    }

    // OTP Layer
    {
        PDU::PDUByteArray layer;
        idx += layer.append(message.data().mid(idx, otpLayer->toPDUByteArray().size())).size();
        otpLayer->fromPDUByteArray(layer);
        if (!otpLayer->isValid()) return;
    }

    // Advertisment Layer
    {
        PDU::PDUByteArray layer;
        idx += layer.append(message.data().mid(idx, advertisementLayer->toPDUByteArray().size())).size();
        advertisementLayer->fromPDUByteArray(layer);
        if (!advertisementLayer->isValid()) return;
    }

    // Name Advertisment Layer
    if (advertisementLayer->getVector() == PDU::VECTOR_OTP_ADVERTISEMENT_NAME)
    {
        PDU::PDUByteArray layer;
        idx += layer.append(message.data().mid(idx)).size();
        nameAdvertisementLayer->fromPDUByteArray(layer);
        if (!nameAdvertisementLayer->isValid()) return;
    }
}

bool Message::isValid()
{
    if (!rootLayer->isValid()) return false;
    if (!otpLayer->isValid()) return false;
    if (!advertisementLayer->isValid()) return false;
    if (!nameAdvertisementLayer->isValid()) return false;
    if (!RANGES::MESSAGE_SIZE.isValid(toByteArray().size())) return false;
    return true;
}

QNetworkDatagram Message::toQNetworkDatagram(sequence_t sequenceNumber, folio_t folio)
{
    otpLayer->setSequence(sequenceNumber);
    otpLayer->setFolio(folio);
    return QNetworkDatagram(toByteArray(), OTP_Advertisement_Message_IPv4, ACN_SDT_MULTICAST_PORT);
}

QByteArray Message::toByteArray()
{
    QByteArray ba;
    ba.append(rootLayer->toPDUByteArray());
    ba.append(otpLayer->toPDUByteArray());
    ba.append(advertisementLayer->toPDUByteArray());
    ba.append(nameAdvertisementLayer->toPDUByteArray());

    return ba;
}

void Message::updatePduLength()
{
    ACN::OTP::PDU::flags_length_t::pduLength_t length = nameAdvertisementLayer->toPDUByteArray().size();
    nameAdvertisementLayer->setPDULength(length);

    length += advertisementLayer->toPDUByteArray().size();
    advertisementLayer->setPDULength(length);

    length += otpLayer->toPDUByteArray().size();
    otpLayer->setPDULength(length);

    length += rootLayer->toPDUByteArray().size();
    rootLayer->setPDULength(length - ACN::OTP::PDU::OTPRootLayer::PREAMBLE_SIZE);
}
