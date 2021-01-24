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
#ifndef OTP_MODULE_ADVERTISMENT_LAYER_HPP
#define OTP_MODULE_ADVERTISMENT_LAYER_HPP

#include <QObject>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

namespace OTP::PDU::OTPModuleAdvertisementLayer
{

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(
            pduLength_t PDULength = 0,
            list_t List = list_t(),
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
    const list_t &getList() { return List; }
    bool setList(list_t value);
    bool addItem(item_t value);

private:
    vector_t Vector;
    pduLength_t PDULength;
    reserved_t Reserved;
    list_t List;

};

}; // namespace

#endif // OTP_MODULE_ADVERTISMENT_LAYER_HPP
