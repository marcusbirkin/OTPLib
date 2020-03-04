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
#include "otp_transform_message.hpp"

using namespace OTP::MESSAGES::OTPTransformMessage;

Message::Message(
        OTP::cid_t CID,
        OTP::name_t ProducerName,
        OTP::system_t System,
        QObject *parent) :
    QObject(parent),
    rootLayer(
        new OTP::PDU::OTPRootLayer::Layer(
            0, CID, this)),
    otpLayer(
        new OTP::PDU::OTPLayer::Layer(
            0, OTP::PDU::VECTOR_OTP_TRANSFORM_MESSAGE, 0, 0, 0, 0, ProducerName, this)),
    transformLayer(
        new OTP::PDU::OTPTransformLayer::Layer(
            0, System, static_cast<timestamp_t>(QDateTime::currentMSecsSinceEpoch() * 1000), this))
{
    updatePduLength();
}

Message::Message(
        QNetworkDatagram message,
        QObject *parent) :
    QObject(parent),
    rootLayer(new OTP::PDU::OTPRootLayer::Layer()),
    otpLayer(new OTP::PDU::OTPLayer::Layer()),
    transformLayer(new OTP::PDU::OTPTransformLayer::Layer())
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

    // Transform Layer
    {
        PDU::PDUByteArray layer;
        idx += layer.append(message.data().mid(idx, transformLayer->toPDUByteArray().size())).size();
        transformLayer->fromPDUByteArray(layer);
        if (!transformLayer->isValid()) return;
    }

    // Point PDU
    while (idx < message.data().size()) {
        // Point Layer
        auto pointLayer = std::make_shared<OTP::PDU::OTPPointLayer::Layer>();
        {
            PDU::PDUByteArray layer;
            idx += layer.append(message.data().mid(idx, pointLayer->toPDUByteArray().size())).size();
            pointLayer->fromPDUByteArray(layer);
            if (!pointLayer->isValid()) return;

            address_t address = {transformLayer->getSystem(), pointLayer->getGroup(), pointLayer->getPoint()};
            pointLayers.insert(address, pointLayer);
        }

        // Module Layer
        auto pduRemaining = pointLayer->getPDULength() - pointLayer->toPDUByteArray().size();
        while (pduRemaining) {
            address_t address = {transformLayer->getSystem(), pointLayer->getGroup(), pointLayer->getPoint()};
            auto moduleLayer = std::make_shared<OTP::PDU::OTPModuleLayer::Layer>();
            {
                PDU::PDUByteArray layer;
                // Obtain, reported, layer size
                layer.append(message.data().mid(idx, pduRemaining));
                auto layerSize =
                        static_cast<int>(OTP::PDU::OTPModuleLayer::Layer::getPDULength(layer));

                // Get layer
                layer.clear();
                idx += layer.append(message.data().mid(idx, layerSize)).size();
                moduleLayer->fromPDUByteArray(layer);
                if (!moduleLayer->isValid()) return;

                pduRemaining -= moduleLayer->toPDUByteArray().size();
            }
            moduleLayers.insert(address, moduleLayer);
        }
    }
}

bool Message::isValid()
{
    if (!rootLayer->isValid()) return false;
    if (!otpLayer->isValid()) return false;
    if (!transformLayer->isValid()) return false;
    for (auto pointLayer : pointLayers)
        if (!pointLayer->isValid()) return false;
    for (auto moduleLayer : moduleLayers)
        if (!moduleLayer->isValid()) return false;
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
    return QNetworkDatagram(toByteArray(), destAddr, OTP_PORT);
}

Message::addModule_ret Message::addModule(addModule_t &moduleData)
{
    if (moduleData.address.system != transformLayer->getSystem()) return InvalidSystem;
    if (moduleData.additional.isEmpty()) return InvalidAdditional;
    if (moduleData.sampleTime == 0) return InvalidTimestamp;

    if (!pointLayers.contains(moduleData.address))
    {
        auto pointLayer = std::make_shared<OTP::PDU::OTPPointLayer::Layer>
                (0, moduleData.address.group, moduleData.address.point, moduleData.sampleTime, this);

        if ((pointLayer->toPDUByteArray().size() + this->toByteArray().size()) > RANGES::MESSAGE_SIZE.getMax())
            return MessageToBig;
        pointLayers.insert(moduleData.address, pointLayer);
    } else {
        /* TODO Seperate modules with differing sample times into different point layers */
        if (moduleData.sampleTime > pointLayers.value(moduleData.address)->getTimestamp())
            pointLayers.value(moduleData.address)->setTimestamp(moduleData.sampleTime);
    }

    auto moduleLayer = std::make_shared<OTP::PDU::OTPModuleLayer::Layer>(0, moduleData.ident.ManufacturerID, moduleData.ident.ModuleNumber);
    moduleLayer->setAdditional(moduleData.additional);

    if ((moduleLayer->toPDUByteArray().size() + this->toByteArray().size()) > RANGES::MESSAGE_SIZE.getMax())
        return MessageToBig;
    moduleLayers.insert(moduleData.address, moduleLayer);

    updatePduLength();

    return OK;
}

QByteArray Message::toByteArray()
{
    QByteArray ba;
    ba.append(rootLayer->toPDUByteArray());
    ba.append(otpLayer->toPDUByteArray());
    ba.append(transformLayer->toPDUByteArray());
    for (auto pointLayer : pointLayers)
    {
        ba.append(pointLayer->toPDUByteArray());
        address_t address = {transformLayer->getSystem(), pointLayer->getGroup(), pointLayer->getPoint()};
        auto iterator = moduleLayers.find(address);
        while (iterator != moduleLayers.end() && iterator.key() == address)
        {
            ba.append(iterator.value()->toPDUByteArray());
            ++iterator;
        }
    }

    return ba;
}

void Message::updatePduLength()
{
    OTP::PDU::flags_length_t::pduLength_t length = 0;

    for (auto moduleLayer : moduleLayers)
    {
        /* 10.2 Length */
        moduleLayer->setPDULength(
                    moduleLayer->toPDUByteArray().size()
                    - sizeof(moduleLayer->getManufacturerID())
                    - sizeof(moduleLayer->getPDULength()));
    }

    for (auto pointLayer : pointLayers)
    {
        OTP::PDU::flags_length_t::pduLength_t pointPDULength = 0;
        address_t address = {transformLayer->getSystem(), pointLayer->getGroup(), pointLayer->getPoint()};
        auto iterator = moduleLayers.find(address);
        while (iterator != moduleLayers.end() && iterator.key() == address)
        {
            pointPDULength += iterator.value()->getPDULength();
            ++iterator;
        }

        /* 9.2 Length */
        pointLayer->setPDULength(
                    pointLayer->toPDUByteArray().size()
                    - sizeof(pointLayer->getVector())
                    - sizeof(pointLayer->getPDULength())
                    + pointPDULength);
        length += pointLayer->getPDULength();
    }

    /* 8.2 Length */
    length +=
            transformLayer->toPDUByteArray().size()
            - sizeof(transformLayer->getVector())
            - sizeof(transformLayer->getPDULength());
    transformLayer->setPDULength(length);

    /* 6.3 Length */
    length += otpLayer->toPDUByteArray().size() - 16;
    otpLayer->setPDULength(length);

    length += rootLayer->toPDUByteArray().size();
    rootLayer->setPDULength(length - OTP::PDU::OTPRootLayer::PREAMBLE_SIZE);
}
