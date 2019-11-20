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
#include "otp_module_advertisement_message.hpp"

using namespace OTP::MESSAGES::OTPModuleAdvertisementMessage;

Message::Message(
        OTP::mode_e mode,
        OTP::cid_t CID,
        OTP::name_t ProducerName,
        OTP::PDU::OTPModuleAdvertisementLayer::list_t ModuleList,
        QObject *parent) :
    QObject(parent),
    rootLayer(
        new OTP::PDU::OTPRootLayer::Layer(
            0, CID, this)),
    otpLayer(
        new OTP::PDU::OTPLayer::Layer(
            0, OTP::PDU::VECTOR_OTP_ADVERTISEMENT_MESSAGE, 0, 0, 0, 0, ProducerName, this)),
    advertisementLayer(
        new OTP::PDU::OTPAdvertisementLayer::Layer(
            0, OTP::PDU::VECTOR_OTP_ADVERTISEMENT_MODULE, this)),
    moduleAdvertisementLayer(
        new OTP::PDU::OTPModuleAdvertisementLayer::Layer(
            0, ModuleList, this))
{
    Q_UNUSED(mode);

    updatePduLength();
}

Message::Message(
        QNetworkDatagram message,
        QObject *parent) :
    QObject(parent),
    rootLayer(new OTP::PDU::OTPRootLayer::Layer()),
    otpLayer(new OTP::PDU::OTPLayer::Layer()),
    advertisementLayer(new OTP::PDU::OTPAdvertisementLayer::Layer()),
    moduleAdvertisementLayer(new OTP::PDU::OTPModuleAdvertisementLayer::Layer())
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

    // Module Advertisment Layer
    if (advertisementLayer->getVector() == PDU::VECTOR_OTP_ADVERTISEMENT_MODULE)
    {
        PDU::PDUByteArray layer;
        idx += layer.append(message.data().mid(idx)).size();
        moduleAdvertisementLayer->fromPDUByteArray(layer);
        if (!moduleAdvertisementLayer->isValid()) return;
    }
}

bool Message::isValid()
{
    if (!rootLayer->isValid()) return false;
    if (!otpLayer->isValid()) return false;
    if (!advertisementLayer->isValid()) return false;
    if (!moduleAdvertisementLayer->isValid()) return false;
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
    QByteArray ba;
    ba.append(rootLayer->toPDUByteArray());
    ba.append(otpLayer->toPDUByteArray());
    ba.append(advertisementLayer->toPDUByteArray());
    ba.append(moduleAdvertisementLayer->toPDUByteArray());

    return ba;
}

void Message::updatePduLength()
{
    OTP::PDU::flags_length_t::pduLength_t length = moduleAdvertisementLayer->toPDUByteArray().size();
    moduleAdvertisementLayer->setPDULength(length);

    length += advertisementLayer->toPDUByteArray().size();
    advertisementLayer->setPDULength(length);

    length += otpLayer->toPDUByteArray().size();
    otpLayer->setPDULength(length);

    length += rootLayer->toPDUByteArray().size();
    rootLayer->setPDULength(length - OTP::PDU::OTPRootLayer::PREAMBLE_SIZE);
}
