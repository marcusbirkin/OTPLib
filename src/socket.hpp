/**
 * @file        socket.hpp
 * @brief       Socket Manager
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
 *  but WITHOUT ANYs WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <exception>
#include <memory>

#if defined MAKE_OTP_LIB
    /**
     * @brief Export symbol as shared library
     * 
     */
    #define OTP_LIB_EXPORT Q_DECL_EXPORT
#else
    /**
     * @brief Import symbol from shared library
     * 
     */
    #define OTP_LIB_EXPORT Q_DECL_IMPORT
#endif

namespace OTP
{
    inline bool operator< (const Q_IPV6ADDR& l, const Q_IPV6ADDR& r){
        for (int n = 0; n < 16; n++)
        {
            if (l.c[n] > r.c[n]) return false;
            if (l.c[n] < r.c[n]) return true;
        }
        return false;
    }
    inline bool operator> (const Q_IPV6ADDR& l, const Q_IPV6ADDR& r){ return r < l; }
    inline bool operator<=(const Q_IPV6ADDR& l, const Q_IPV6ADDR& r){ return !(l > r); }
    inline bool operator>=(const Q_IPV6ADDR& l, const Q_IPV6ADDR& r){ return !(l < r); }

    /**
     * @internal
     * @brief Socket manager and socket interface
     * @details Manages network sockets creation and distruction
     * and provides primary interface to sending and reciving of datagrams
     * 
     */
    class OTP_LIB_EXPORT SocketManager final : public QObject
    {
        Q_OBJECT
    public:
        ~SocketManager();

        /**
         * @brief Get the socket object assigned to, or create a new socket object for, this combination of interface and transport
         * 
         * @param interface Network interace to bind to 
         * @param transport Transport, either IPv4 or IPv6, to use
         * @return SocketManager object
         */
        static QSharedPointer<SocketManager> getSocket(QNetworkInterface interface, QAbstractSocket::NetworkLayerProtocol transport);

        /**
         * @brief Is this network interface suitable for OTP?
         * @details This checks the interace is up, running, and able to multicast
         * 
         * @param interface Network interface to check
         * @return true Interface is suitable
         * @return false Interface is no suitable
         */
        static bool isValid(QNetworkInterface interface);

        /**
         * @brief Send a network datagram, on a specfic network interface
         * 
         * @param interface Network interface to use
         * @param datagrams Datagram to send
         * @return true Send successful
         * @return false Send failed
         */
        static bool writeDatagrams(QNetworkInterface interface, const QList<QNetworkDatagram> &datagrams)
        {
            for (const auto &datagram : datagrams)
                if (!getSocket(interface, datagram.destinationAddress().protocol())->writeDatagram(datagram))
                {
                    qDebug() << "writeDatagram() failed to" << datagram.destinationAddress();
                    return false;
                }
            return true;
        }

        /**
         * @brief Send a network datagram
         * 
         * @param datagram Datagram to send
         * @return true Send successful 
         * @return false Send failed 
         */
        bool writeDatagram(const QNetworkDatagram &datagram);

        /**
         * @brief Join multicast group
         * 
         * @param groupAddress Address to join
         * @return true Join successful
         * @return false Join failed
         */
        bool joinMulticastGroup(const QHostAddress &groupAddress);

        /**
         * @brief Join serveral multicast groups
         * 
         * @param groupAddress Addresses to join
         * @return true Join successful
         * @return false At least one join failed
         */
        bool joinMulticastGroup(const QList<QHostAddress> &groupAddress)
        {
            bool ret = true;
            for (const auto &address : groupAddress)
                if (!joinMulticastGroup(address)) ret = false;
            return ret;
        }

        /**
         * @brief Leave multicast group
         * 
         * @param groupAddress Adress to leave
         * @return true Leave successful
         * @return false Leave failed
         */
        bool leaveMulticastGroup(const QHostAddress &groupAddress);

        /**
         * @brief Leave several multicast group
         * 
         * @param groupAddress Addresses to leave
         * @return true Leave successful
         * @return false At least one leave failed
         */
        bool leaveMulticastGroup(const QList<QHostAddress> &groupAddress)
        {
            bool ret = true;
            for (const auto &address : groupAddress)
                if (!leaveMulticastGroup(address)) ret = false;
            return ret;
        }

        /**
         * @brief Obtain the current state of this socket
         * 
         * @return Socket status
         */
        QAbstractSocket::SocketState state();

    signals:
        /**
         * @brief Emitted when a new datagram is received
         * 
         * @param datagram New datagram
         */
        void newDatagram(QNetworkDatagram datagram);

        /**
         * @brief Emitted when the socket state changes
         * 
         * @param state New socket state
         */
        void stateChanged(QAbstractSocket::SocketState state);

    private:
        SocketManager(QNetworkInterface interface, QAbstractSocket::NetworkLayerProtocol transport);
        SocketManager(const SocketManager&) = delete;
        SocketManager& operator=(const SocketManager&) = delete;
        SocketManager(SocketManager&&) = delete;
        SocketManager& operator=(SocketManager&&) = delete;

        typedef std::pair<QString, QAbstractSocket::NetworkLayerProtocol> instanceKey_t;
        typedef QMap<instanceKey_t, QWeakPointer<SocketManager>> instances_t;
        static instances_t& getInstances(); /*!< Get list of all socket instances */

        QNetworkInterface interface; /*!< Network Interface assigned to this instance */
        QAbstractSocket::NetworkLayerProtocol transport; /*!< Network transport to this instance */
        void setupRXSocket(); /*!< Setup listener socket for this instance */
        std::unique_ptr<QUdpSocket> RXSocket; /*!< Listener socket for this instance */
    };
}

#endif // SOCKET_HPP
