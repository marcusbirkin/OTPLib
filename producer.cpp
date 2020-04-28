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
#include "network/modules/modules.hpp"
#include "network/messages/otp_transform_message.hpp"
#include <QTimer>
#include <random>

#include <QDebug>

using namespace OTP;

Producer::Producer(
        QNetworkInterface iface,
        QAbstractSocket::NetworkLayerProtocol transport,
        name_t name,
        cid_t CID,
        std::chrono::milliseconds transformRate,
        QObject *parent) :
    QObject(parent),
    otpNetwork(new Container(this)),
    iface(iface),
    transport(transport),
    CID(CID),
    name(name)

{
    // Signals
    connect(otpNetwork.get(), &Container::newSystem,
        [this](cid_t cid, system_t system) {
            if (cid == getProducerCID()) emit newProducerSystem(system);
        });
    connect(otpNetwork.get(), &Container::removedSystem,
        [this](cid_t cid, system_t system) {
            if (cid == getProducerCID()) emit removedProducerSystem(system);
        });

    connect(otpNetwork.get(), &Container::newGroup,
        [this](cid_t cid, system_t system, group_t group) {
            if (cid == getProducerCID()) emit newProducerGroup(system, group);
        });
    connect(otpNetwork.get(), &Container::removedGroup,
        [this](cid_t cid, system_t system, group_t group) {
            if (cid == getProducerCID()) emit removedProducerGroup(system, group);
        });

    connect(otpNetwork.get(), &Container::newPoint,
        [this](cid_t cid, system_t system, group_t group, point_t point) {
            if (cid == getProducerCID()) emit newProducerPoint(system, group, point);
        });
    connect(otpNetwork.get(), &Container::removedPoint,
        [this](cid_t cid, system_t system, group_t group, point_t point) {
            if (cid == getProducerCID()) emit removedProducerPoint(system, group, point);
        });

    connect(otpNetwork.get(), &Container::newComponent, this, &Producer::newComponent);
    connect(otpNetwork.get(), &Container::removedComponent, this, &Producer::removedComponent);

    connect(otpNetwork.get(), qOverload<const cid_t&, const name_t&>(&Container::updatedComponent),
            this, qOverload<const cid_t&, const name_t&>(&Producer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const OTP::cid_t&, const QHostAddress&>(&Container::updatedComponent),
            this, qOverload<const OTP::cid_t&, const QHostAddress&>(&Producer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const OTP::cid_t&, const moduleList_t &>(&Container::updatedComponent),
            this, qOverload<const OTP::cid_t&, const moduleList_t &>(&Producer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const OTP::cid_t&, OTP::component_t::type_t>(&Container::updatedComponent),
            this, qOverload<const OTP::cid_t&, OTP::component_t::type_t>(&Producer::updatedComponent));

    connect(otpNetwork.get(), &Container::newSystem, this, &Producer::newSystem);
    connect(otpNetwork.get(), &Container::removedSystem, this, &Producer::removedSystem);

    connect(otpNetwork.get(), &Container::newGroup, this, &Producer::newGroup);
    connect(otpNetwork.get(), &Container::removedGroup, this, &Producer::removedGroup);

    connect(otpNetwork.get(), &Container::newPoint, this, &Producer::newPoint);
    connect(otpNetwork.get(), &Container::removedPoint, this, &Producer::removedPoint);

    setupListener();
    auto startSenderTimeout = new QTimer;
    startSenderTimeout->setSingleShot(true);
    connect(startSenderTimeout, &QTimer::timeout, this, [this, transformRate]() {this->setupSender(transformRate); });
    connect(startSenderTimeout, SIGNAL(timeout()), startSenderTimeout, SLOT(deleteLater()));
    startSenderTimeout->start(OTP_ADVERTISEMENT_STARTUP_WAIT);
}

Producer::~Producer()
{}

void Producer::ClearOTPMap()
{
    otpNetwork->clearSystems();
    otpNetwork->clearComponents();
}

/* Producer CID */
void Producer::setProducerCID(cid_t value)
{
    if (CID == value) return;
    CID = value;
    emit newProducerCID(CID);
}

/* Producer Name */
void Producer::setProducerName(name_t value)
{
    if (name == value) return;
    name = value;
    emit newProducerName(name);
}

/* Producer Network Interface */
void Producer::setProducerNetworkInterface(QNetworkInterface value)
{
    if (iface.name() == value.name()) return;
    iface = value;
    setupListener();
    emit newProducerNetworkInterface(iface);
}

/* Producer Network Transport */
void Producer::setProducerNetworkTransport(QAbstractSocket::NetworkLayerProtocol value)
{
    if (transport == value) return;
    transport = value;
    setupListener();
    emit newProducerNetworkTransport(transport);
}

/* Producer Systems */
QList<system_t> Producer::getProducerSystems() const
{
    return otpNetwork->getSystemList(getProducerCID());
}

void Producer::addProducerSystem(system_t system)
{
    otpNetwork->addSystem(getProducerCID(), system);
}

void Producer::removeProducerSystem(system_t system)
{
    otpNetwork->removeSystem(getProducerCID(), system);
}

/* Producer Groups */
QList<group_t> Producer::getProducerGroups(system_t system) const
{
    return otpNetwork->getGroupList(getProducerCID(), system);
}

void Producer::addProducerGroup(system_t system, group_t group)
{
    otpNetwork->addGroup(getProducerCID(), system, group);
}

void Producer::removeProducerGroup(system_t system, group_t group)
{
    otpNetwork->removeGroup(getProducerCID(), system, group);
}

/* Producer Points */
QList<point_t> Producer::getProducerPoints(system_t system, group_t group) const
{
    return otpNetwork->getPointList(CID, system, group);
}

void Producer::addProducerPoint(address_t address, priority_t priority)
{
    otpNetwork->addPoint(getProducerCID(), address, priority);
    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.parent.setSystem(address.system, 0);
    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.parent.setGroup(address.group, 0);
    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.parent.setPoint(address.point, 0);
}

void Producer::removeProducerPoint(address_t address)
{
    otpNetwork->removePoint(getProducerCID(), address);
}

QString Producer::getProducerPointName(address_t address) const
{
    if (!getProducerPoints(address.system, address.group).contains(address.point)) return QString();
    return otpNetwork->PointDetails(getProducerCID(), address)->getName();
}

void Producer::setProducerPointName(address_t address, QString name)
{
    if (!getProducerPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(getProducerCID(), address)->setName(name);
    emit newProducerPointName(name);
}

priority_t Producer::getProducerPointPriority(address_t address) const
{
    if (!getProducerPoints(address.system, address.group).contains(address.point)) return priority_t();
    return otpNetwork->PointDetails(getProducerCID(), address)->getPriority();
}

void Producer::setProducerPointPriority(address_t address, priority_t priority)
{
    if (!getProducerPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(getProducerCID(), address)->setPriority(priority);
    emit newProducerPointPriority(priority);
}

/* Producer Addesses */
QList<address_t> Producer::getProducerAddresses()
{
    QList<address_t> ret;
    for (auto system : getProducerSystems())
        ret.append(getAddresses(system));

    return ret;
}
QList<address_t> Producer::getProducerAddresses(system_t system)
{
    QList<address_t> ret;
    for (auto group : getProducerGroups(system))
        ret.append(getAddresses(system, group));

    return ret;
}
QList<address_t> Producer::getProducerAddresses(system_t system, group_t group)
{
    QList<address_t> ret;
    for (auto point : getProducerPoints(system, group))
        ret.append(address_t(system, group, point));

    return ret;
}

/* Components */
QList<cid_t> Producer::getComponents() const
{
    return otpNetwork->getComponentList();
}

component_t Producer::getComponent(cid_t cid) const
{
    return otpNetwork->getComponent(cid);
}

/* Systems */
QList<system_t> Producer::getSystems() const
{
    return otpNetwork->getSystemList();
}

void Producer::addSystem(cid_t cid, system_t system)
{
    otpNetwork->addSystem(cid, system);
}

/* Groups */
QList<group_t> Producer::getGroups(system_t system) const
{
    return otpNetwork->getGroupList(system);
}

void Producer::addGroup(cid_t cid, system_t system, group_t group)
{
    otpNetwork->addGroup(cid, system, group);
}

/* Points */
QList<point_t> Producer::getPoints(system_t system, group_t group) const
{
    return otpNetwork->getPointList(system, group);
}
QList<point_t> Producer::getPoints(cid_t cid, system_t system, group_t group) const
{
    return otpNetwork->getPointList(cid, system, group);
}

QString Producer::getPointName(cid_t cid, address_t address) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return QString();
    if (cid.isNull())
    {
        cid_t newestCid;
        QDateTime lastSeen;
        for (auto cid : getComponents())
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

/* Addresses */
QList<address_t> Producer::getAddresses()
{
    QList<address_t> ret;
    for (auto system : getSystems())
        ret.append(getAddresses(system));

    return ret;
}
QList<address_t> Producer::getAddresses(system_t system)
{
    QList<address_t> ret;
    for (auto group : getGroups(system))
        ret.append(getAddresses(system, group));

    return ret;
}

QList<address_t> Producer::getAddresses(system_t system, group_t group)
{
    QList<address_t> ret;
    for (auto point : getPoints(system, group))
        ret.append(address_t(system, group, point));

    return ret;
}

/* Standard Modules */
QString Producer::getScaleString(MODULES::STANDARD::PositionModule_t::scale_t scale, bool html) const
{
    return MODULES::STANDARD::VALUES::UNITS::getScaleString(scale, html);
}

QString Producer::getUnitString(MODULES::STANDARD::VALUES::moduleValue_t moduleValue, bool html) const
{
    using namespace MODULES::STANDARD::VALUES;
    return QString ("%2")
            .arg(UNITS::getUnitString(moduleValue, html));
}

QString Producer::getUnitString(
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
Producer::PositionValue_t Producer::getProducerPosition(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::PositionValue_t ret;
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.position.getPosition(axis);
    ret.scale = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.position.getScaling();
    ret.unit = getUnitString(ret.scale, VALUES::POSITION);
    ret.timestamp = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.position.getTimestamp();
    return ret;
}

void Producer::setProducerPosition(address_t address, axis_t axis, PositionValue_t position)
{
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.position.setPosition(
                axis, position.value, position.timestamp);
    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.position.setScaling(
                position.scale);

    emit updatedPosition(address, axis);
}

/* Standard Modules - Position Velocity/Acceleration */
Producer::PositionVelocity_t Producer::getProducerPositionVelocity(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::PositionVelocity_t ret;
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.positionVelAcc.getVelocity(axis);
    ret.unit = getUnitString(VALUES::POSITION_VELOCITY);
    ret.timestamp = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.positionVelAcc.getTimestamp();
    return ret;
}

void Producer::setProducerPositionVelocity(address_t address, axis_t axis, PositionVelocity_t positionVel)
{
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.positionVelAcc.setVelocity(
                axis, positionVel.value, positionVel.timestamp);

    emit updatedPositionVelocity(address, axis);
}

Producer::PositionAcceleration_t Producer::getProducerPositionAcceleration(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::PositionAcceleration_t ret;
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.positionVelAcc.getAcceleration(axis);
    ret.unit = getUnitString(VALUES::POSITION_ACCELERATION);
    ret.timestamp = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.positionVelAcc.getTimestamp();
    return ret;
}

void Producer::setProducerPositionAcceleration(address_t address, axis_t axis, PositionAcceleration_t positionAccel)
{
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.positionVelAcc.setAcceleration(
                axis, positionAccel.value, positionAccel.timestamp);

    emit updatedPositionAcceleration(address, axis);
}

/* Standard Modules - Rotation */
Producer::RotationValue_t Producer::getProducerRotation(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::RotationValue_t ret;
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.rotation.getRotation(axis);
    ret.unit = getUnitString(VALUES::ROTATION);
    ret.timestamp = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.rotation.getTimestamp();
    return ret;
}

void Producer::setProducerRotation(address_t address, axis_t axis, RotationValue_t rotation)
{
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.rotation.setRotation(
                axis, rotation.value, rotation.timestamp);

    emit updatedRotation(address, axis);
}

/* Standard Modules - Position Velocity/Acceleration */
Producer::RotationVelocity_t Producer::getProducerRotationVelocity( address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::RotationVelocity_t ret;
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.rotationVelAcc.getVelocity(axis);
    ret.unit = getUnitString(VALUES::ROTATION_VELOCITY);
    ret.timestamp = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.rotationVelAcc.getTimestamp();
    return ret;
}

void Producer::setProducerRotationVelocity(address_t address, axis_t axis, RotationVelocity_t rotationVel)
{
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.rotationVelAcc.setVelocity(
                axis, rotationVel.value, rotationVel.timestamp);

    emit updatedRotationVelocity(address, axis);
}

Producer::RotationAcceleration_t Producer::getProducerRotationAcceleration(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::RotationAcceleration_t ret;
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.rotationVelAcc.getAcceleration(axis);
    ret.unit = getUnitString(VALUES::ROTATION_VELOCITY);
    ret.timestamp = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.rotationVelAcc.getTimestamp();
    return ret;
}

void Producer::setProducerRotationAcceleration(address_t address, axis_t axis, RotationAcceleration_t rotationAccel)
{
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.rotationVelAcc.setAcceleration(
                axis, rotationAccel.value, rotationAccel.timestamp);

    emit updatedRotationAcceleration(address, axis);
}

Producer::Scale_t Producer::getProducerScale(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::Scale_t ret;
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.scale.getScale(axis);
    ret.timestamp = otpNetwork->PointDetails(getProducerCID(), address)->standardModules.scale.getTimestamp();
    return ret;
}

void Producer::setProducerScale(address_t address, axis_t axis, Scale_t scale)
{
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getProducerCID(), address)->standardModules.scale.setScale(
                axis, scale.value, scale.timestamp);

    emit updatedScale(address, axis);
}

Producer::Parent_t Producer::getProducerParent(address_t address) const
{
    using namespace MODULES::STANDARD;
    Producer::Parent_t ret;
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point))
        return ret;

    auto module = &otpNetwork->PointDetails(getProducerCID(), address)->standardModules.parent;
    ret.value = {module->getSystem(), module->getGroup(), module->getPoint()};
    ret.relative = module->isRelative();
    ret.timestamp = module->getTimestamp();
    return ret;
}

void Producer::setProducerParent(address_t address, Parent_t parent)
{
    if (!getPoints(getProducerCID(), address.system, address.group).contains(address.point)) return;

    auto module = &otpNetwork->PointDetails(getProducerCID(), address)->standardModules.parent;
    module->setSystem(parent.value.system, parent.timestamp);
    module->setGroup(parent.value.group, parent.timestamp);
    module->setPoint(parent.value.point, parent.timestamp);
    module->setRelative(parent.relative, parent.timestamp);

    emit updatedParent(address);
}

void Producer::setupListener()
{
    qDebug() << this << "- Starting on interface" << iface.humanReadableName() << iface.hardwareAddress();

    for (auto connection : listenerConnections)
        disconnect(connection);

    if ((transport == QAbstractSocket::IPv4Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
    {
        listenerConnections.append(connect(SocketManager::getInstance(iface, QAbstractSocket::IPv4Protocol).get(), &SocketManager::newDatagram,
                this, &Producer::newDatagram));
        SocketManager::getInstance(iface, QAbstractSocket::IPv4Protocol)->joinMulticastGroup(OTP_Advertisement_Message_IPv4);
        qDebug() << this << "- Listening to Advertisement Messages" << OTP_Advertisement_Message_IPv4;
    }

    if ((transport == QAbstractSocket::IPv6Protocol) || (transport == QAbstractSocket::AnyIPProtocol))
    {
        listenerConnections.append(connect(SocketManager::getInstance(iface, QAbstractSocket::IPv6Protocol).get(), &SocketManager::newDatagram,
                this, &Producer::newDatagram));
        SocketManager::getInstance(iface, QAbstractSocket::IPv6Protocol)->joinMulticastGroup(OTP_Advertisement_Message_IPv6);
        qDebug() << this << "- Listening to Advertisement Messages" << OTP_Advertisement_Message_IPv6;
    }
}

void Producer::setupSender(std::chrono::milliseconds transformRate)
{
    qDebug() << this << "- Starting OTP Transform Messages" << iface.name();
    connect(&transformMsgTimer, &QTimer::timeout, [this]() {
        for (auto system : getProducerSystems())
            sendOTPTransformMessage(system);
    });
    transformRate = std::clamp(transformRate, OTP_TRANSFORM_TIMING_MIN, OTP_TRANSFORM_TIMING_MAX);
    transformMsgTimer.start(transformRate);
}

void Producer::newDatagram(QNetworkDatagram datagram)
{
    /* Unicast packets to self have no senderAddress */
    if (datagram.senderAddress().isNull()) datagram.setSender(datagram.destinationAddress());

    //  Advertisement Message
    if ((datagram.destinationAddress() == OTP_Advertisement_Message_IPv4) ||
            (datagram.destinationAddress() == OTP_Advertisement_Message_IPv6))
    {
        MESSAGES::OTPModuleAdvertisementMessage::Message moduleAdvert(datagram);
        MESSAGES::OTPNameAdvertisementMessage::Message nameAdvert(datagram);
        MESSAGES::OTPSystemAdvertisementMessage::Message systemAdvert(datagram);

        // Module Advertisement Message?
        if (moduleAdvert.isValid())
        {
            auto cid = moduleAdvert.getOTPLayer()->getCID();
            if (!sequenceMap[cid].checkSequence(
                        PDU::VECTOR_OTP_ADVERTISEMENT_MODULE,
                        moduleAdvert.getOTPLayer()->getSequence()))
            {
                qDebug() << this << "- Out of Sequence OTP Module Advertisement Message Request Received From" << datagram.senderAddress();
                return;
            }

            qDebug() << this << "- OTP Module Advertisement Message Request Received From" << datagram.senderAddress();

            otpNetwork->addComponent(
                        cid,
                        datagram.senderAddress(),
                        moduleAdvert.getOTPLayer()->getComponentName(),
                        component_t::type_t::consumer,
                        moduleAdvert.getModuleAdvertisementLayer()->getList());
            return;
        }

        // Name Advertisement Message?
        if (nameAdvert.isValid())
        {
            auto cid = nameAdvert.getOTPLayer()->getCID();
            if (!sequenceMap[cid].checkSequence(
                        PDU::VECTOR_OTP_ADVERTISEMENT_NAME,
                        nameAdvert.getOTPLayer()->getSequence()))
            {
                qDebug() << this << "- Out of Sequence OTP Name Advertisement Message Request Received From" << datagram.senderAddress();
                return;
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

            if (type == component_t::type_t::consumer)
            {
                auto folio = nameAdvert.getOTPLayer()->getFolio();
                auto destAddr = datagram.senderAddress();
                auto timer = getBackoffTimer(OTP_NAME_ADVERTISEMENT_MAX_BACKOFF);
                connect(timer, &QTimer::timeout, this, [this, destAddr, folio]() {
                    sendOTPNameAdvertisementMessage(destAddr, folio);
                });
                timer->start();
            }
            return;
        }

        // System Advertisement Message?
        if (systemAdvert.isValid() && systemAdvert.getSystemAdvertisementLayer()->getOptions().isRequest())
        {
            auto cid = systemAdvert.getOTPLayer()->getCID();
            if (!sequenceMap[cid].checkSequence(
                        PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM,
                        systemAdvert.getOTPLayer()->getSequence()))
            {
                qDebug() << this << "- Out of Sequence OTP System Advertisement Message Request Received From" << datagram.senderAddress();
                return;
            }

            auto type = (systemAdvert.getSystemAdvertisementLayer()->getOptions().isResponse()) ? component_t::type_t::produder : component_t::type_t::consumer;
            if (type == component_t::type_t::produder)
                qDebug() << this << "- OTP System Advertisement Message Response Received From" << datagram.senderAddress();
            else if (type == component_t::type_t::consumer)
                qDebug() << this << "- OTP System Advertisement Message Request Received From" << datagram.senderAddress();

            for (auto system : systemAdvert.getSystemAdvertisementLayer()->getList())
            {
                otpNetwork->addComponent(
                        cid,
                        datagram.senderAddress(),
                        systemAdvert.getOTPLayer()->getComponentName(),
                        type);

                otpNetwork->addSystem(
                        cid,
                        system);
            }

            if (type == component_t::type_t::consumer)
            {
                auto folio = systemAdvert.getOTPLayer()->getFolio();
                auto destAddr = datagram.senderAddress();
                auto timer = getBackoffTimer(OTP_SYSTEM_ADVERTISEMENT_MAX_BACKOFF);
                connect(timer, &QTimer::timeout, this, [this, destAddr, folio]() {
                    sendOTPSystemAdvertisementMessage(destAddr, folio);
                });
                timer->start();
            }
            return;
        }
    }
}

QTimer* Producer::getBackoffTimer(std::chrono::milliseconds maximum)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, maximum.count());

    auto timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(std::chrono::milliseconds(dis(gen)));
    connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));
    return timer;
}

void Producer::sendOTPNameAdvertisementMessage(QHostAddress destinationAddr, MESSAGES::OTPNameAdvertisementMessage::folio_t folio)
{
    using namespace OTP::MESSAGES::OTPNameAdvertisementMessage;

    // Create a List of Address Point Descriptions
    list_t list;
    for (auto address : getProducerAddresses())
    {
        item_t item(address.system, address.group, address.point,
                    getProducerPointName(address));
        list.append(item);
    }

    // Generate messages
    QVector<std::shared_ptr<Message>> folioMessages;
    while (list.count()) {
        folioMessages.append(
                    std::make_shared<Message>(
                        mode_e::Producer,
                        getProducerCID(),
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
            qDebug() << this << "- OTP Name Advertisement Message Response Not Valid";
            return;
        }
    }

    // Send messages
    page_t lastPage = static_cast<page_t>(folioMessages.count()) - 1;
    for (page_t page = 0; page < folioMessages.count(); page++)
    {
        auto datagram = folioMessages[page]->toQNetworkDatagram(
                    destinationAddr,
                    sequenceMap[getProducerCID()].getNextSequence(PDU::VECTOR_OTP_ADVERTISEMENT_NAME),
                    folio,
                    page,
                    lastPage);
        if (SocketManager::getInstance(iface, destinationAddr.protocol())->writeDatagram(datagram))
            qDebug() << this << "- OTP Name Advertisement Message Response Sent to" << destinationAddr;
        else
            qDebug() << this << "- OTP Name Advertisement Message Response Failed";
    }
}

void Producer::sendOTPSystemAdvertisementMessage(QHostAddress destinationAddr, MESSAGES::OTPNameAdvertisementMessage::folio_t folio)
{
    using namespace OTP::MESSAGES::OTPSystemAdvertisementMessage;

    // Get list of systems
    list_t list = otpNetwork->getSystemList(getProducerCID());

    // Generate messages
    QVector<std::shared_ptr<Message>> folioMessages;
    while (list.count()) {
        folioMessages.append(
                    std::make_shared<Message>(
                        mode_e::Producer,
                        getProducerCID(),
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
            qDebug() << this << "- OTP System Advertisement Message Response Not Valid";
            return;
        }
    }

    // Send messages
    page_t lastPage = static_cast<page_t>(folioMessages.count()) - 1;
    for (page_t page = 0; page < folioMessages.count(); page++)
    {
        auto datagram = folioMessages[page]->toQNetworkDatagram(
                    destinationAddr,
                    sequenceMap[getProducerCID()].getNextSequence(PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM),
                    folio,
                    page,
                    lastPage);

        if (SocketManager::getInstance(iface, destinationAddr.protocol())->writeDatagram(datagram))
            qDebug() << this << "- OTP System Advertisement Message Response Sent To" << datagram.destinationAddress();
        else
            qDebug() << this << "- OTP System Advertisement Message Response Failed";
    }
}

void Producer::sendOTPTransformMessage(system_t system)
{
    using namespace OTP::MESSAGES::OTPTransformMessage;
    auto folio = TransformMessage_Folio++;

    // Establish requested modules for system
    QVector<PDU::OTPModuleLayer::ident_t> requestedModules;
    for (auto cid : otpNetwork->getComponentList())
    {
        if (otpNetwork->getSystemList(cid).contains(system))
        {
            auto component = otpNetwork->getComponent(cid);
            for (auto module : component.getModuleList())
            {
                if (!requestedModules.contains(module))
                    requestedModules.append(module);
            }
        }
    }
    if (requestedModules.isEmpty()) return;

    // Get each requested module
    QVector<Message::addModule_t> folioModuleData;
    for (auto address: getProducerAddresses(system))
    {
        auto pointDetails = otpNetwork->PointDetails(getProducerCID(), address);
        for (auto module : requestedModules)
        {
            switch (module.ManufacturerID)
            {
                case ESTA_MANUFACTURER_ID:
                {
                    folioModuleData.append(
                        {pointDetails->getPriority(),
                         address,
                         MODULES::STANDARD::getTimestamp(module, pointDetails),
                         {module.ManufacturerID, module.ModuleNumber},
                         MODULES::STANDARD::getAdditional(module, pointDetails)});
                } break;
                default:
                {
                    qDebug() << this << "- OTP Transform - Unknown module request" << module.ManufacturerID << "/" << module.ModuleNumber;
                } return;
            }
        }
    }

    // Generate messages
    QVector<std::shared_ptr<Message>> folioMessages;
    while (folioModuleData.count()) {
        folioMessages.append(std::make_shared<Message>(getProducerCID(), name, system, true, this));

        Message::addModule_ret result;
        do {
            result = folioMessages.back()->addModule(folioModuleData.front());
            if (result != Message::addModule_ret::MessageToBig)
                folioModuleData.removeFirst();
        } while ((result != Message::addModule_ret::MessageToBig) && folioModuleData.count());

        if (!folioMessages.back()->isValid())
        {
            qDebug() << this << "- OTP Transform Message Not Valid";
            return;
        }
    }

    // Send messages
    page_t lastPage = static_cast<page_t>(folioMessages.count()) - 1;
    for (page_t page = 0; page < folioMessages.count(); page++)
    {
        auto datagrams = folioMessages[page]->toQNetworkDatagrams(
                    transport,
                    sequenceMap[getProducerCID()].getNextSequence(PDU::VECTOR_OTP_TRANSFORM_MESSAGE),
                    folio,
                    page,
                    lastPage);

        if (!SocketManager::writeDatagrams(iface, datagrams))
            qDebug() << this << "- OTP Transform Message Failed";
    }
}
