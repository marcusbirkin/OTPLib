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
#include "otp_module_layer.hpp"

using namespace ACN::OTP::PDU::OTPModuleLayer;

Layer::Layer(
        ACN::OTP::PDU::flags_length_t::pduLength_t PDULength,
        vector_t::manufacturerID_t ManufacturerID,
        vector_t::moduleNumber_t ModuleNumber,
        QObject *parent) :
    QObject(parent),
    FlagsLength({FLAGS, PDULength}),
    Vector({ManufacturerID, ModuleNumber}),
    Additional(QByteArray())
{}

Layer::Layer(
        ACN::OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    FlagsLength({0, 0}),
    Vector({0,0}),
    Additional(QByteArray())
{
    if (layer.size() != Layer().toPDUByteArray().size())
        return;

    layer >> FlagsLength
        >> Vector
        >> Additional;
}

bool Layer::isValid()
{
    if (FlagsLength.Flags != FLAGS) return false;
    if (FlagsLength.PDULength == 0) return false;
    if (Additional.isNull()) return false;
    return true;
}

ACN::OTP::PDU::PDUByteArray Layer::toPDUByteArray()
{
    PDUByteArray ret;
    return ret << FlagsLength
        << Vector
        << Additional;
}

void Layer::fromPDUByteArray(ACN::OTP::PDU::PDUByteArray layer)
{
    FlagsLength = {0,0};
    Vector = {0,0};
    Additional.clear();
    if (layer.size() != getPDULength(layer))
        return;

    layer >> FlagsLength
        >> Vector
        >> Additional;
}
