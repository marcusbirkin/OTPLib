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
#include <QCoreApplication>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

namespace OTP::PDU::OTPLayer
{

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(
            vector_t Vector = 0,
            pduLength_t PDULength = 0,
            cid_t CID = cid_t::createUuid(),
            folio_t Folio = 0,
            page_t Page = 0,
            page_t LastPage = 0,
            name_t ComponentName = QCoreApplication::applicationName().toUtf8(),
            QObject *parent = nullptr);
    explicit Layer(
            PDUByteArray layer,
            QObject *parent = nullptr);
    bool isValid();
    PDUByteArray toPDUByteArray();
    void fromPDUByteArray(PDUByteArray layer);

    otpIdent_t getPacketIdent() const { return PacketIdent; }
    const vector_t &getVector() { return Vector; }
    void setVector(vector_t value) { Vector = value; }
    const pduLength_t &getPDULength() const { return PDULength; }
    void setPDULength(pduLength_t value) { PDULength = value; }
    const auto &getFooter() const { return Footer; }
    cid_t getCID() const { return CID; }
    void setCID(cid_t value) { CID = value; }
    const folio_t &getFolio() const { return Folio; }
    void setFolio(folio_t value) { Folio = value; }
    const page_t &getPage() const { return Page; }
    void setPage(page_t value) { Page = value; }
    const page_t &getLastPage() const { return LastPage; }
    void setLastPage(page_t value) { LastPage = value; }
    const options_t &getOptions() const { return Options; }
    const reserved_t &getReserved() const { return Reserved; }
    const name_t &getComponentName() const { return ComponentName; }
    void setComponentName(name_t value) { ComponentName = value; }

private:
    otpIdent_t PacketIdent;
    vector_t Vector;
    pduLength_t PDULength;
    footer_t Footer;
    cid_t CID;
    folio_t Folio;
    page_t Page;
    page_t LastPage;
    options_t Options;
    reserved_t Reserved;
    name_t ComponentName;
};

} // namespace

#endif // OTP_LAYER_HPP
