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
#ifndef OTP_MODULE_LAYER_HPP
#define OTP_MODULE_LAYER_HPP

#include <QObject>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

namespace OTP::PDU::OTPModuleLayer
{

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(QObject *parent = nullptr);
    explicit Layer(
            manufacturerID_t ManufacturerID,
            pduLength_t PDULength,
            moduleNumber_t ModuleNumber,
            QObject *parent = nullptr);
    explicit Layer(
            PDUByteArray layer,
            QObject *parent = nullptr);
    bool isValid();
    PDUByteArray toPDUByteArray();
    void fromPDUByteArray(PDUByteArray layer);

    const manufacturerID_t &getManufacturerID() { return ModuleIdent.ManufacturerID; }
    void setManufacturerID(manufacturerID_t value) { ModuleIdent.ManufacturerID = value; }
    static pduLength_t getPDULength(PDUByteArray layer)
    {
        ident_t ModuleIdent;
        pduLength_t PDULength;
        layer >> ModuleIdent.ManufacturerID >> PDULength;
        return PDULength;
    }
    const pduLength_t &getPDULength() { return PDULength; }
    void setPDULength(pduLength_t value) { PDULength = value; }

    const moduleNumber_t &getModuleNumber() { return ModuleIdent.ModuleNumber; }
    void setModuleNumber(moduleNumber_t value) { ModuleIdent.ModuleNumber = value; }
    const additional_t &getAdditional() { return Additional; }
    void setAdditional(QByteArray value) { Additional = value; }

private:
    ident_t ModuleIdent;
    pduLength_t PDULength;
    additional_t Additional;

};

}; // namespace

#endif // OTP_MODULE_LAYER_HPP
