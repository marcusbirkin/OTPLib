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
#ifndef OTP_ROOT_LAYER_HPP
#define OTP_ROOT_LAYER_HPP

#include <QObject>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

namespace ACN::OTP::PDU::OTPRootLayer
{

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(
            flags_length_t::pduLength_t PDULength = 0,
            cid_t CID = cid_t::createUuid(),
            QObject *parent = nullptr);
    explicit Layer(
            PDUByteArray layer,
            QObject *parent = nullptr);
    bool isValid();
    PDUByteArray toPDUByteArray();
    void fromPDUByteArray(PDUByteArray layer);

    ambleSize_t getPreableSize() const { return PreableSize; }
    ambleSize_t getPostableSize() const { return PostableSize; }
    acnIdent_t getPacketIdent() const { return PacketIdent; }
    flags_length_t::flags_t getFlags() const { return FlagsLength.Flags; }
    flags_length_t::pduLength_t getPDULength() const { return FlagsLength.PDULength; }
    void setPDULength(flags_length_t::pduLength_t value) { FlagsLength.PDULength = value; }
    vector_t getVector() const { return Vector; }
    cid_t getCID() const { return CID; }
    void setCID(cid_t value) { CID = value; }

private:
    ambleSize_t PreableSize;
    ambleSize_t PostableSize;
    acnIdent_t PacketIdent;
    flags_length_t FlagsLength;
    vector_t Vector;
    cid_t CID;
};

} // namespace

#endif // OTP_ROOT_LAYER_HPP
