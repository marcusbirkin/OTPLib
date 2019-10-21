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
#include "socket.hpp"
#include <QNetworkDatagram>
#include "network/pdu/pdu_const.hpp"

using namespace ACN::OTP;

/* TODO Add IPv6 support */
SocketManager::SocketManager(QNetworkInterface interface) : QObject(),
    interface(interface)
{
    RXSocket.reset(new QUdpSocket(this));

    RXSocket->bind(
            QHostAddress::AnyIPv4,
            ACN::OTP::ACN_SDT_MULTICAST_PORT);

    connect(RXSocket.get(), &QUdpSocket::readyRead,
        [this]() {
            if (!RXSocket || !RXSocket->isValid()) return;
            while (RXSocket->hasPendingDatagrams())
                emit this->newDatagram(RXSocket->receiveDatagram());
        });
};

std::shared_ptr<SocketManager> SocketManager::getInstance(QNetworkInterface interface)
{
    static QMap<QString, std::shared_ptr<SocketManager>> instances;
    if (!instances.contains(interface.name()))
        instances.insert(interface.name(), std::make_shared<SocketManager>(interface));
    return instances.value(interface.name());
}

qint64 SocketManager::writeDatagram(const QNetworkDatagram &datagram)
{
    auto socket = QUdpSocket();
    for (auto ifaceAddr : interface.addressEntries()) {
        if (ifaceAddr.ip().protocol() == QAbstractSocket::IPv4Protocol) {
            socket.bind(ifaceAddr.ip());
        }
    }

    socket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, QVariant(1));
    socket.setMulticastInterface(interface);

    return socket.writeDatagram(datagram);
}

bool SocketManager::joinMulticastGroup(const QHostAddress &groupAddress)
{
    return RXSocket->joinMulticastGroup(groupAddress, interface);
}

bool SocketManager::leaveMulticastGroup(const QHostAddress &groupAddress)
{
    return RXSocket->leaveMulticastGroup(groupAddress, interface);
}

