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
#ifndef OTP_LAYER_HPP
#define OTP_LAYER_HPP

#include <QObject>
#include <QApplication>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

namespace ACN::OTP::PDU::OTPLayer
{

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(flags_length_t::pduLength_t PDULength = 0,
            vector_t Vector = 0,
            sequence_t Sequence = 0,
            folio_t Folio = 0,
            page_t Page = 0,
            page_t LastPage = 0,
            name_t ProducerName = QApplication::applicationName().toUtf8(),
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
    void setVector(vector_t value) { Vector = value; }
    const protocol_t &getProtocolVer() { return ProtocolVer; }
    const sequence_t &getSequence() { return Sequence; }
    void setSequence(sequence_t value) { Sequence = value; }
    const folio_t &getFolio() { return Folio; }
    void setFolio(folio_t value) { Folio = value; }
    const page_t &getPage() { return Page; }
    void setPage(page_t value) { Page = value; }
    const page_t &getLastPage() { return LastPage; }
    void setLastPage(page_t value) { LastPage = value; }
    const options_t &getOptions() { return Options; }
    const reserved_t &getReserved() { return Reserved; }
    const name_t &getProducerName() { return ProducerName; }
    void setProducerName(name_t value) { ProducerName = value; }

private:
    flags_length_t FlagsLength;
    vector_t Vector;
    protocol_t ProtocolVer;
    sequence_t Sequence;
    folio_t Folio;
    page_t Page;
    page_t LastPage;
    options_t Options;
    reserved_t Reserved;
    name_t ProducerName;
};

} // namespace

#endif // OTP_LAYER_HPP
