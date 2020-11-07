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
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <exception>
#include <memory>

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

    class SocketManager final : public QObject
    {
        Q_OBJECT
    public:
        SocketManager(QNetworkInterface interface, QAbstractSocket::NetworkLayerProtocol transport);
        ~SocketManager();
        static QSharedPointer<SocketManager> getSocket(QNetworkInterface interface, QAbstractSocket::NetworkLayerProtocol transport);

        static bool isValid(QNetworkInterface interface);

        static bool writeDatagrams(QNetworkInterface interface, const QList<QNetworkDatagram> &datagrams)
        {
            for (auto datagram : datagrams)
                if (!getSocket(interface, datagram.destinationAddress().protocol())->writeDatagram(datagram))
                {
                    qDebug() << "writeDatagram() failed to" << datagram.destinationAddress();
                    return false;
                }
            return true;
        }

        bool writeDatagram(const QNetworkDatagram &datagram);
        bool joinMulticastGroup(const QHostAddress &groupAddress);
        bool joinMulticastGroup(const QList<QHostAddress> &groupAddress)
        {
            bool ret = true;
            for (auto address : groupAddress)
                if (!joinMulticastGroup(address)) ret = false;
            return ret;
        }
        bool leaveMulticastGroup(const QHostAddress &groupAddress);
        bool leaveMulticastGroup(const QList<QHostAddress> &groupAddress)
        {
            bool ret = true;
            for (auto address : groupAddress)
                if (!leaveMulticastGroup(address)) ret = false;
            return ret;
        }

        QAbstractSocket::SocketState state() { return RXSocket->state(); }

    signals:
        void newDatagram(QNetworkDatagram datagram);
        void stateChanged(QAbstractSocket::SocketState state);

    private:
        SocketManager(const SocketManager&) = delete;
        SocketManager& operator=(const SocketManager&) = delete;
        SocketManager(SocketManager&&) = delete;
        SocketManager& operator=(SocketManager&&) = delete;

        QNetworkInterface interface;
        QAbstractSocket::NetworkLayerProtocol transport;
        void setupRXSocket();
        std::unique_ptr<QUdpSocket> RXSocket;
    };
}

#endif // SOCKET_HPP
