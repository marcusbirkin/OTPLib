/*
    OTPLib
    A QT interface for E1.59 (Entertainment Technology Object Transform Protocol (OTP))
    Copyright (C) 2020 Marcus Birkin

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
#include "component.hpp"
#include "network/messages/messages.hpp"

using namespace OTP;

Component::Component(
        QNetworkInterface iface,
        QAbstractSocket::NetworkLayerProtocol transport,
        cid_t CID,
        name_t name,
        QObject *parent) :
    QObject(parent),
    otpNetwork(new Container(this)),
    iface(iface),
    transport(transport),
    CID(CID),
    name(name)
{
    // Component Signals
    connect(otpNetwork.get(), &Container::newComponent, this, &Component::newComponent);
    connect(otpNetwork.get(), &Container::removedComponent, this, &Component::removedComponent);

    connect(otpNetwork.get(), qOverload<const cid_t&, const name_t&>(&Container::updatedComponent),
            this, qOverload<const cid_t&, const name_t&>(&Component::updatedComponent));
    connect(otpNetwork.get(), qOverload<const OTP::cid_t&, const QHostAddress&>(&Container::updatedComponent),
            this, qOverload<const OTP::cid_t&, const QHostAddress&>(&Component::updatedComponent));
    connect(otpNetwork.get(), qOverload<const OTP::cid_t&, const moduleList_t &>(&Container::updatedComponent),
            this, qOverload<const OTP::cid_t&, const moduleList_t &>(&Component::updatedComponent));
    connect(otpNetwork.get(), qOverload<const OTP::cid_t&, OTP::component_t::type_t>(&Container::updatedComponent),
            this, qOverload<const OTP::cid_t&, OTP::component_t::type_t>(&Component::updatedComponent));

    // System Signals
    connect(otpNetwork.get(), &Container::newSystem,
        [this](cid_t cid, system_t system) {
            if (cid == getLocalCID()) emit newLocalSystem(system);
        });
    connect(otpNetwork.get(), &Container::removedSystem,
        [this](cid_t cid, system_t system) {
            if (cid == getLocalCID()) emit removedLocalSystem(system);
        });
    connect(otpNetwork.get(), &Container::newSystem, this, &Component::newSystem);
    connect(otpNetwork.get(), &Container::removedSystem, this, &Component::removedSystem);

    // Group Signals
    connect(otpNetwork.get(), &Container::newGroup, this, &Component::newGroup);
    connect(otpNetwork.get(), &Container::removedGroup, this, &Component::removedGroup);

    // Point Signals
    connect(otpNetwork.get(), &Container::newPoint, this, &Component::newPoint);
    connect(otpNetwork.get(), &Container::updatedPoint, this, &Component::updatedPoint);
    connect(otpNetwork.get(), &Container::expiredPoint, this, &Component::expiredPoint);
    connect(otpNetwork.get(), &Container::removedPoint, this, &Component::removedPoint);
}

/* OTP Network */
void Component::ClearOTPMap()
{
    otpNetwork->clearSystems();
    otpNetwork->clearComponents();
}

/* Network Interface */
void Component::setNetworkInterface(QNetworkInterface value)
{
    if (iface.name() == value.name()) return;
    iface = value;
    setupListener();
    emit newNetworkInterface(iface);
}

