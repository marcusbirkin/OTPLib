/**
 * @file        otp_layer.hpp
 * @brief       OTP PDU Layer
 * @details     Part of OTPLib - A QT interface for E1.59
 * @authors     Marcus Birkin
 * @copyright   Copyright (C) 2019 Marcus Birkin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef OTP_LAYER_HPP
#define OTP_LAYER_HPP

#include <QObject>
#include <QCoreApplication>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

/**
 * @internal
 * @brief OTP PDU Layer
 * @details Protocol Data Unit (PDU) containing all other PDUs
 * 
 */
namespace OTP::PDU::OTPLayer
{

/**
 * @internal
 * @brief OTP PDU Layer
 * 
 */
class Layer : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct a Layer
     * 
     * @param Vector Payload Vector
     * @param PDULength PDU Length
     * @param CID Component IDentifer
     * @param Folio Folio Number
     * @param Page Folio page number
     * @param LastPage Folio last page number
     * @param ComponentName Component Name
     * @param parent Parent object
     */
    explicit Layer(
            vector_t Vector = 0,
            pduLength_t PDULength = 0,
            cid_t CID = cid_t::createUuid(),
            folio_t Folio = 0,
            page_t Page = 0,
            page_t LastPage = 0,
            name_t ComponentName = QCoreApplication::applicationName().toUtf8(),
            QObject *parent = nullptr);

    /**
     * @brief Construct a new Layer from an existing layer extracted from a network message
     * @details Used to dissect an on-the wire message
     * 
     * @param layer Byte array to unpack and dissect
     * @param parent Parent object
     */
    explicit Layer(
            PDUByteArray layer,
            QObject *parent = nullptr);

    /**
     * @brief Is the layer valid
     * 
     * @return true Layer is valid
     * @return false Layer is not valid
     */
    bool isValid() const;

    /**
     * @brief Pack the layer into a byte array
     * @details As transmitted "on-the-wire"
     * 
     * @return Packed byte array 
     */
    PDUByteArray toPDUByteArray() const;

    /**
     * @brief Unpack a byte array into layer
     * 
     * @param layer Byte array to unpack and dissect
     */
    void fromPDUByteArray(PDUByteArray layer);

    /**
     * @brief Get PDU OTP Packet Identifier
     * 
     * @return otpIdent_t OTP Packet Identifier
     */
    otpIdent_t getPacketIdent() const { return PacketIdent; }

    /**
     * @brief Get PDU Vector
     * 
     * @return PDU Vector
     */
    const vector_t &getVector() const { return Vector; }

    /**
     * @brief Set PDU Vector
     * 
     * @param value PDU Vector
     */
    void setVector(vector_t value) { Vector = value; }

    /**
     * @brief Get PDU Length
     * 
     * @return PDU Length
     */
    const pduLength_t &getPDULength() const { return PDULength; }

    /**
     * @brief Set PDU Length
     * 
     * @param value PDU Length
     */
    void setPDULength(pduLength_t value) { PDULength = value; }

    /**
     * @brief Get PDU footer option flags
     * 
     * @return Footer
     */
    const footer_t &getFooter() const { return Footer; }

    /**
     * @brief Get PDU Component IDentifer
     * 
     * @return Component IDentifer
     */
    cid_t getCID() const { return CID; }

    /**
     * @brief Set PDU Component IDentifer
     * 
     * @param value Component IDentifer
     */
    void setCID(cid_t value) { CID = value; }

    /**
     * @brief Get PDU Folio number
     * 
     * @return Folio number
     */
    const folio_t &getFolio() const { return Folio; }

    /**
     * @brief Set PDU Folio number
     * 
     * @param value Folio number
     */
    void setFolio(folio_t value) { Folio = value; }

    /**
     * @brief Get PDU Folio page number
     * 
     * @return Folio page number
     */
    const page_t &getPage() const { return Page; }

    /**
     * @brief Set PDU Folio page number
     * 
     * @param value Folio page number
     */
    void setPage(page_t value) { Page = value; }

    /**
     * @brief Get PDU Folio last page number
     * 
     * @return Folio last page number
     */
    const page_t &getLastPage() const { return LastPage; }

    /**
     * @brief Set PDU Folio last page number
     * 
     * @param value Folio last page number
     */
    void setLastPage(page_t value) { LastPage = value; }

    /**
     * @brief Get PDU option flags
     * 
     * @return option flags
     */
    const options_t &getOptions() const { return Options; }
    
    /**
     * @brief Get PDU Reserved value
     * 
     * @return PDU Reserved value
     */
    const reserved_t &getReserved() const { return Reserved; }

    /**
     * @brief Get PDU Component Name
     * 
     * @return Component Name
     */
    const name_t &getComponentName() const { return ComponentName; }

    /**
     * @brief Set PDU Component Name
     * 
     * @param value Component Name
     */
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
