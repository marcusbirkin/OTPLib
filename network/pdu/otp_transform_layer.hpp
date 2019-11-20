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
#ifndef OTP_TRANSFORM_LAYER_HPP
#define OTP_TRANSFORM_LAYER_HPP

#include <QObject>
#include <QDateTime>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

namespace OTP::PDU::OTPTransformLayer
{

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(
            flags_length_t::pduLength_t PDULength = 0,
            system_t System = 0,
            timestamp_t Timestamp = static_cast<timestamp_t>(QDateTime::currentMSecsSinceEpoch() * 1000),
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
    const system_t &getSystem() { return System; }
    void setSystem(system_t value) { System = value; }
    const timestamp_t &getTimestamp() { return Timestamp; }
    void setTimestamp(timestamp_t value) { Timestamp = value; }
    const options_t &getOptions() { return Options; }
    const reserved_t &getReserved() { return Reserved; }

private:
    flags_length_t FlagsLength;
    vector_t Vector;
    system_t System;
    timestamp_t Timestamp;
    options_t Options;
    reserved_t Reserved;
};

}; // namespace

#endif // OTP_LAYER_OPT_H
