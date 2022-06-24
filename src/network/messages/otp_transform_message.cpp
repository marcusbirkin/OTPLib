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

using namespace OTP::PDU;
using namespace OTP::MESSAGES::OTPTransformMessage;

Message::Message(OTP::cid_t CID,
        OTP::name_t ComponentName,
        OTP::system_t System,
        bool FullPointSet,
        QObject *parent) :
    QObject(parent),
    otpLayer(
        new OTPLayer::Layer(
            VECTOR_OTP_TRANSFORM_MESSAGE, 0, CID, 0, 0, 0, ComponentName, this)),
    transformLayer(
        new OTPTransformLayer::Layer(
            0, System, static_cast<timestamp_t>(QDateTime::currentMSecsSinceEpoch() * 1000), OTPTransformLayer::options_t(FullPointSet), this))
{
    updatePduLength();
}

Message::Message(
        QNetworkDatagram message,
        QObject *parent) :
    QObject(parent),
    otpLayer(new OTP::PDU::OTPLayer::Layer()),
    transformLayer(new OTP::PDU::OTPTransformLayer::Layer())
{
    int idx = 0;
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
        int pduRemaining = (pointLayer->getPDULength() + OTPPointLayer::LENGTHOFFSET) - pointLayer->toPDUByteArray().size();
        while (pduRemaining) {
            address_t address = {transformLayer->getSystem(), pointLayer->getGroup(), pointLayer->getPoint()};
            auto moduleLayer = std::make_shared<OTP::PDU::OTPModuleLayer::Layer>();
            {
                PDU::PDUByteArray layer;
                // Obtain, reported, layer size
                layer.append(message.data().mid(idx, pduRemaining));
                auto layerSize =
                        static_cast<int>(OTP::PDU::OTPModuleLayer::Layer::getPDULength(layer) + OTPModuleLayer::LENGTHOFFSET);

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

bool Message::isValid() const
{
    size_t lengthCheck = toByteArray().length();
    if (lengthCheck != otpLayer->getPDULength() + OTPLayer::LENGTHOFFSET)
        return false;
    if (!otpLayer->isValid()) return false;

    lengthCheck -= otpLayer->toPDUByteArray().length();
    if (lengthCheck != transformLayer->getPDULength() + OTPTransformLayer::LENGTHOFFSET)
        return false;
    if (!transformLayer->isValid()) return false;

    for (const auto &pointLayer : pointLayers)
        if (!pointLayer->isValid()) return false;

    for (const auto &moduleLayer : moduleLayers)
        if (!moduleLayer->isValid()) return false;
    if (!RANGES::MESSAGE_SIZE.isValid(toByteArray().size() - otpLayer->getFooter().getLength()))
        return false;
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

Message::addModule_ret Message::addModule(addModule_t &moduleData)
{
    if (moduleData.address.system != transformLayer->getSystem()) return InvalidSystem;
    if (moduleData.additional.isEmpty()) return InvalidAdditional;
    if (moduleData.sampleTime == 0) return InvalidTimestamp;

    if (!pointLayers.contains(moduleData.address))
    {
        auto pointLayer = std::make_shared<OTP::PDU::OTPPointLayer::Layer>
                (0, moduleData.priority, moduleData.address.group, moduleData.address.point, moduleData.sampleTime, this);
        if ((pointLayer->toPDUByteArray().size() + this->toByteArray().size()) > RANGES::MESSAGE_SIZE.getMax())
            return MessageToBig;
        pointLayers.insert(moduleData.address, pointLayer);
    } else {
        /* TODO Seperate modules with differing sample times into different point layers */
        if (moduleData.sampleTime > pointLayers.value(moduleData.address)->getTimestamp())
            pointLayers.value(moduleData.address)->setTimestamp(moduleData.sampleTime);
    }

    auto moduleLayer = std::make_shared<OTP::PDU::OTPModuleLayer::Layer>(moduleData.ident.ManufacturerID, 0, moduleData.ident.ModuleNumber);
    moduleLayer->setAdditional(moduleData.additional);

    if ((moduleLayer->toPDUByteArray().size() + this->toByteArray().size()) > RANGES::MESSAGE_SIZE.getMax())
        return MessageToBig;
    moduleLayers.insert(moduleData.address, moduleLayer);

    updatePduLength();

    return OK;
}

QByteArray Message::toByteArray() const
{
    QByteArray ba;
    ba.append(otpLayer->toPDUByteArray());
    ba.append(transformLayer->toPDUByteArray());
    for (const auto &pointLayer : pointLayers)
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
    pduLength_t length = 0;

    for (const auto &pointLayer : qAsConst(pointLayers))
    {
        pduLength_t modulesLength = 0;
        address_t address = {transformLayer->getSystem(), pointLayer->getGroup(), pointLayer->getPoint()};
        auto iterator = moduleLayers.find(address);
        while (iterator != moduleLayers.end() && iterator.key() == address)
        {
            auto moduleLayer = iterator.value();

            /* 10.2 Length */
            modulesLength += moduleLayer->toPDUByteArray().size();
            moduleLayer->setPDULength(
                        moduleLayer->toPDUByteArray().size() - OTPModuleLayer::LENGTHOFFSET);

            ++iterator;
        }

        /* 9.2 Length */
        length += pointLayer->toPDUByteArray().size() + modulesLength;
        pointLayer->setPDULength(
                    (pointLayer->toPDUByteArray().size() - OTPPointLayer::LENGTHOFFSET)
                    + modulesLength);
    }

    /* 8.2 Length */
    length += transformLayer->toPDUByteArray().size();
    transformLayer->setPDULength(length - OTPTransformLayer::LENGTHOFFSET);

    /* 6.3 Length */
    length += otpLayer->toPDUByteArray().size();
    otpLayer->setPDULength(length - OTPLayer::LENGTHOFFSET);
}
