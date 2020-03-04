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
#include "otp_module_advertisement_layer.hpp"

using namespace OTP::PDU::OTPModuleAdvertisementLayer;

Layer::Layer(OTP::PDU::flags_length_t::pduLength_t PDULength,
        list_t List,
        QObject *parent) :
    QObject(parent),
    FlagsLength({FLAGS, PDULength}),
    Vector(VECTOR),
    Reserved(RESERVED),
    List(List)
{}

Layer::Layer(
        OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    FlagsLength({0, 0}),
    Vector(0),
    Reserved(0),
    List(list_t())
{
    fromPDUByteArray(layer);
}

bool Layer::isValid()
{
    if (FlagsLength.Flags != FLAGS) return false;
    if (FlagsLength.PDULength == 0) return false;
    if (Vector != VECTOR) return false;
    if (PDULength !=
            toPDUByteArray().size()
            - sizeof(Vector)
            - sizeof(PDULength)
            )
        return false;
    if (!RANGES::ListSize.isValid(List.size() * item_t().getSize())) return false;
    return true;
}

OTP::PDU::PDUByteArray Layer::toPDUByteArray()
{
    PDUByteArray ret;
    return ret << FlagsLength
        << Vector
        << Reserved
        << List;
}

void Layer::fromPDUByteArray(OTP::PDU::PDUByteArray layer)
{
    FlagsLength = {0,0};
    Vector = 0;
    Reserved = 0;
    List.clear();

    if (layer.size() < Layer().toPDUByteArray().size())
        return;

    layer >> FlagsLength
        >> Vector
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
