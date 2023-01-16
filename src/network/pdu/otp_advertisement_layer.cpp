/**
 * @file        otp_advertisement_layer.cpp
 * @brief       Advertisement PDU Layer
 * @details     Part of OTPLib - A QT interface for E1.59
 * @authors     Marcus Birkin
 * @copyright   Copyright (C) 2019 Marcus Birkin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "otp_advertisement_layer.hpp"

using namespace OTP::PDU;
using namespace OTP::PDU::OTPAdvertisementLayer;

Layer::Layer(
        vector_t Vector,
        pduLength_t PDULength,
        QObject *parent) :
    QObject(parent),
    Vector(Vector),
    PDULength(PDULength),
    Reserved(RESERVED)
{}

Layer::Layer(
        OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    Vector(0),
    PDULength(0),
    Reserved(0)
{
    fromPDUByteArray(layer);
}

bool Layer::isValid() const
{
    if (!VECTOR.contains(Vector)) return false;
    if (PDULength <= toPDUByteArray().size() - LENGTHOFFSET) return false;
    return true;
}

OTP::PDU::PDUByteArray Layer::toPDUByteArray() const
{
    PDUByteArray ret;
    return ret << Vector
        << PDULength
        << Reserved;
}

void Layer::fromPDUByteArray(OTP::PDU::PDUByteArray layer)
{
    Vector = 0;
    PDULength = 0;
    Reserved = 0;

    if (layer.size() != Layer().toPDUByteArray().size())
        return;

    layer >> Vector
        >> PDULength
        >> Reserved;
}
