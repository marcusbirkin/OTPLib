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

using namespace OTP::PDU;
using namespace OTP::PDU::OTPLayer;

Layer::Layer(vector_t Vector,
        pduLength_t PDULength,
        cid_t CID,
        folio_t Folio,
        page_t Page,
        page_t LastPage,
        name_t ComponentName,
        QObject *parent) :
    QObject(parent),
    PacketIdent(OTP_PACKET_IDENT),
    Vector(Vector),
    PDULength(PDULength),
    CID(CID),
    Folio(Folio),
    Page(Page),
    LastPage(LastPage),
    Options(OPTIONS),
    Reserved(RESERVED),
    ComponentName(ComponentName)
{}

Layer::Layer(
        OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    PacketIdent(QByteArray()),
    Vector(0),
    PDULength(0),
    CID(cid_t()),
    Folio(0),
    Page(0),
    LastPage(0),
    Options(0),
    Reserved(0),
    ComponentName(name_t())
{
    fromPDUByteArray(layer);
}

bool Layer::isValid()
{
    if (PacketIdent != OTP_PACKET_IDENT) return false;
    if (!VECTOR.contains(Vector)) return false;
    if (PDULength <= toPDUByteArray().size() - LENGTHOFFSET) return false;
    if (CID.isNull()) return false;
    if (Page > LastPage) return false;
    if (ComponentName.length() != NAME_LENGTH) return false;
    return true;
}

OTP::PDU::PDUByteArray Layer::toPDUByteArray()
{
    PDUByteArray ret;
    return ret
        << PacketIdent
        << Vector
        << PDULength
        << Footer.Flags()
        << Footer.getLength()
        << CID
        << Folio
        << Page
        << LastPage
        << Options
        << Reserved
        << ComponentName;
}

void Layer::fromPDUByteArray(OTP::PDU::PDUByteArray layer)
{
    PacketIdent = QByteArray();
    Vector = 0;
    PDULength = 0;
    Footer = footer_t();
    CID = cid_t();
    Folio = 0;
    Page = 0;
    LastPage = 0;
    Options = 0;
    Reserved = 0;
    ComponentName = name_t();

    if (layer.size() != Layer().toPDUByteArray().size())
        return;

    decltype (Footer.getLength()) FooterLength;
    layer >> PacketIdent
        >> Vector
        >> PDULength
        >> Footer.Flags()
        >> FooterLength // Currently Ignored
        >> CID
        >> Folio
        >> Page
        >> LastPage
        >> Options
        >> Reserved
        >> ComponentName;
}
