/**
 * @file        otp_advertisement_layer.hpp
 * @brief       Advertisement PDU Layer
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
#ifndef OTP_ADVERTISMENT_LAYER_HPP
#define OTP_ADVERTISMENT_LAYER_HPP

#include <QObject>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

/**
 * @internal
 * @brief Advertisement PDU Layer
 * @details Protocol Data Unit (PDU) containing OTP Advertisements
 * 
 */
namespace OTP::PDU::OTPAdvertisementLayer
{

/**
 * @internal
 * @brief Advertisement PDU Layer
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
     * @param parent Parent object
     */
    explicit Layer(
            vector_t Vector = 0,
            pduLength_t PDULength = 0,
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
     * @brief Get PDU Reserved value
     * 
     * @return PDU Reserved value
     */
    const reserved_t &getReserved() const { return Reserved; }

private:
    vector_t Vector;
    pduLength_t PDULength;
    reserved_t Reserved;
};

} // namespace

#endif // OTP_ADVERTISMENT_LAYER_HPP
