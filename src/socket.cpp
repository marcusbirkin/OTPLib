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

using namespace OTP;

typedef std::pair<QString, QAbstractSocket::NetworkLayerProtocol> instanceKey_t;
static QMap<instanceKey_t, QWeakPointer<SocketManager>> instances;

SocketManager::SocketManager(QNetworkInterface interface, QAbstractSocket::NetworkLayerProtocol transport) : QObject(),
    interface(interface), transport(transport)
{
    assert( (transport == QAbstractSocket::IPv4Protocol) || (transport == QAbstractSocket::IPv6Protocol) );
    RXSocket.reset(new QUdpSocket(this));

    QVector<QHostAddress> bindAddr;
    switch (transport) {
        case QAbstractSocket::IPv4Protocol:
        {
            RXSocket->bind(QHostAddress::AnyIPv4, OTP::OTP_PORT);
        } break;

        case QAbstractSocket::IPv6Protocol:
        {
            RXSocket->bind(QHostAddress::AnyIPv6, OTP::OTP_PORT);
        } break;

        default: return;
    }

    connect(RXSocket.get(), &QUdpSocket::readyRead,
        [this]() {
            if (!RXSocket || !RXSocket->isValid()) return;
            while (RXSocket->hasPendingDatagrams())
                emit this->newDatagram(RXSocket->receiveDatagram());
        });

    connect(RXSocket.get(), &QUdpSocket::stateChanged,
        [this](QAbstractSocket::SocketState state) {
            if (!RXSocket || !RXSocket->isValid()) return;
            emit this->stateChanged(state);
        });
};

SocketManager::~SocketManager()
{
    instanceKey_t key = {interface.name(), transport};
    instances.remove(key);
    RXSocket->close();
    RXSocket.release();
}

QSharedPointer<SocketManager> SocketManager::getSocket(QNetworkInterface interface, QAbstractSocket::NetworkLayerProtocol transport)
{
    instanceKey_t key = {interface.name(), transport};
    QSharedPointer<SocketManager> sp;
    if (!instances.contains(key)) {
        sp = QSharedPointer<SocketManager>(new SocketManager(interface, transport));
        instances.insert(key, sp);
    } else {
        sp = instances.value(key).lock();
    }
    return sp;
}

bool SocketManager::isValid(QNetworkInterface interface)
{
    return (
        interface.flags().testFlag(QNetworkInterface::IsUp) &&
        interface.flags().testFlag(QNetworkInterface::IsRunning) &&
        interface.flags().testFlag(QNetworkInterface::CanMulticast)
    );
}

bool SocketManager::writeDatagram(const QNetworkDatagram &datagram)
{
    // Sending unicast to self?
    if (!datagram.destinationAddress().isMulticast() &&
            !datagram.destinationAddress().isBroadcast()) {
        const auto addressEntries = interface.addressEntries();
        for (const auto &ifaceAddr : addressEntries)
            if (ifaceAddr.ip() == datagram.destinationAddress())
            {
                emit this->newDatagram(datagram);
                return true;
            }
    }

    auto socket = QUdpSocket();
    const auto addressEntries = interface.addressEntries();
    for (const auto &ifaceAddr : addressEntries)
        if (ifaceAddr.ip().protocol() == transport)
            socket.bind(ifaceAddr.ip());

    socket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, QVariant(1));
    socket.setMulticastInterface(interface);

    return socket.writeDatagram(datagram) == datagram.data().size();
}

bool SocketManager::joinMulticastGroup(const QHostAddress &groupAddress)
{
    return RXSocket->joinMulticastGroup(groupAddress, interface);
}

bool SocketManager::leaveMulticastGroup(const QHostAddress &groupAddress)
{
    return RXSocket->leaveMulticastGroup(groupAddress, interface);
}

