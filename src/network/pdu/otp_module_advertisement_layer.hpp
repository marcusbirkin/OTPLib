/**
 * @file        otp_module_advertisement_layer.hpp
 * @brief       Module Advertisment PDU Layer
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
#ifndef OTP_MODULE_ADVERTISMENT_LAYER_HPP
#define OTP_MODULE_ADVERTISMENT_LAYER_HPP

#include <QObject>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

/**
 * @internal
 * @brief Module Advertisement PDU Layer
 * @details Protocol Data Unit (PDU) containing OTP Module Advertisements
 * 
 */
namespace OTP::PDU::OTPModuleAdvertisementLayer
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
     * @param PDULength PDU Length
     * @param List List of modules to advertise
     * @param parent Parent object
     */
    explicit Layer(
            pduLength_t PDULength = 0,
            list_t List = list_t(),
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
     * @brief Get PDU Reserved value
     * 
     * @return PDU Reserved value
     */
    const reserved_t &getReserved() const { return Reserved; }

    /**
     * @brief Get list of advertised modules
     * 
     * @return List of modules
     */
    const list_t &getList() const { return List; }

    /**
     * @brief Set list of advertised modules
     * 
     * @param value List of advertised modules
     * @return true List added
     * @return false List is too large to add
     */
    bool setList(list_t value);

    /**
     * @brief Add module to list of advertised modules
     * 
     * @param value Module to add
     * @return true Added to list
     * @return false List is too large to add new item
     */
    bool addItem(item_t value);

private:
    vector_t Vector;
    pduLength_t PDULength;
    reserved_t Reserved;
    list_t List;

};

}; // namespace

#endif // OTP_MODULE_ADVERTISMENT_LAYER_HPP
