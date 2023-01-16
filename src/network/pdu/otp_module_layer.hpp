/**
 * @file        otp_module_layer.hpp
 * @brief       Module PDU Layer
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
#ifndef OTP_MODULE_LAYER_HPP
#define OTP_MODULE_LAYER_HPP

#include <QObject>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

/**
 * @internal
 * @brief Module PDU Layer
 * @details Protocol Data Unit (PDU) containing OTP Module Transform Data
 * 
 */
namespace OTP::PDU::OTPModuleLayer
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
     * @param parent Parent object 
     */
    explicit Layer(QObject *parent = nullptr);

    /**
     * @brief Construct a new Layer object
     * 
     * @param ManufacturerID Manufacturer ID
     * @param PDULength PDU Length
     * @param ModuleNumber Module Number
     * @param parent Parent object
     */
    explicit Layer(
            manufacturerID_t ManufacturerID,
            pduLength_t PDULength,
            moduleNumber_t ModuleNumber,
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
    bool isValid();

    /**
     * @brief Pack the layer into a byte array
     * @details As transmitted "on-the-wire"
     * 
     * @return Packed byte array 
     */
    PDUByteArray toPDUByteArray();

    /**
     * @brief Unpack a byte array into layer
     * 
     * @param layer Byte array to unpack and dissect
     */
    void fromPDUByteArray(PDUByteArray layer);

    /**
     * @brief Get PDU Manufacturer ID
     * 
     * @return Manufacturer ID
     */
    const manufacturerID_t &getManufacturerID() { return ModuleIdent.ManufacturerID; }

    /**
     * @brief Set PDU Manufacturer ID
     * 
     * @param value Manufacturer ID
     */
    void setManufacturerID(manufacturerID_t value) { ModuleIdent.ManufacturerID = value; }

    /**
     * @brief Get PDU Length from byte array
     * @details Length of PDU excluding Vector and Length fields
     * 
     * @param layer Byte array to unpack
     * @return PDU Length
     */
    static pduLength_t getPDULength(PDUByteArray layer)
    {
        ident_t ModuleIdent;
        pduLength_t PDULength;
        layer >> ModuleIdent.ManufacturerID >> PDULength;
        return PDULength;
    }

    /**
     * @brief Get PDU Length
     * 
     * @return PDU Length
     */
    const pduLength_t &getPDULength() { return PDULength; }

    /**
     * @brief Set PDU Length
     * 
     * @param value PDU Length
     */
    void setPDULength(pduLength_t value) { PDULength = value; }

    /**
     * @brief Get PDU Module Number
     * 
     * @return Module Number
     */
    const moduleNumber_t &getModuleNumber() { return ModuleIdent.ModuleNumber; }

    /**
     * @brief Set PDU Module Number
     * 
     * @param value Module Number
     */
    void setModuleNumber(moduleNumber_t value) { ModuleIdent.ModuleNumber = value; }

    /**
     * @brief Get PDU Additional Fields
     * 
     * @return Additional Fields
     */
    const additional_t &getAdditional() { return Additional; }

    /**
     * @brief Set module sepcfic PDU Additional Fields
     * 
     * @param value Additional Fields
     */
    void setAdditional(const additional_t &value) { Additional = value; }

private:
    ident_t ModuleIdent;
    pduLength_t PDULength;
    additional_t Additional;

};

}; // namespace

#endif // OTP_MODULE_LAYER_HPP
