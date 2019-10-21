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
#include "otp_system_advertisement_layer.hpp"

using namespace ACN::OTP::PDU::OTPSystemAdvertisementLayer;

Layer::Layer(
        ACN::OTP::PDU::flags_length_t::pduLength_t PDULength,
        options_t Options,
        list_t List,
        QObject *parent) :
    QObject(parent),
    FlagsLength({FLAGS, PDULength}),
    Vector(VECTOR),
    Options(Options),
    Reserved(RESERVED),
    List(List)
{}

Layer::Layer(
        ACN::OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    FlagsLength({0, 0}),
    Vector(0),
    Options(options_t()),
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
    if (Options.isResponse())
    {
        if (!RANGES::ListSize.isValid(List.size() * sizeof(system_t))) return false;
    }

    return true;
}

ACN::OTP::PDU::PDUByteArray Layer::toPDUByteArray()
{
    PDUByteArray ret;
    ret << FlagsLength
        << Vector
        << Options
        << Reserved
        << List;

    return ret;
}

void Layer::fromPDUByteArray(ACN::OTP::PDU::PDUByteArray layer)
{
    FlagsLength = {0,0};
    Vector = 0;
    Options = options_t();
    Reserved = 0;
    List.clear();

    if (layer.size() < Layer().toPDUByteArray().size())
        return;

    layer >> FlagsLength
        >> Vector
        >> Options
        >> Reserved
        >> List;
}
