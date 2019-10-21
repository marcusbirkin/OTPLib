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

namespace ACN::OTP
{
    class SocketManager final : public QObject
    {
        Q_OBJECT
    public:
        SocketManager(QNetworkInterface interface);
        ~SocketManager() = default;
        static std::shared_ptr<SocketManager> getInstance(QNetworkInterface interface);

        qint64 writeDatagram(const QNetworkDatagram &datagram);
        bool joinMulticastGroup(const QHostAddress &groupAddress);
        bool leaveMulticastGroup(const QHostAddress &groupAddress);

        QAbstractSocket::SocketState state() { return RXSocket->state(); }

    signals:
        void newDatagram(QNetworkDatagram datagram);

    private:
        SocketManager(const SocketManager&) = delete;
        SocketManager& operator=(const SocketManager&) = delete;
        SocketManager(SocketManager&&) = delete;
        SocketManager& operator=(SocketManager&&) = delete;

        QNetworkInterface interface;
        void setupRXSocket();
        std::unique_ptr<QUdpSocket> RXSocket;
    };
}

#endif // SOCKET_HPP
