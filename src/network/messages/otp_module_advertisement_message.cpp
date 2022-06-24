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

using namespace OTP::PDU;
using namespace OTP::MESSAGES::OTPModuleAdvertisementMessage;

Message::Message(
        OTP::mode_e mode,
        OTP::cid_t CID,
        OTP::name_t ComponentName,
        OTP::PDU::OTPModuleAdvertisementLayer::list_t ModuleList,
        QObject *parent) :
    QObject(parent),  
    otpLayer(
        new OTPLayer::Layer(
            VECTOR_OTP_ADVERTISEMENT_MESSAGE, 0, CID, 0, 0, 0, ComponentName, this)),
    advertisementLayer(
        new OTP::PDU::OTPAdvertisementLayer::Layer(
            OTP::PDU::VECTOR_OTP_ADVERTISEMENT_MODULE, 0, this)),
    moduleAdvertisementLayer(
        new OTP::PDU::OTPModuleAdvertisementLayer::Layer(
            0, ModuleList, this))
{
    Q_UNUSED(mode)
    updatePduLength();
}

Message::Message(
        QNetworkDatagram message,
        QObject *parent) :
    QObject(parent),
    otpLayer(new OTP::PDU::OTPLayer::Layer()),
    advertisementLayer(new OTP::PDU::OTPAdvertisementLayer::Layer()),
    moduleAdvertisementLayer(new OTP::PDU::OTPModuleAdvertisementLayer::Layer())
{
    int idx = 0;

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
    size_t lengthCheck = toByteArray().length();
    if (lengthCheck != static_cast<size_t>(otpLayer->getPDULength() + OTPLayer::LENGTHOFFSET))
        return false;
    if (!otpLayer->isValid()) return false;

    lengthCheck -= otpLayer->toPDUByteArray().length();
    if (lengthCheck != static_cast<size_t>(advertisementLayer->getPDULength() + OTPAdvertisementLayer::LENGTHOFFSET))
        return false;
    if (advertisementLayer->getVector() != PDU::VECTOR_OTP_ADVERTISEMENT_MODULE) return false;
    if (!advertisementLayer->isValid()) return false;

    lengthCheck -= advertisementLayer->toPDUByteArray().length();
    if (lengthCheck != static_cast<size_t>(moduleAdvertisementLayer->getPDULength() + OTPModuleAdvertisementLayer::LENGTHOFFSET))
        return false;
    if (!moduleAdvertisementLayer->isValid()) return false;
    if (!RANGES::MESSAGE_SIZE.isValid(toByteArray().size())) return false;
    return true;
}

QNetworkDatagram Message::toQNetworkDatagram(
        QHostAddress destAddr,
        folio_t folio,
        page_t thisPage,
        page_t lastPage)
{
    otpLayer->setFolio(folio);
    otpLayer->setPage(thisPage);
    otpLayer->setLastPage(lastPage);
    updatePduLength();
    return QNetworkDatagram(toByteArray(), destAddr, OTP_PORT);
}

QByteArray Message::toByteArray()
{   
    QByteArray ba;
    ba.append(otpLayer->toPDUByteArray());
    ba.append(advertisementLayer->toPDUByteArray());
    ba.append(moduleAdvertisementLayer->toPDUByteArray());

    return ba;
}

void Message::updatePduLength()
{
    /* 14.2 Length */
    pduLength_t length = moduleAdvertisementLayer->toPDUByteArray().size();
    moduleAdvertisementLayer->setPDULength(length - OTPModuleLayer::LENGTHOFFSET);

    /* 11.2 Length */
    length += advertisementLayer->toPDUByteArray().size();
    advertisementLayer->setPDULength(length - OTPAdvertisementLayer::LENGTHOFFSET);

    /* 6.3 Length */
    length += otpLayer->toPDUByteArray().size();
    otpLayer->setPDULength(length - OTPLayer::LENGTHOFFSET);
}
