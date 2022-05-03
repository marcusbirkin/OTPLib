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

using namespace OTP::PDU;
using namespace OTP::PDU::OTPModuleLayer;

Layer::Layer(QObject *parent) :
    QObject(parent),
    ModuleIdent(ident_t()),
    PDULength(0),
    Additional(QByteArray())
{}

Layer::Layer(manufacturerID_t ManufacturerID,
        pduLength_t PDULength,
        moduleNumber_t ModuleNumber,
        QObject *parent) :
    QObject(parent),
    ModuleIdent({ManufacturerID, ModuleNumber}),
    PDULength(PDULength),
    Additional(QByteArray())
{}

Layer::Layer(
        OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    ModuleIdent(ident_t()),
    PDULength(0),
    Additional(QByteArray())
{
    if (layer.size() != Layer().toPDUByteArray().size())
        return;

    layer >> ModuleIdent.ManufacturerID
        >> PDULength
        >> ModuleIdent.ModuleNumber
        >> Additional;
}

bool Layer::isValid()
{
    if (PDULength != toPDUByteArray().size() - LENGTHOFFSET) return false;
    if (Additional.isNull()) return false;
    return true;
}

OTP::PDU::PDUByteArray Layer::toPDUByteArray()
{
    PDUByteArray ret;
    return ret << ModuleIdent.ManufacturerID
        << PDULength
        << ModuleIdent.ModuleNumber
        << Additional;
}

void Layer::fromPDUByteArray(OTP::PDU::PDUByteArray layer)
{
    ModuleIdent = {0,0};
    PDULength = 0;
    Additional.clear();

    if (layer.size() < Layer().toPDUByteArray().size())
        return;

    layer >> ModuleIdent.ManufacturerID
        >> PDULength
        >> ModuleIdent.ModuleNumber
        >> Additional;
}
