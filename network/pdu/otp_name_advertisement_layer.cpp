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
#include "otp_name_advertisement_layer.hpp"

using namespace OTP::PDU;
using namespace OTP::PDU::OTPNameAdvertisementLayer;

Layer::Layer(
        pduLength_t PDULength,
        options_t Options,
        list_t List,
        QObject *parent) :
    QObject(parent),
    Vector(VECTOR),
    PDULength(PDULength),
    Options(Options),
    Reserved(RESERVED),
    List(List)
{}

Layer::Layer(
        OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    Vector(0),
    PDULength(0),
    Options(options_t()),
    Reserved(0),
    List(list_t())
{
    fromPDUByteArray(layer);
}

bool Layer::isValid()
{
    if (Vector != VECTOR) return false;
    if (PDULength != toPDUByteArray().size() - LENGTHOFFSET) return false;
    if (Options.isResponse())
    {
        if (!RANGES::ListSize.isValid(List.size() * item_t().getSize())) return false;
    }
    return true;
}

OTP::PDU::PDUByteArray Layer::toPDUByteArray()
{
    PDUByteArray ret;
    ret << Vector
        << PDULength
        << Options
        << Reserved;
    if (Options.isResponse()) ret << List;

    return ret;
}

void Layer::fromPDUByteArray(OTP::PDU::PDUByteArray layer)
{
    Vector = 0;
    PDULength = 0;
    Options = options_t();
    Reserved = 0;
    List.clear();

    if (layer.size() < Layer().toPDUByteArray().size())
        return;

    layer >> Vector
        >> PDULength
        >> Options
        >> Reserved
        >> List;
}

bool Layer::setList(list_t value)
{
    List = value;
    return RANGES::ListSize.isValid(List.size() * item_t().getSize());
}

bool Layer::addItem(item_t value)
{
    if (List.contains(value)) return true;
    List.append(value);
    if (RANGES::ListSize.isValid(List.size() * item_t().getSize())) return true;
    List.removeLast();
    return false;
}
