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
#include "otp_root_layer.hpp"

using namespace OTP::PDU::OTPRootLayer;

Layer::Layer(
        OTP::PDU::flags_length_t::pduLength_t PDULength,
        cid_t CID,
        QObject *parent) :
    QObject(parent),
    PreableSize(PREAMBLE_SIZE),
    PostableSize(POSTAMBLE_SIZE),
    PacketIdent(ACN_PACKET_IDENT),
    FlagsLength({FLAGS, PDULength}),
    Vector(VECTOR),
    CID(CID)
{}

Layer::Layer(
        OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    PreableSize(0),
    PostableSize(0),
    PacketIdent(QByteArray()),
    FlagsLength({0, 0}),
    Vector(0)
{
    fromPDUByteArray(layer);
}

bool Layer::isValid()
{
    if (PreableSize != PREAMBLE_SIZE) return false;
    if (PostableSize != POSTAMBLE_SIZE) return false;
    if (PacketIdent != ACN_PACKET_IDENT) return false;
    if (FlagsLength.Flags != FLAGS) return false;
    if (FlagsLength.PDULength == 0) return false;
    if (Vector != VECTOR) return false;
    if (CID.isNull()) return false;
    return true;
}

OTP::PDU::PDUByteArray Layer::toPDUByteArray()
{
    PDUByteArray ret;
    return ret
        << PreableSize
        << PostableSize
        << PacketIdent
        << FlagsLength
        << Vector
        << CID;
}

void Layer::fromPDUByteArray(OTP::PDU::PDUByteArray layer)
{

    PreableSize = 0;
    PostableSize = 0;
    PacketIdent.clear();
    FlagsLength = {0,0};
    Vector = 0;
    CID = cid_t::fromString(QStringView(QString("{00000000-0000-0000-0000-000000000000}")));
    if (layer.size() != Layer().toPDUByteArray().size())
        return;

    layer >> PreableSize
        >> PostableSize
        >> PacketIdent
        >> FlagsLength
        >> Vector
        >> CID;
}
