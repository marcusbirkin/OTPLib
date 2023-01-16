/**
 * @file        otp_transform_message.hpp
 * @brief       Transform Message
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
#ifndef OTP_TRANSFORM_MESSAGE_HPP
#define OTP_TRANSFORM_MESSAGE_HPP

#include <QObject>
#include <QNetworkDatagram>
#include "message_types.hpp"
#include "message_const.hpp"
#include "../pdu/pdu.hpp"

/**
 * @internal
 * @brief Transform Message
 * @details 
 * <b>OTP Transform Message:</b> An OTP Transform Message carries object transform information such as position and rotation. 
 * 
 */
namespace OTP::MESSAGES::OTPTransformMessage
{

/**
 * @internal
 * @brief Transform Message
 * 
 */
class Message : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct a Message
     * 
     * @param CID Component IDentifer
     * @param ComponentName Component Name
     * @param System System number
     * @param FullPointSet Flag as containing A Full Point Set Point for a the System
     * @param parent Parent object
     */
    explicit Message(
            cid_t CID,
            name_t ComponentName,
            system_t System,
            bool FullPointSet,
            QObject *parent = nullptr);

    /**
     * @brief Construct a new Message from a Network datagram
     * @details Used to dissect an on-the wire message
     * 
     * @param message Network datagram
     * @param parent Parent object
     */
    explicit Message(
            QNetworkDatagram message,
            QObject *parent = nullptr);

    /**
     * @brief Is the message valid
     * 
     * @return true Message is valid
     * @return false Message is not valid
     */
    bool isValid() const;

    /**
     * @brief Pack a unicast network datagram from this message
     * @details Intended for unicast replies
     * 
     * @param destAddr Destination IP Address
     * @param folio Folio Number 
     * @param thisPage Folio page number
     * @param lastPage Folio last page numer
     * @return QNetworkDatagram 
     */
    QNetworkDatagram toQNetworkDatagram(
            QHostAddress destAddr,
            folio_t folio,
            page_t thisPage,
            page_t lastPage);

    /**
     * @brief Pack a multicast network datagram(s) from this message
     * @details Intended for multicast announcements
     * 
     * @param transport IPv4 or IPv6
     * @param folio Folio Number
     * @param thisPage Folio page number
     * @param lastPage Folio last page numer
     * @return QList<QNetworkDatagram> 
     */
    QList<QNetworkDatagram> toQNetworkDatagrams(
            QAbstractSocket::NetworkLayerProtocol transport,
            folio_t folio,
            page_t thisPage,
            page_t lastPage)
    {
        QList<QNetworkDatagram> ret;
        if ((transport == QAbstractSocket::IPv4Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
            ret.append(toQNetworkDatagram(
                            QHostAddress(OTP_Transform_Message_IPv4.toIPv4Address()
                                         + transformLayer->getSystem()),
                            folio,
                            thisPage,
                            lastPage));
        if ((transport == QAbstractSocket::IPv6Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
            ret.append(toQNetworkDatagram(
                            QHostAddress(OTP_Transform_Message_IPv6.toIPv6Address()
                                        + transformLayer->getSystem()),
                            folio,
                            thisPage,
                            lastPage));
        return ret;
    }

    /**
     * @brief addModule() result
     * 
     */
    typedef enum {
        OK, /**< Added ok */
        InvalidSystem, /**< Invalid System for this message */
        InvalidAdditional, /**< Empty or invalid Additional */
        InvalidTimestamp, /**< Null timestamp */
        MessageToBig /**< Message is would be too big with this module */
    } addModule_ret;

    /**
     * @brief Module struct
     * @details Complete data structure for a module
     *
     */
    typedef struct {
        OTP::priority_t priority; /**< Priority of module data */
        OTP::address_t address; /**< Point address */
        OTP::timestamp_t sampleTime; /**< Sample time of module data */
        OTP::PDU::OTPModuleLayer::ident_t ident; /**< Module ident */
        OTP::PDU::OTPModuleLayer::additional_t additional; /**< Module specfic additional data */
    } addModule_t;
    
    /**
     * @brief Add module data to message
     * 
     * @param moduleData Module
     * @return Success status 
     */
    addModule_ret addModule(addModule_t &moduleData);

    /**
     * @brief Get only the OTP Layer of the Message
     * 
     * @return OTP Layer
     */
    std::shared_ptr<OTP::PDU::OTPLayer::Layer> getOTPLayer() { return otpLayer; }

    /**
     * @brief Get only the OTP Transform Layer of the Message
     * 
     * @return OTP Transform Layer
     */
    std::shared_ptr<OTP::PDU::OTPTransformLayer::Layer> getTransformLayer() { return transformLayer; }

    /**
     * @brief Get the OTP Point Layers of the Message
     * 
     * @return OTP Point Layers
     */
    QMap<address_t, std::shared_ptr<OTP::PDU::OTPPointLayer::Layer>> getPointLayers() { return pointLayers; }

    /**
     * @brief Get the OTP Module Layers of the Message
     * 
     * @return OTP Module Layers indexed by point address
     */
    QMultiMap<address_t, std::shared_ptr<OTP::PDU::OTPModuleLayer::Layer>> getModuleLayers() { return moduleLayers; }

private:
    /**
     * @brief Update all PDU lengths to match the data contained
     * 
     */
    void updatePduLength();

    /**
     * @brief Convert message into a ByteArray
     * @details As transmitted "on-the-wire"
     * 
     * @return QByteArray 
     */
    QByteArray toByteArray() const;

    std::shared_ptr<OTP::PDU::OTPLayer::Layer> otpLayer;
    std::shared_ptr<OTP::PDU::OTPTransformLayer::Layer> transformLayer;
    QMap<address_t, std::shared_ptr<OTP::PDU::OTPPointLayer::Layer>> pointLayers;
    QMultiMap<address_t, std::shared_ptr<OTP::PDU::OTPModuleLayer::Layer>> moduleLayers;
};

} // namespace

#endif // OTP_NAME_ADVERTISEMENT_MESSAGE_HPP
