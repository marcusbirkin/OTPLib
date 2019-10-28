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
#include "otp_system_advertisement_message.hpp"

using namespace ACN::OTP::MESSAGES::OTPSystemAdvertisementMessage;

Message::Message(
        ACN::OTP::mode_e mode,
        ACN::OTP::cid_t CID,
        ACN::OTP::name_t ProducerName,
        ACN::OTP::PDU::OTPSystemAdvertisementLayer::list_t SystemList,
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
            0, ACN::OTP::PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM, this)),
    systemAdvertisementLayer(
        new OTP::PDU::OTPSystemAdvertisementLayer::Layer(
            0, ACN::OTP::PDU::OTPSystemAdvertisementLayer::options_t(), SystemList, this))
{

    ACN::OTP::PDU::OTPSystemAdvertisementLayer::options_t options;
    switch (mode) {
        case Producer: options.setResponse(); break;
        case Consumer: options.setRequest(); break;
    }
    systemAdvertisementLayer->setOptions(options);

    updatePduLength();
}

Message::Message(
        QNetworkDatagram message,
        QObject *parent) :
    QObject(parent),
    rootLayer(new OTP::PDU::OTPRootLayer::Layer()),
    otpLayer(new OTP::PDU::OTPLayer::Layer()),
    advertisementLayer(new OTP::PDU::OTPAdvertisementLayer::Layer()),
    systemAdvertisementLayer(new OTP::PDU::OTPSystemAdvertisementLayer::Layer())
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

    // System Advertisment Layer
    if (advertisementLayer->getVector() == PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM)
    {
        PDU::PDUByteArray layer;
        idx += layer.append(message.data().mid(idx)).size();
        systemAdvertisementLayer->fromPDUByteArray(layer);
        if (!systemAdvertisementLayer->isValid()) return;
    }
}

bool Message::isValid()
{
    if (!rootLayer->isValid()) return false;
    if (!otpLayer->isValid()) return false;
    if (!advertisementLayer->isValid()) return false;
    if (!systemAdvertisementLayer->isValid()) return false;
    if (!RANGES::MESSAGE_SIZE.isValid(toByteArray().size())) return false;
    return true;
}

QNetworkDatagram Message::toQNetworkDatagram(
        QHostAddress destAddr,
        sequence_t sequenceNumber,
        folio_t folio,
        page_t thisPage,
        page_t lastPage)
{
    otpLayer->setSequence(sequenceNumber);
    otpLayer->setFolio(folio);
    otpLayer->setPage(thisPage);
    otpLayer->setLastPage(lastPage);
    updatePduLength();
    return QNetworkDatagram(toByteArray(), destAddr, ACN_SDT_MULTICAST_PORT);
}

QByteArray Message::toByteArray()
{
    updatePduLength();

    QByteArray ba;
    ba.append(rootLayer->toPDUByteArray());
    ba.append(otpLayer->toPDUByteArray());
    ba.append(advertisementLayer->toPDUByteArray());
    ba.append(systemAdvertisementLayer->toPDUByteArray());

    return ba;
}

void Message::updatePduLength()
{
    ACN::OTP::PDU::flags_length_t::pduLength_t length = systemAdvertisementLayer->toPDUByteArray().size();
    systemAdvertisementLayer->setPDULength(length);

    length += advertisementLayer->toPDUByteArray().size();
    advertisementLayer->setPDULength(length);

    length += otpLayer->toPDUByteArray().size();
    otpLayer->setPDULength(length);

    length += rootLayer->toPDUByteArray().size();
    rootLayer->setPDULength(length - ACN::OTP::PDU::OTPRootLayer::PREAMBLE_SIZE);
}
