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
#include "otp.hpp"
#include "container.hpp"
#include "socket.hpp"
#include "network/pdu/pdu_const.hpp"
#include "network/modules/modules.hpp"
#include <QTimer>
#include <QDebug>

using namespace OTP;

Consumer::Consumer(
        QNetworkInterface iface,
        QAbstractSocket::NetworkLayerProtocol transport,
        QList<system_t> systems,
        name_t name,
        cid_t CID,
        QObject *parent) :
    QObject(parent),
    otpNetwork(new Container(this)),
    iface(iface),
    transport(transport),
    CID(CID),
    name(name)
{
    // Signals
    connect(otpNetwork.get(), &Container::newComponent, this, &Consumer::newComponent);
    connect(otpNetwork.get(), &Container::removedComponent, this, &Consumer::removedComponent);

    connect(otpNetwork.get(), qOverload<const cid_t&, const name_t&>(&Container::updatedComponent),
            this, qOverload<const cid_t&, const name_t&>(&Consumer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const OTP::cid_t&, const QHostAddress&>(&Container::updatedComponent),
            this, qOverload<const OTP::cid_t&, const QHostAddress&>(&Consumer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const OTP::cid_t&, const moduleList_t &>(&Container::updatedComponent),
            this, qOverload<const OTP::cid_t&, const moduleList_t &>(&Consumer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const OTP::cid_t&, OTP::component_t::type_t>(&Container::updatedComponent),
            this, qOverload<const OTP::cid_t&, OTP::component_t::type_t>(&Consumer::updatedComponent));

    connect(otpNetwork.get(), &Container::newSystem, this, &Consumer::newSystem);
    connect(otpNetwork.get(), &Container::removedSystem, this, &Consumer::removedSystem);

    connect(otpNetwork.get(), &Container::newGroup, this, &Consumer::newGroup);
    connect(otpNetwork.get(), &Container::removedGroup, this, &Consumer::removedGroup);

    connect(otpNetwork.get(), &Container::newPoint, this, &Consumer::newPoint);
    connect(otpNetwork.get(), &Container::updatedPoint, this, &Consumer::updatedPoint);
    connect(otpNetwork.get(), &Container::expiredPoint, this, &Consumer::expiredPoint);
    connect(otpNetwork.get(), &Container::removedPoint, this, &Consumer::removedPoint);

    // Setup Systems
    for (auto system : systems)
        addConsumerSystem(system);
    setupListener();

    // Module Advertisement Message Timer
    QTimer *moduleAdvertTimer = new QTimer(this);
    moduleAdvertTimer->setInterval(OTP_ADVERTISEMENT_TIMING);
    moduleAdvertTimer->setSingleShot(false);
    connect(moduleAdvertTimer, &QTimer::timeout, [this]() {sendOTPModuleAdvertisementMessage();});
    moduleAdvertTimer->start();
    sendOTPModuleAdvertisementMessage();
};

Consumer::~Consumer()
{}

void Consumer::ClearOTPMap()
{
    otpNetwork->clearSystems();
    otpNetwork->clearComponents();
}

void Consumer::UpdateOTPMap()
{
    sendOTPSystemAdvertisementMessage();
    sendOTPNameAdvertisementMessage();
}

/* Consumer CID */
void Consumer::setConsumerCID(cid_t value)
{
    if (CID == value) return;
    CID = value;
    emit newConsumerCID(getConsumerCID());
}

/* Consumer Name */
void Consumer::setConsumerName(name_t value)
{
    if (name == value) return;
    name = value;
    emit newConsumerName(name);
}

/* Consumer Network Interface */
void Consumer::setConsumerNetworkInterface(QNetworkInterface value)
{
    if (iface.name() == value.name()) return;
    iface = value;
    setupListener();
    emit newConsumerNetworkInterface(iface);
}

/* Consumer Network Transport */
void Consumer::setConsumerNetworkTransport(QAbstractSocket::NetworkLayerProtocol value)
{
    if (transport == value) return;
    transport = value;
    setupListener();
    emit newConsumerNetworkTransport(transport);
}

/* Consumer Systems */
QList<system_t> Consumer::getConsumerSystems() const
{
    return otpNetwork->getSystemList(getConsumerCID());
}

void Consumer::addConsumerSystem(system_t system)
{
    if (system.isValid())
    {
        if (!getConsumerSystems().contains(system)) otpNetwork->addSystem(getConsumerCID(), system);

        if ((transport == QAbstractSocket::IPv4Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
        {
            auto groupAddr = QHostAddress(OTP_Transform_Message_IPv4.toIPv4Address() + system);
            if (SocketManager::getInstance(iface, QAbstractSocket::IPv4Protocol)->joinMulticastGroup(groupAddr))
            {
                emit newConsumerSystem(system);
                qDebug() << this << "- Listening to Transform Messages for System" << system << groupAddr;
            }
        }

        if ((transport == QAbstractSocket::IPv6Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
        {
            auto groupAddr = QHostAddress(OTP_Transform_Message_IPv6.toIPv6Address() + system);
            if (SocketManager::getInstance(iface, QAbstractSocket::IPv6Protocol)->joinMulticastGroup(groupAddr))
            {
                emit newConsumerSystem(system);
                qDebug() << this << "- Listening to Transform Messages for System" << system << groupAddr;
            }
        }
    }
}

void Consumer::removeConsumerSystem(system_t system)
{
    if (system.isValid())
    {
        otpNetwork->removeSystem(getConsumerCID(), system);
        if ((transport == QAbstractSocket::IPv4Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
        {
            auto groupAddr = QHostAddress(OTP_Transform_Message_IPv4.toIPv4Address() + system);
            if (SocketManager::getInstance(iface, QAbstractSocket::IPv4Protocol)->leaveMulticastGroup(groupAddr))
                qDebug() << this << "- Stopping listening to Transform Messages for System" << system << groupAddr;
        }

        if ((transport == QAbstractSocket::IPv6Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
        {
            auto groupAddr = QHostAddress(OTP_Transform_Message_IPv6.toIPv6Address() + system);
            if (SocketManager::getInstance(iface, QAbstractSocket::IPv6Protocol)->leaveMulticastGroup(groupAddr))
                qDebug() << this << "- Stopping listening to Transform Messages for System" << system << groupAddr;
        }

        removedConsumerSystem(system);
    }
}

/* Components */
QList<cid_t> Consumer::getComponents() const
{
    return otpNetwork->getComponentList();
}

component_t Consumer::getComponent(cid_t cid) const
{
    return otpNetwork->getComponent(cid);
}


/* Systems */
QList<system_t> Consumer::getSystems() const
{
    return otpNetwork->getSystemList();
}
QList<system_t> Consumer::getSystems(cid_t cid) const
{
    return otpNetwork->getSystemList(cid);
}

/* Groups */
QList<group_t> Consumer::getGroups(system_t system) const
{
    return otpNetwork->getGroupList(system);
}
QList<group_t> Consumer::getGroups(cid_t cid, system_t system) const
{
    return otpNetwork->getGroupList(cid, system);
}

bool Consumer::isGroupExpired(cid_t cid, system_t system, group_t group) const
{
    QList<point_t> pointList;
    if (cid.isNull())
        pointList = getPoints(system, group);
    else
        pointList = getPoints(cid, system, group);

    for (auto point : pointList)
    {
        auto address = address_t{system, group, point};
        if (!isPointExpired(cid, address)) return false;
    }
    return true;
}

/* Points */
bool Consumer::isPointValid(address_t address) const
{
    if (!address.isValid())
        return false;
    if (!getPoints(address.system, address.group).contains(address.point))
        return false;

    return true;
}

bool Consumer::isPointValid(cid_t cid, address_t address) const
{
    if (!address.isValid())
        return false;
    if (!getPoints(cid, address.system, address.group).contains(address.point))
        return false;

    return true;
}

QList<point_t> Consumer::getPoints(system_t system, group_t group) const
{
    return otpNetwork->getPointList(system, group);
}

QList<point_t> Consumer::getPoints(cid_t cid, system_t system, group_t group) const
{
    return otpNetwork->getPointList(cid, system, group);
}

QString Consumer::getPointName(cid_t cid, address_t address) const
{
    if (cid.isNull())
    {
        cid_t newestCid;
        QDateTime lastSeen;
        for (auto cid : getComponents())
        {
            if (isPointValid(cid, address))
            {
                auto tempSeen = otpNetwork->PointDetails(cid, address)->getLastSeen();
                if (tempSeen > lastSeen)
                {
                    lastSeen = tempSeen;
                    newestCid = cid;
                }
            }
        }
        return otpNetwork->PointDetails(newestCid, address)->getName();
    }

    if (!isPointValid(cid, address)) return QString();
    return otpNetwork->PointDetails(cid, address)->getName();
}

QDateTime Consumer::getPointLastSeen(cid_t cid, address_t address) const
{
    if (cid.isNull())
    {
        QDateTime ret;
        for (auto cid : getComponents())
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

bool Consumer::isPointExpired(cid_t cid, address_t address) const
{
    if (cid.isNull())
    {
        for (auto cid : getComponents())
            if (isPointValid(cid, address))
                if (!otpNetwork->PointDetails(cid, address)->isExpired()) return false;
        return true;
    }

    if (!isPointValid(cid, address)) return true;
    return otpNetwork->PointDetails(cid, address)->isExpired();
}

/* Addresses */
QList<address_t> Consumer::getAddresses()
{
    QList<address_t> ret;
    for (auto system : getSystems())
        ret.append(getAddresses(system));

    return ret;
}
QList<address_t> Consumer::getAddresses(system_t system)
{
    QList<address_t> ret;
    for (auto group : getGroups(system))
        ret.append(getAddresses(system, group));

    return ret;
}

QList<address_t> Consumer::getAddresses(system_t system, group_t group)
{
    QList<address_t> ret;
    for (auto point : getPoints(system, group))
        ret.append(address_t(system, group, point));

    return ret;
}

/* Standard Modules */
QString Consumer::getScaleString(MODULES::STANDARD::PositionModule_t::scale_t scale, bool html) const
{
    return MODULES::STANDARD::VALUES::UNITS::getScaleString(scale, html);
}

QString Consumer::getUnitString(MODULES::STANDARD::VALUES::moduleValue_t moduleValue, bool html) const
{
    using namespace MODULES::STANDARD::VALUES;
    return QString ("%2")
            .arg(UNITS::getUnitString(moduleValue, html));
}

QString Consumer::getUnitString(
        MODULES::STANDARD::PositionModule_t::scale_t scale,
        MODULES::STANDARD::VALUES::moduleValue_t moduleValue,
        bool html) const
{
    using namespace MODULES::STANDARD::VALUES;
    return QString ("%1%2")
            .arg(getScaleString(scale, html))
            .arg(UNITS::getUnitString(moduleValue, html));
}

/* Standard Modules - Position */
Consumer::PositionValue_t Consumer::getPosition(cid_t cid, address_t address, axis_t axis, bool respectRelative) const
{
    using namespace MODULES::STANDARD;
    Consumer::PositionValue_t ret;
    if (!isPointValid(cid, address))
        return ret;

    auto module = otpNetwork->PointDetails(cid, address)->standardModules.position;
    if (respectRelative) {
        QList<address_t> previous({address});
        auto referenceFrame = getReferenceFrame(cid, address);
        while (isPointValid(referenceFrame.value) &&
               !previous.contains(referenceFrame.value))
        {
            module += otpNetwork->PointDetails(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value)->standardModules.position;
            previous << referenceFrame.value;
            referenceFrame = getReferenceFrame(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value);
        }
    }
    ret.value = module.getPosition(axis);
    ret.scale = module.getScaling();
    ret.unit = getUnitString(ret.scale, VALUES::POSITION);
    ret.timestamp = module.getTimestamp();
    ret.sourceCID = cid;
    ret.priority = otpNetwork->PointDetails(cid, address)->getPriority();
    return ret;
}

Consumer::PositionValue_t Consumer::getPosition(address_t address, axis_t axis, bool respectRelative) const
{
    auto cid = otpNetwork->getWinningComponent(address);
    return getPosition(cid, address, axis, respectRelative);
}

/* Standard Modules - Position Velocity/Acceleration */
Consumer::PositionVelocity_t Consumer::getPositionVelocity(cid_t cid, address_t address, axis_t axis, bool respectRelative) const
{
    using namespace MODULES::STANDARD;
    Consumer::PositionVelocity_t ret;
    if (!isPointValid(cid, address))
        return ret;

    auto module = otpNetwork->PointDetails(cid, address)->standardModules.positionVelAcc;
    if (respectRelative) {
        QList<address_t> previous({address});
        auto referenceFrame = getReferenceFrame(cid, address);
        while (isPointValid(referenceFrame.value) &&
               !previous.contains(referenceFrame.value))
        {
            module += otpNetwork->PointDetails(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value)->standardModules.positionVelAcc;
            previous << referenceFrame.value;
            referenceFrame = getReferenceFrame(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value);
        }
    }
    ret.value = module.getVelocity(axis);
    ret.unit = getUnitString(VALUES::POSITION_VELOCITY);
    ret.timestamp = module.getTimestamp();
    ret.sourceCID = cid;
    ret.priority = otpNetwork->PointDetails(cid, address)->getPriority();
    return ret;
}

Consumer::PositionVelocity_t Consumer::getPositionVelocity(address_t address, axis_t axis, bool respectRelative) const
{
    auto cid = otpNetwork->getWinningComponent(address);
    return getPositionVelocity(cid, address, axis, respectRelative);
}

Consumer::PositionAcceleration_t Consumer::getPositionAcceleration(cid_t cid, address_t address, axis_t axis, bool respectRelative) const
{
    using namespace MODULES::STANDARD;
    Consumer::PositionAcceleration_t ret;
    if (!isPointValid(cid, address))
        return ret;

    auto module = otpNetwork->PointDetails(cid, address)->standardModules.positionVelAcc;
    if (respectRelative) {
        QList<address_t> previous({address});
        auto referenceFrame = getReferenceFrame(cid, address);
        while (isPointValid(referenceFrame.value) &&
               !previous.contains(referenceFrame.value))
        {
            module += otpNetwork->PointDetails(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value)->standardModules.positionVelAcc;
            previous << referenceFrame.value;
            referenceFrame = getReferenceFrame(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value);
        }
    }
    ret.value = module.getAcceleration(axis);
    ret.unit = getUnitString(VALUES::POSITION_ACCELERATION);
    ret.timestamp = module.getTimestamp();
    ret.sourceCID = cid;
    ret.priority = otpNetwork->PointDetails(cid, address)->getPriority();
    return ret;
}

Consumer::PositionAcceleration_t Consumer::getPositionAcceleration(address_t address, axis_t axis, bool respectRelative) const
{
    auto cid = otpNetwork->getWinningComponent(address);
    return getPositionAcceleration(cid, address, axis, respectRelative);
}


/* Standard Modules - Rotation */
Consumer::RotationValue_t Consumer::getRotation(cid_t cid, address_t address, axis_t axis, bool respectRelative) const
{
    using namespace MODULES::STANDARD;
    Consumer::RotationValue_t ret;
    if (!isPointValid(cid, address))
        return ret;

    auto module = otpNetwork->PointDetails(cid, address)->standardModules.rotation;
    if (respectRelative) {
        QList<address_t> previous({address});
        auto referenceFrame = getReferenceFrame(cid, address);
        while (isPointValid(referenceFrame.value) &&
               !previous.contains(referenceFrame.value))
        {
            module += otpNetwork->PointDetails(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value)->standardModules.rotation;
            previous << referenceFrame.value;
            referenceFrame = getReferenceFrame(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value);
        }
    }
    ret.value = module.getRotation(axis);
    ret.unit = getUnitString(VALUES::ROTATION);
    ret.timestamp = module.getTimestamp();
    ret.sourceCID = cid;
    ret.priority = otpNetwork->PointDetails(cid, address)->getPriority();
    return ret;
}

Consumer::RotationValue_t Consumer::getRotation(address_t address, axis_t axis, bool respectRelative) const
{
    auto cid = otpNetwork->getWinningComponent(address);
    return getRotation(cid, address, axis, respectRelative);
}


/* Standard Modules - Position Velocity/Acceleration */
Consumer::RotationVelocity_t Consumer::getRotationVelocity(cid_t cid, address_t address, axis_t axis, bool respectRelative) const
{
    using namespace MODULES::STANDARD;
    Consumer::RotationVelocity_t ret;
    if (!isPointValid(cid, address))
        return ret;

    auto module = otpNetwork->PointDetails(cid, address)->standardModules.rotationVelAcc;
    if (respectRelative) {
        QList<address_t> previous({address});
        auto referenceFrame = getReferenceFrame(cid, address);
        while (isPointValid(referenceFrame.value) &&
               !previous.contains(referenceFrame.value))
        {
            module += otpNetwork->PointDetails(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value)->standardModules.rotationVelAcc;
            previous << referenceFrame.value;
            referenceFrame = getReferenceFrame(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value);
        }
    }
    ret.value = module.getVelocity(axis);
    ret.unit = getUnitString(VALUES::ROTATION_VELOCITY);
    ret.timestamp = module.getTimestamp();
    ret.sourceCID = cid;
    ret.priority = otpNetwork->PointDetails(cid, address)->getPriority();
    return ret;
}

Consumer::RotationVelocity_t Consumer::getRotationVelocity(address_t address, axis_t axis, bool respectRelative) const
{
    auto cid = otpNetwork->getWinningComponent(address);
    return getRotationVelocity(cid, address, axis, respectRelative);
}

Consumer::RotationAcceleration_t Consumer::getRotationAcceleration(cid_t cid, address_t address, axis_t axis, bool respectRelative) const
{
    using namespace MODULES::STANDARD;
    Consumer::RotationAcceleration_t ret;
    if (!isPointValid(cid, address))
        return ret;

    auto module = otpNetwork->PointDetails(cid, address)->standardModules.rotationVelAcc;
    if (respectRelative) {
        QList<address_t> previous({address});
        auto referenceFrame = getReferenceFrame(cid, address);
        while (isPointValid(referenceFrame.value) &&
               !previous.contains(referenceFrame.value))
        {
            module += otpNetwork->PointDetails(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value)->standardModules.rotationVelAcc;
            previous << referenceFrame.value;
            referenceFrame = getReferenceFrame(otpNetwork->getWinningComponent(referenceFrame.value), referenceFrame.value);
        }
    }
    ret.value = module.getAcceleration(axis);
    ret.unit = getUnitString(VALUES::ROTATION_ACCELERATION);
    ret.timestamp = module.getTimestamp();
    ret.sourceCID = cid;
    ret.priority = otpNetwork->PointDetails(cid, address)->getPriority();
    return ret;
}

Consumer::RotationAcceleration_t Consumer::getRotationAcceleration(address_t address, axis_t axis, bool respectRelative) const
{
    auto cid = otpNetwork->getWinningComponent(address);
    return getRotationAcceleration(cid, address, axis, respectRelative);
}

Consumer::Scale_t Consumer::getScale(cid_t cid, address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Consumer::Scale_t ret;
    if (!isPointValid(cid, address))
        return ret;

    ret.value = otpNetwork->PointDetails(cid, address)->standardModules.scale.getScale(axis);
    ret.timestamp = otpNetwork->PointDetails(cid, address)->standardModules.scale.getTimestamp();
    ret.sourceCID = cid;
    ret.priority = otpNetwork->PointDetails(cid, address)->getPriority();
    return ret;
}

Consumer::Scale_t Consumer::getScale(address_t address, axis_t axis) const
{
    auto cid = otpNetwork->getWinningComponent(address);
    return getScale(cid, address, axis);
}

Consumer::ReferenceFrame_t Consumer::getReferenceFrame(cid_t cid, address_t address) const
{
    using namespace MODULES::STANDARD;
    Consumer::ReferenceFrame_t ret;
    if (!isPointValid(cid, address))
        return ret;

    auto module = otpNetwork->PointDetails(cid, address)->standardModules.referenceFrame;
    ret.value = {module.getSystem(), module.getGroup(), module.getPoint()};
    ret.timestamp = module.getTimestamp();
    ret.sourceCID = cid;
    ret.priority = otpNetwork->PointDetails(cid, address)->getPriority();
    return ret;
}

Consumer::ReferenceFrame_t Consumer::getReferenceFrame(address_t address) const
{
    auto cid = otpNetwork->getWinningComponent(address);
    return getReferenceFrame(cid, address);
}

void Consumer::setupListener()
{
    qDebug() << this << "- Starting on interface" << iface.humanReadableName() << iface.hardwareAddress();

    for (auto connection : listenerConnections)
        disconnect(connection);

    if ((transport == QAbstractSocket::IPv4Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
    {
        listenerConnections.append(connect(SocketManager::getInstance(iface, QAbstractSocket::IPv4Protocol).get(), &SocketManager::newDatagram,
                this, &Consumer::newDatagram));
        SocketManager::getInstance(iface, QAbstractSocket::IPv4Protocol)->joinMulticastGroup(OTP_Advertisement_Message_IPv4);
        qDebug() << this << "- Listening to Advertisement Messages" << OTP_Advertisement_Message_IPv4;
    }

    if ((transport == QAbstractSocket::IPv6Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
    {
        listenerConnections.append(connect(SocketManager::getInstance(iface, QAbstractSocket::IPv6Protocol).get(), &SocketManager::newDatagram,
                this, &Consumer::newDatagram));
        SocketManager::getInstance(iface, QAbstractSocket::IPv6Protocol)->joinMulticastGroup(OTP_Advertisement_Message_IPv6);
        qDebug() << this << "- Listening to Advertisement Messages" << OTP_Advertisement_Message_IPv6;
    }

    for (auto system : getConsumerSystems())
        addConsumerSystem(system);
}

void Consumer::newDatagram(QNetworkDatagram datagram)
{
    /* Unicast packets to self have no senderAddress */
    if (datagram.senderAddress().isNull()) datagram.setSender(datagram.destinationAddress());

    // Transform Messages
    if (receiveOTPTransformMessage(datagram)) return;

    // Advertisement Messages
    if (receiveOTPNameAdvertisementMessage(datagram)) return;
    if (receiveOTPSystemAdvertisementMessage(datagram)) return;
}

bool Consumer::receiveOTPTransformMessage(QNetworkDatagram datagram)
{
    if (
        ((datagram.destinationAddress().toIPv4Address() >= OTP_Transform_Message_IPv4.toIPv4Address()) &&
            (datagram.destinationAddress().toIPv4Address() <= OTP_Transform_Message_IPv4.toIPv4Address()
             + RANGES::System.getMax())) ||
        ((datagram.destinationAddress().toIPv6Address() >= OTP_Transform_Message_IPv6.toIPv6Address()) &&
            (datagram.destinationAddress().toIPv6Address() <= OTP_Transform_Message_IPv6.toIPv6Address()
             + static_cast<system_t>(RANGES::System.getMax()))))
    {
        MESSAGES::OTPTransformMessage::Message transformMessage(datagram);
        if (transformMessage.isValid())
        {
            auto cid = transformMessage.getOTPLayer()->getCID();
            auto folio = transformMessage.getOTPLayer()->getFolio();
            auto system = transformMessage.getTransformLayer()->getSystem();
            if (!folioMap.checkSequence(
                    cid,
                    system,
                    PDU::VECTOR_OTP_TRANSFORM_MESSAGE,
                    folio))
            {
                qDebug() << this << "- Out of Sequence OTP Transform Message Request Received From" << datagram.senderAddress();
                return true;
            }

            otpNetwork->addComponent(
                    cid,
                    datagram.senderAddress(),
                    transformMessage.getOTPLayer()->getComponentName(),
                    component_t::type_t::produder);

            // Add page to folio map
            folioMap.addPage(
                        cid,
                        system,
                        PDU::VECTOR_OTP_TRANSFORM_MESSAGE,
                        folio,
                        transformMessage.getOTPLayer()->getPage(),
                        datagram);

            // Last page?
            if (folioMap.checkAllPages(
                        cid,
                        system,
                        PDU::VECTOR_OTP_TRANSFORM_MESSAGE,
                        folio,
                        transformMessage.getOTPLayer()->getLastPage()))
            {
                // Process all pages
                for (auto datagram : folioMap.getDatagrams(cid,
                            system,
                            PDU::VECTOR_OTP_TRANSFORM_MESSAGE,
                            folio))
                {
                    // Process each Point layer
                    for (auto pointLayer : transformMessage.getPointLayers())
                    {
                        auto address = address_t{
                            transformMessage.getTransformLayer()->getSystem(),
                            pointLayer->getGroup(),
                            pointLayer->getPoint()};
                        auto timestamp = pointLayer->getTimestamp();
                        otpNetwork->addPoint(cid, address, pointLayer->getPriority());
                        otpNetwork->PointDetails(cid, address)->setPriority(pointLayer->getPriority());

                        pointDetails::standardModules_t newStandardModules;
                        for (auto moduleLayer : transformMessage.getModuleLayers().values(address))
                        {
                            switch (moduleLayer->getManufacturerID())
                            {
                                case ESTA_MANUFACTURER_ID:
                                {
                                    switch (moduleLayer->getModuleNumber()) {
                                        case MODULES::STANDARD::POSITION:
                                            newStandardModules.position = MODULES::STANDARD::PositionModule_t(moduleLayer->getAdditional(), timestamp);
                                            break;

                                        case MODULES::STANDARD::POSITION_VELOCITY_ACCELERATION:
                                            newStandardModules.positionVelAcc = MODULES::STANDARD::PositionVelAccModule_t(moduleLayer->getAdditional(), timestamp);
                                            break;

                                        case MODULES::STANDARD::ROTATION:
                                            newStandardModules.rotation = MODULES::STANDARD::RotationModule_t(moduleLayer->getAdditional(), timestamp);
                                            break;

                                        case MODULES::STANDARD::ROTATION_VELOCITY_ACCELERATION:
                                            newStandardModules.rotationVelAcc = MODULES::STANDARD::RotationVelAccModule_t(moduleLayer->getAdditional(), timestamp);
                                            break;

                                        case MODULES::STANDARD::SCALE:
                                            newStandardModules.scale = MODULES::STANDARD::ScaleModule_t(moduleLayer->getAdditional(), timestamp);
                                            break;

                                        case MODULES::STANDARD::REFERENCE_FRAME:
                                            newStandardModules.referenceFrame = MODULES::STANDARD::ReferenceFrameModule_t(moduleLayer->getAdditional(), timestamp);
                                            break;

                                        default:
                                        {
                                        qDebug() << this << "Unknown module ID"
                                                 << moduleLayer->getManufacturerID() << moduleLayer->getModuleNumber()
                                                 << "From" << datagram.senderAddress();
                                        } break;
                                    }

                                } break;
                                default:
                                {
                                    qDebug() << this << "Unknown module Manufacturer ID"
                                             << moduleLayer->getManufacturerID()
                                             << "From" << datagram.senderAddress();
                                } break;
                            }
                        }

                        // Update standard module details
                        auto const oldStandardModules = otpNetwork->PointDetails(cid, address)->standardModules;
                        for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                        {
                            // - MODULES::STANDARD::POSITION
                            otpNetwork->PointDetails(cid, address)->standardModules.position = newStandardModules.position;
                            if (oldStandardModules.position.getPosition(axis) != newStandardModules.position.getPosition(axis))
                                emit updatedPosition(cid, address, axis);

                            // - MODULES::STANDARD::POSITION_VELOCITY_ACCELERATION
                            otpNetwork->PointDetails(cid, address)->standardModules.positionVelAcc = newStandardModules.positionVelAcc;
                            if (oldStandardModules.positionVelAcc.getVelocity(axis) != newStandardModules.positionVelAcc.getVelocity(axis))
                                emit updatedPositionVelocity(cid, address, axis);
                            if (oldStandardModules.positionVelAcc.getAcceleration(axis) != newStandardModules.positionVelAcc.getAcceleration(axis))
                                emit updatedPositionAcceleration(cid, address, axis);

                            // - MODULES::STANDARD::ROTATION
                            otpNetwork->PointDetails(cid, address)->standardModules.rotation = newStandardModules.rotation;
                            if (oldStandardModules.rotation.getRotation(axis) != newStandardModules.rotation.getRotation(axis))
                                emit updatedRotation(cid, address, axis);

                            // - MODULES::STANDARD::ROTATION_VELOCITY_ACCELERATION
                            otpNetwork->PointDetails(cid, address)->standardModules.rotationVelAcc = newStandardModules.rotationVelAcc;
                            if (oldStandardModules.rotationVelAcc.getVelocity(axis) != newStandardModules.rotationVelAcc.getVelocity(axis))
                                emit updatedRotationVelocity(cid, address, axis);
                            if (oldStandardModules.rotationVelAcc.getAcceleration(axis) != newStandardModules.rotationVelAcc.getAcceleration(axis))
                                emit updatedRotationAcceleration(cid, address, axis);

                            // - MODULES::STANDARD::SCALE
                            otpNetwork->PointDetails(cid, address)->standardModules.scale = newStandardModules.scale;
                            if (oldStandardModules.scale.getScale(axis) != newStandardModules.scale.getScale(axis))
                                emit updatedScale(cid, address, axis);

                            // - MODULES::STANDARD::REFERENCE_FRAME
                            otpNetwork->PointDetails(cid, address)->standardModules.referenceFrame = newStandardModules.referenceFrame;
                            if (oldStandardModules.referenceFrame != newStandardModules.referenceFrame)
                                    emit updatedReferenceFrame(cid, address);
                        }
                    }
                }
            }
        }
        return true;
    }
    return false;
}

bool Consumer::receiveOTPNameAdvertisementMessage(QNetworkDatagram datagram)
{
    MESSAGES::OTPNameAdvertisementMessage::Message nameAdvert(datagram);

    // Name Advertisement Message?
    if (nameAdvert.isValid())
    {
        auto cid = nameAdvert.getOTPLayer()->getCID();
        auto folio = nameAdvert.getOTPLayer()->getFolio();
        if (cid != getConsumerCID() && !folioMap.checkSequence(
                    cid,
                    PDU::VECTOR_OTP_ADVERTISEMENT_NAME,
                    folio))
        {
            qDebug() << this << "- Out of Sequence OTP Name Advertisement Message Request Received From" << datagram.senderAddress();
            return true;
        }

        auto type = (nameAdvert.getNameAdvertisementLayer()->getOptions().isResponse()) ? component_t::type_t::produder : component_t::type_t::consumer;
        if (type == component_t::type_t::produder)
            qDebug() << this << "- OTP Name Advertisement Message Response Received From" << datagram.senderAddress();
        else if (type == component_t::type_t::consumer)
            qDebug() << this << "- OTP Name Advertisement Message Request Received From" << datagram.senderAddress();

        otpNetwork->addComponent(
                cid,
                datagram.senderAddress(),
                nameAdvert.getOTPLayer()->getComponentName(),
                type);

        // Add page to folio map
        folioMap.addPage(
                    cid,
                    PDU::VECTOR_OTP_ADVERTISEMENT_NAME,
                    folio,
                    nameAdvert.getOTPLayer()->getPage(),
                    datagram);

        // Last page?
        if (folioMap.checkAllPages(
                    cid,
                    PDU::VECTOR_OTP_ADVERTISEMENT_NAME,
                    folio,
                    nameAdvert.getOTPLayer()->getLastPage()))
        {
            // Process all pages
            MESSAGES::OTPNameAdvertisementMessage::list_t list;
            for (auto datagram : folioMap.getDatagrams(cid,
                        PDU::VECTOR_OTP_ADVERTISEMENT_NAME,
                        folio))
            {
                MESSAGES::OTPNameAdvertisementMessage::Message nameAdvert(datagram);
                list.append(nameAdvert.getNameAdvertisementLayer()->getList());
            }

            // Add names
            for (auto point : list)
            {
                address_t address = address_t(point.System, point.Group, point.Point);
                otpNetwork->addPoint(cid, address);
                otpNetwork->PointDetails(cid, address)->setName(point.PointName);
            }
        }
        return true;
    }
    return false;
}

bool Consumer::receiveOTPSystemAdvertisementMessage(QNetworkDatagram datagram)
{
    MESSAGES::OTPSystemAdvertisementMessage::Message systemAdvert(datagram);

    // System Advertisement Message?
    if (systemAdvert.isValid())
    {
        auto cid = systemAdvert.getOTPLayer()->getCID();
        auto folio = systemAdvert.getOTPLayer()->getFolio();
        if (cid != getConsumerCID() && !folioMap.checkSequence(
                    cid,
                    PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM,
                    folio))
        {
            qDebug() << this << "- Out of Sequence OTP Name Advertisement Message Request Received From" << datagram.senderAddress();
            return true;
        }

        auto type = (systemAdvert.getSystemAdvertisementLayer()->getOptions().isResponse()) ? component_t::type_t::produder : component_t::type_t::consumer;
        if (type == component_t::type_t::produder)
            qDebug() << this << "- OTP System Advertisement Message Response Received From" << datagram.senderAddress();
        else if (type == component_t::type_t::consumer)
            qDebug() << this << "- OTP System Advertisement Message Request Received From" << datagram.senderAddress();

        otpNetwork->addComponent(
                cid,
                datagram.senderAddress(),
                systemAdvert.getOTPLayer()->getComponentName(),
                type);

        // Add page to folio map
        folioMap.addPage(
                    cid,
                    PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM,
                    folio,
                    systemAdvert.getOTPLayer()->getPage(),
                    datagram);

        // Last page?
        if (folioMap.checkAllPages(
                    cid,
                    PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM,
                    folio,
                    systemAdvert.getOTPLayer()->getLastPage()))
        {
            // Process all pages
            MESSAGES::OTPSystemAdvertisementMessage::list_t list;
            for (auto datagram : folioMap.getDatagrams(cid,
                        PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM,
                        folio))
            {
                MESSAGES::OTPSystemAdvertisementMessage::Message systemAdvert(datagram);
                list.append(systemAdvert.getSystemAdvertisementLayer()->getList());
            }

            // Add new systems
            for (auto system : list)
            {
                otpNetwork->addSystem(
                        cid,
                        system);
            }

            // Prune old
            for (auto system : otpNetwork->getSystemList(cid))
            {
                if (!list.contains(system))
                    otpNetwork->removeSystem(
                            cid,
                            system);
            }
        }
        return true;
    }
    return false;
}

void Consumer::sendOTPModuleAdvertisementMessage()
{
    using namespace OTP::MESSAGES::OTPModuleAdvertisementMessage;

    // Get list of supported modules
    list_t list = OTP::MODULES::getSupportedModules();

    // Generate messages
    QVector<std::shared_ptr<Message>> folioMessages;
    while (list.count()) {
        folioMessages.append(
                    std::make_shared<Message>(
                        mode_e::Consumer,
                        getConsumerCID(),
                        name,
                        list_t(),
                        this));

        bool result = true;
        while (result && list.count())
        {
            result = folioMessages.back()->addItem(list.front());
            if (result) list.removeFirst();
        }

        if (!folioMessages.back()->isValid())
        {
            qDebug() << this << "- OTP Module Advertisement Message Request Not Valid";
            return;
        }
    }

    // Send messages
    page_t lastPage = static_cast<page_t>(folioMessages.count()) - 1;
    for (page_t page = 0; page < folioMessages.count(); page++)
    {
        auto datagrams = folioMessages[page]->toQNetworkDatagrams(
                    transport,
                    ModuleAdvertisementMessage_Folio++,
                    page,
                    lastPage);
        if (SocketManager::writeDatagrams(iface, datagrams))
            qDebug() << this << "- OTP Module Advertisement Message Request Sent";
        else
            qDebug() << this << "- OTP Module Advertisement Message Request Failed";
    }
}

void Consumer::sendOTPNameAdvertisementMessage()
{
    // Name Advertisement Message
    using namespace OTP::MESSAGES::OTPNameAdvertisementMessage;
    Message nameAdvertisementMessage(
                mode_e::Consumer,
                getConsumerCID(),
                name,
                list_t(),
                this);

    if (!nameAdvertisementMessage.isValid())
    {
        qDebug() << this << "- OTP Name Advertisement Message Request Not Valid";
        return;
    }

    auto datagrams = nameAdvertisementMessage.toQNetworkDatagrams(
                transport,
                NameAdvertisementMessage_Folio++,
                0,0);
    if (SocketManager::writeDatagrams(iface, datagrams))
        qDebug() << this << "- OTP Name Advertisement Message Request Sent";
    else
        qDebug() << this << "- OTP Name Advertisement Message Request Failed";
}

void Consumer::sendOTPSystemAdvertisementMessage()
{
    // System Advertisement Message
    using namespace OTP::MESSAGES::OTPSystemAdvertisementMessage;
    Message systemAdvertisementMessage(
                mode_e::Consumer,
                getConsumerCID(),
                name,
                list_t(),
                this);

    if (!systemAdvertisementMessage.isValid())
    {
        qDebug() << this << "- OTP System Advertisement Message Request Not Valid";
        return;
    }

    auto datagrams = systemAdvertisementMessage.toQNetworkDatagrams(
                transport,
                SystemAdvertisementMessage_Folio++,
                0,0);
    if (SocketManager::writeDatagrams(iface, datagrams))
        qDebug() << this << "- OTP System Advertisement Message Request Sent";
    else
        qDebug() << this << "- OTP System Advertisement Message Request Failed";
}
