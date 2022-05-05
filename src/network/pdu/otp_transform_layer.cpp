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
#include "otp_transform_layer.hpp"

using namespace OTP::PDU;
using namespace OTP::PDU::OTPTransformLayer;

Layer::Layer(
        pduLength_t PDULength,
        system_t System,
        timestamp_t Timestamp,
        options_t Options,
        QObject *parent) :
    QObject(parent),
    Vector(VECTOR),
    PDULength(PDULength),
    System(System),
    Timestamp(Timestamp),
    Options(Options),
    Reserved(RESERVED)
{}

Layer::Layer(
        OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    Vector(0),
    PDULength(0),
    System(0),
    Options(options_t()),
    Reserved(0)
{
    if (layer.size() != Layer().toPDUByteArray().size())
        return;

    layer >> Vector
        >> PDULength
        >> System
        >> Timestamp
        >> Options
        >> Reserved;
}

bool Layer::isValid() const
{
    if (Vector != VECTOR) return false;
    if (PDULength <= toPDUByteArray().size() - LENGTHOFFSET) return false;
    if (!System.isValid()) return false;
    if (Timestamp == 0) return false;
    return true;
}

OTP::PDU::PDUByteArray Layer::toPDUByteArray() const
{
    PDUByteArray ret;
    return ret << Vector
        << PDULength
        << System
        << Timestamp
        << Options
        << Reserved;
}

void Layer::fromPDUByteArray(OTP::PDU::PDUByteArray layer)
{
    Vector = 0;
    PDULength = 0;
    System = 0;
    Timestamp = 0;
    Options = options_t();
    Reserved = 0;

    if (layer.size() != Layer().toPDUByteArray().size())
        return;

    layer >> Vector
        >> PDULength
        >> System
        >> Timestamp
        >> Options
        >> Reserved;
}
