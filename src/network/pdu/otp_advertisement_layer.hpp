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
#ifndef OTP_ADVERTISMENT_LAYER_HPP
#define OTP_ADVERTISMENT_LAYER_HPP

#include <QObject>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

namespace OTP::PDU::OTPAdvertisementLayer
{

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(
            vector_t Vector = 0,
            pduLength_t PDULength = 0,
            QObject *parent = nullptr);
    explicit Layer(
            PDUByteArray layer,
            QObject *parent = nullptr);
    bool isValid();
    PDUByteArray toPDUByteArray();
    void fromPDUByteArray(PDUByteArray layer);

    const pduLength_t &getPDULength() { return PDULength; }
    void setPDULength(pduLength_t value) { PDULength = value; }
    const vector_t &getVector() { return Vector; }
    void setVector(vector_t value) { Vector = value; }

private:
    vector_t Vector;
    pduLength_t PDULength;
    reserved_t Reserved;
};

} // namespace

#endif // OTP_ADVERTISMENT_LAYER_HPP
