/**
 * @file        otp_module_advertisement_message.hpp
 * @brief       Module Advertisement Message
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
#ifndef OTP_MODULE_ADVERTISEMENT_MESSAGE_HPP
#define OTP_MODULE_ADVERTISEMENT_MESSAGE_HPP

#include <QObject>
#include <QNetworkDatagram>
#include "message_types.hpp"
#include "message_const.hpp"
#include "../pdu/pdu.hpp"

/**
 * @internal
 * @brief Module Advertisement Message
 * @details 
 * <b>OTP Module Advertisement Message:</b> An OTP Module Advertisement Message contains a packed list of the Module Identifiers of Modules supported by a Consumer.
 * 
 */
namespace OTP::MESSAGES::OTPModuleAdvertisementMessage
{

/**
 * @internal
 * @brief Module Advertisement Message
 * 
 */
class Message : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct a Message
     * 
     * @param mode Component operating mode
     * @param CID Component IDentifer
     * @param ComponentName Component Name
     * @param ModuleList List of modules to be advertised
     * @param parent Parent object
     */
    explicit Message(
            OTP::mode_e mode,
            cid_t CID,
            name_t ComponentName,
            list_t ModuleList,
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
    bool isValid();

    /**
     * @brief Pack a unicast network datagram from this message
     * @details Intended for unicast replies
     * 
     * @param destAddr Destination IP Address
     * @param folio Folio 
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
     * @param folio Folio
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
                            OTP_Advertisement_Message_IPv4,
                            folio,
                            thisPage,
                            lastPage));
        if ((transport == QAbstractSocket::IPv6Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
            ret.append(toQNetworkDatagram(
                            OTP_Advertisement_Message_IPv6,
                            folio,
                            thisPage,
                            lastPage));
        return ret;
    }

    /**
     * @brief Add an module to the list for advertisment
     * 
     * @param value Module to add
     * @return true Item was added successfully
     * @return false Item was not added successfully
     */
    bool addItem(item_t value) {
        auto ret = moduleAdvertisementLayer->addItem(value);
        updatePduLength();
        return ret;
    }

    /**
     * @brief Get only the OTP Layer of the Message
     * 
     * @return OTP Layer
     */
    std::shared_ptr<OTP::PDU::OTPLayer::Layer> getOTPLayer() { return otpLayer; }

    /**
     * @brief Get only the OTP Advertisement Layer of the Message
     * 
     * @return OTP Advertisement Layer
     */
    std::shared_ptr<OTP::PDU::OTPAdvertisementLayer::Layer> getAdvertisementLayer() { return advertisementLayer; }

    /**
     * @brief Get only the OTP Module Advertisement Layer of the Message
     * 
     * @return OTP Module Advertisement Layer
     */
    std::shared_ptr<OTP::PDU::OTPModuleAdvertisementLayer::Layer> getModuleAdvertisementLayer() { return moduleAdvertisementLayer; }

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
    QByteArray toByteArray();

    std::shared_ptr<OTP::PDU::OTPLayer::Layer> otpLayer;
    std::shared_ptr<OTP::PDU::OTPAdvertisementLayer::Layer> advertisementLayer;
    std::shared_ptr<OTP::PDU::OTPModuleAdvertisementLayer::Layer> moduleAdvertisementLayer;
};

} // namespace

#endif // OTP_NAME_ADVERTISEMENT_MESSAGE_HPP
