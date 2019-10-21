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
#ifndef OTP_NAME_ADVERTISMENT_LAYER_HPP
#define OTP_NAME_ADVERTISMENT_LAYER_HPP

#include <QObject>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

namespace ACN::OTP::PDU::OTPNameAdvertisementLayer
{

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(
            flags_length_t::pduLength_t PDULength = 0,
            options_t Options = options_t(),
            list_t List = list_t(),
            QObject *parent = nullptr);
    explicit Layer(
            PDUByteArray layer,
            QObject *parent = nullptr);
    bool isValid();
    PDUByteArray toPDUByteArray();
    void fromPDUByteArray(PDUByteArray layer);

    const flags_length_t::flags_t &getFlags() { return FlagsLength.Flags; }
    const flags_length_t::pduLength_t &getPDULength() { return FlagsLength.PDULength; }
    void setPDULength(flags_length_t::pduLength_t value) { FlagsLength.PDULength = value; }
    const vector_t &getVector() { return Vector; }
    const options_t &getOptions() { return Options; }
    void setOptions(options_t value) { Options = value; }
    const list_t &getList() { return List; }
    void setList(list_t value) { List = value; }

private:
    flags_length_t FlagsLength;
    vector_t Vector;
    options_t Options;
    reserved_t Reserved;
    list_t List;

};

}; // namespace

#endif // OTP_MODULE_ADVERTISMENT_LAYER_HPP
