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
#include "otp_layer.hpp"

using namespace ACN::OTP::PDU::OTPLayer;

Layer::Layer(
        ACN::OTP::PDU::flags_length_t::pduLength_t PDULength,
        ACN::OTP::PDU::vector_t Vector,
        sequence_t Sequence,
        folio_t Folio,
        page_t Page,
        page_t LastPage,
        ACN::OTP::PDU::name_t ProducerName,
        QObject *parent) :
    QObject(parent),
    FlagsLength({FLAGS, PDULength}),
    Vector(Vector),
    ProtocolVer(PROTOCOL_VERSION),
    Sequence(Sequence),
    Folio(Folio),
    Page(Page),
    LastPage(LastPage),
    Options(OPTIONS),
    Reserved(RESERVED),
    ProducerName(ProducerName)
{}

Layer::Layer(
        ACN::OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    FlagsLength({0, 0}),
    Vector(0),
    ProtocolVer(0),
    Sequence(0),
    Folio(0),
    Page(0),
    LastPage(0),
    Options(0),
    Reserved(0),
    ProducerName(name_t())
{
    fromPDUByteArray(layer);
}

bool Layer::isValid()
{
    if (FlagsLength.Flags != FLAGS) return false;
    if (FlagsLength.PDULength == 0) return false;
    if (!VECTOR.contains(Vector)) return false;
    if (ProtocolVer != PROTOCOL_VERSION) return false;
    if (Page > LastPage) return false;
    if (ProducerName.length() != NAME_LENGTH) return false;
    return true;
}

ACN::OTP::PDU::PDUByteArray Layer::toPDUByteArray()
{
    PDUByteArray ret;
    return ret
        << FlagsLength
        << Vector
        << ProtocolVer
        << Sequence
        << Folio
        << Page
        << LastPage
        << Options
        << Reserved
        << ProducerName;
}


void Layer::fromPDUByteArray(ACN::OTP::PDU::PDUByteArray layer)
{
    FlagsLength = {0,0};
    Vector = 0;
    ProtocolVer = 0;
    Sequence = 0;
    Folio = 0;
    Page = 0;
    LastPage = 0;
    Options = 0;
    Reserved = 0;
    ProducerName = name_t();

    if (layer.size() != Layer().toPDUByteArray().size())
        return;

    layer >> FlagsLength
        >> Vector
        >> ProtocolVer
        >> Sequence
        >> Folio
        >> Page
        >> LastPage
        >> Options
        >> Reserved
        >> ProducerName;
}
