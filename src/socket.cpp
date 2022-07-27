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

SocketManager::SocketManager(QNetworkInterface interface, QAbstractSocket::NetworkLayerProtocol transport) : QObject(),
    interface(interface), transport(transport)
{
    assert( (transport == QAbstractSocket::IPv4Protocol) || (transport == QAbstractSocket::IPv6Protocol) );
    RXSocket.reset(new QUdpSocket(this));

    switch (transport) {
        case QAbstractSocket::IPv4Protocol:
        {
            RXSocket->bind(QHostAddress::AnyIPv4,
                           OTP::OTP_PORT,
                           QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
        } break;

        case QAbstractSocket::IPv6Protocol:
        {
            RXSocket->bind(QHostAddress::AnyIPv6,
                           OTP::OTP_PORT,
                           QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
        } break;

        default: return;
    }

    connect(RXSocket.get(), &QUdpSocket::readyRead, this,
        [this]() {
            if (!RXSocket || !RXSocket->isValid()) return;
            while (RXSocket->hasPendingDatagrams())
                emit this->newDatagram(RXSocket->receiveDatagram());
        });

    connect(RXSocket.get(), &QUdpSocket::stateChanged, this,
        [this](QAbstractSocket::SocketState state) {
            if (!RXSocket || !RXSocket->isValid()) return;
            emit this->stateChanged(state);
        });
};

SocketManager::~SocketManager()
{
    instanceKey_t key = {interface.name(), transport};
    getInstances().remove(key);
    RXSocket->close();
    RXSocket.release();
}

SocketManager::instances_t& SocketManager::getInstances()
{
    static instances_t instances;
    return instances;
}

QSharedPointer<SocketManager> SocketManager::getSocket(QNetworkInterface interface, QAbstractSocket::NetworkLayerProtocol transport)
{
    instanceKey_t key = {interface.name(), transport};
    QSharedPointer<SocketManager> sp;
    if (!getInstances().contains(key)) {
        sp = QSharedPointer<SocketManager>(new SocketManager(interface, transport));
        getInstances().insert(key, sp);
    } else {
        sp = getInstances().value(key).lock();
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

QAbstractSocket::SocketState SocketManager::state() {
    if (RXSocket)
        return RXSocket->state();
    else
        return QAbstractSocket::UnconnectedState;
}