/* Network Transport */
void Component::setNetworkTransport(QAbstractSocket::NetworkLayerProtocol value)
{
    if (transport == value) return;
    transport = value;
    setupListener();
    emit newNetworkTransport(transport);
}
void Component::setupListener()
{
    qDebug() << parent() << "- Starting on interface" << iface.humanReadableName() << iface.hardwareAddress();

    sockets.clear();

    for (const auto &connection : listenerConnections)
        disconnect(connection);

    if ((transport == QAbstractSocket::IPv4Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
    {
        sockets.insert(QAbstractSocket::IPv4Protocol, SocketManager::getSocket(iface, QAbstractSocket::IPv4Protocol));
        sockets.value(QAbstractSocket::IPv4Protocol).get()->joinMulticastGroup(OTP_Advertisement_Message_IPv4);
        qDebug() << this << "- Listening to Advertisement Messages" << OTP_Advertisement_Message_IPv4;
    }

    if ((transport == QAbstractSocket::IPv6Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
    {
        sockets.insert(QAbstractSocket::IPv6Protocol, SocketManager::getSocket(iface, QAbstractSocket::IPv6Protocol));
        sockets.value(QAbstractSocket::IPv6Protocol).get()->joinMulticastGroup(OTP_Advertisement_Message_IPv6);
        qDebug() << this << "- Listening to Advertisement Messages" << OTP_Advertisement_Message_IPv6;
    }

    for (const auto &socket : sockets) {
        listenerConnections.append(
                    connect(
                        socket.get(), &SocketManager::newDatagram,
                        this, &Component::newDatagram));
        listenerConnections.append(
                    connect(
                        socket.get(), &SocketManager::stateChanged,
                        this, &Component::stateChangedNetworkInterface));
    }
}

/* Local CID */
void Component::setLocalCID(cid_t value)
{
    if (CID == value) return;
    CID = value;
    emit newLocalCID(CID);
}

/* Local Name */
void Component::setLocalName(name_t value)
{
    if (name == value) return;
    name = value;
    emit newLocalName(name);
}

/* Local Systems */
QList<system_t> Component::getLocalSystems() const
{
    return otpNetwork->getSystemList(getLocalCID());
}
void Component::addLocalSystem(system_t system)
{
    if (!system.isValid()) return;

    if (!getLocalSystems().contains(system)) {
        otpNetwork->addSystem(getLocalCID(), system);
        emit newLocalSystem(system);
    }
}
void Component::removeLocalSystem(system_t system)
{
    if (!system.isValid()) return;

    otpNetwork->removeSystem(getLocalCID(), system);
    emit removedLocalSystem(system);
}

/* Groups */
bool Component::isGroupExpired(cid_t cid, system_t system, group_t group) const
{
    QList<point_t> pointList;
    if (cid.isNull())
        pointList = getPoints(system, group);
    else
        pointList = getPoints(cid, system, group);

    for (const auto &point : pointList)
    {
        auto address = address_t{system, group, point};
        if (!isPointExpired(cid, address)) return false;
    }
    return true;
}

/* Points */
QString Component::getPointName(cid_t cid, address_t address) const
{
    if (!isPointValid(cid, address)) return QString();
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return QString();
    if (cid.isNull())
    {
        cid_t newestCid;
        QDateTime lastSeen;
        for (const auto &cid : getComponents())
        {
            auto tempSeen = otpNetwork->PointDetails(cid, address)->getLastSeen();
            if (tempSeen > lastSeen)
            {
                lastSeen = tempSeen;
                newestCid = cid;
            }
        }
        return otpNetwork->PointDetails(newestCid, address)->getName();
    }
    return otpNetwork->PointDetails(cid, address)->getName();
}
bool Component::isPointValid(address_t address) const
{
    if (!address.isValid())
        return false;
    if (!getPoints(address.system, address.group).contains(address.point))
        return false;

    return true;
}
bool Component::isPointValid(cid_t cid, address_t address) const
{
    if (!address.isValid())
        return false;
    if (!getPoints(cid, address.system, address.group).contains(address.point))
        return false;

    return true;
}
QDateTime Component::getPointLastSeen(cid_t cid, address_t address) const
{
    if (cid.isNull())
    {
        QDateTime ret;
        for (const auto &cid : getComponents())
        {
            if (isPointValid(cid, address))
            {
                auto temp = otpNetwork->PointDetails(cid, address)->getLastSeen();
                if (temp > ret) ret = temp;
            }
        }
        return ret;
    }

    if (!isPointValid(cid, address)) return QDateTime();
    return otpNetwork->PointDetails(cid, address)->getLastSeen();
}
bool Component::isPointExpired(cid_t cid, address_t address) const
{
    if (cid.isNull())
    {
        for (const auto &cid : getComponents())
            if (isPointValid(cid, address))
                if (!otpNetwork->PointDetails(cid, address)->isExpired()) return false;
        return true;
    }

    if (!isPointValid(cid, address)) return true;
    return otpNetwork->PointDetails(cid, address)->isExpired();
}

/* Addresses */
QList<address_t> Component::getAddresses()
{
    QList<address_t> ret;
    for (const auto &system : getSystems())
        ret.append(getAddresses(system));

    return ret;
}
QList<address_t> Component::getAddresses(system_t system)
{
    QList<address_t> ret;
    for (const auto &group : getGroups(system))
        ret.append(getAddresses(system, group));

    return ret;
}
QList<address_t> Component::getAddresses(system_t system, group_t group)
{
    QList<address_t> ret;
    for (const auto &point : getPoints(system, group))
        ret.append(address_t(system, group, point));

    return ret;
}

/* Standard Modules */
QString Component::getScaleString(MODULES::STANDARD::PositionModule_t::scale_t scale, bool html) const
{
    return MODULES::STANDARD::VALUES::UNITS::getScaleString(scale, html);
}
QString Component::getUnitString(MODULES::STANDARD::VALUES::moduleValue_t moduleValue, bool html) const
{
    using namespace MODULES::STANDARD::VALUES;
    return QString ("%2")
            .arg(UNITS::getUnitString(moduleValue, html));
}
QString Component::getUnitString(
        MODULES::STANDARD::PositionModule_t::scale_t scale,
        MODULES::STANDARD::VALUES::moduleValue_t moduleValue,
        bool html) const
{
    using namespace MODULES::STANDARD::VALUES;
    return QString ("%1%2")
            .arg(getScaleString(scale, html))
            .arg(UNITS::getUnitString(moduleValue, html));
}
