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

#include <QDebug>

using namespace ACN::OTP;

Producer::Producer(
        QNetworkInterface iface,
        QAbstractSocket::NetworkLayerProtocol transport,
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
    connect(otpNetwork.get(), qOverload<const ACN::OTP::cid_t&, const QHostAddress&>(&Container::updatedComponent),
            this, qOverload<const ACN::OTP::cid_t&, const QHostAddress&>(&Producer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const ACN::OTP::cid_t&, const moduleList_t &>(&Container::updatedComponent),
            this, qOverload<const ACN::OTP::cid_t&, const moduleList_t &>(&Producer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const ACN::OTP::cid_t&, ACN::OTP::component_t::type_t>(&Container::updatedComponent),
            this, qOverload<const ACN::OTP::cid_t&, ACN::OTP::component_t::type_t>(&Producer::updatedComponent));

    connect(otpNetwork.get(), &Container::newSystem, this, &Producer::newSystem);
    connect(otpNetwork.get(), &Container::removedSystem, this, &Producer::removedSystem);

    connect(otpNetwork.get(), &Container::newGroup, this, &Producer::newGroup);
    connect(otpNetwork.get(), &Container::removedGroup, this, &Producer::removedGroup);

    connect(otpNetwork.get(), &Container::newPoint, this, &Producer::newPoint);
    connect(otpNetwork.get(), &Container::removedPoint, this, &Producer::removedPoint);

    setupListener();
    auto startSenderTimeout = new QTimer;
    startSenderTimeout->setSingleShot(true);
    connect(startSenderTimeout, SIGNAL(timeout()), this, SLOT(setupSender()));
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

void Producer::addProducerPoint(address_t address)
{
    otpNetwork->addPoint(getProducerCID(), address);
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

QString Producer::getPointName(cid_t cid, address_t address) const
{
    if (!getPoints(address.system, address.group).contains(address.point)) return QString();
    if (cid.isNull()) return otpNetwork->PointDetails(address)->getName();
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
QString Producer::getScaleString(address_t address, MODULES::STANDARD::VALUES::moduleValue_t moduleValue) const
{
    using namespace MODULES::STANDARD::VALUES;
    switch (moduleValue)
    {
        case POSITION: return UNITS::getScaleString(getPositionScale(address));
        default: return QString();
    }
}

QString Producer::getUnitString(address_t address, MODULES::STANDARD::VALUES::moduleValue_t moduleValue) const
{
    using namespace MODULES::STANDARD::VALUES;
    return QString ("%1%2")
            .arg(getScaleString(address, moduleValue))
            .arg(UNITS::getUnitString(moduleValue));
}

/* Standard Modules - Position */
MODULES::STANDARD::PositionModule_t::scale_t Producer::getPositionScale(address_t address) const
{
    if (!getPoints(address.system, address.group).contains(address.point))
        return MODULES::STANDARD::PositionModule_t::scale_t();
    return otpNetwork->PointDetails(address)->standardModules.position.getScaling();
}

void Producer::setPositionScale(address_t address, MODULES::STANDARD::PositionModule_t::scale_t scale)
{
    if (!getPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(address)->standardModules.position.setScaling(scale);
    for (auto axis = axis_t::first; axis < axis_t::count; axis++)
        emit updatedPosition(address, axis);
}


MODULES::STANDARD::PositionModule_t::location_t Producer::getPositionLocation(address_t address, axis_t axis) const
{
    if (!getPoints(address.system, address.group).contains(address.point))
        return MODULES::STANDARD::PositionModule_t::location_t();
    return otpNetwork->PointDetails(address)->standardModules.position.getLocation(axis);
}

void Producer::setPositionLocation(
        address_t address,
        axis_t axis,
        MODULES::STANDARD::PositionModule_t::location_t value,
        timestamp_t timestamp)
{
    if (!getPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(address)->standardModules.position.setLocation(axis, value, timestamp);

    emit updatedPosition(address, axis);
}

/* Standard Modules - Position Velocity/Acceleration */
MODULES::STANDARD::PositionVelAccModule_t::velocity_t Producer::getPositionVelocity(address_t address, axis_t axis) const
{
    if (!getPoints(address.system, address.group).contains(address.point))
        return MODULES::STANDARD::PositionVelAccModule_t::velocity_t();
    return otpNetwork->PointDetails(address)->standardModules.positionVelAcc.getVelocity(axis);
}

void Producer::setPositionVelocity(
        address_t address,
        axis_t axis,
        MODULES::STANDARD::PositionVelAccModule_t::velocity_t value,
        timestamp_t timestamp)
{
    if (!getPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(address)->standardModules.positionVelAcc.setVelocity(axis, value, timestamp);

    emit updatedPositionVelocity(address, axis);
}

MODULES::STANDARD::PositionVelAccModule_t::acceleration_t Producer::getPositionAcceleration(address_t address, axis_t axis) const
{
    if (!getPoints(address.system, address.group).contains(address.point))
        return MODULES::STANDARD::PositionVelAccModule_t::acceleration_t();
    return otpNetwork->PointDetails(address)->standardModules.positionVelAcc.getAcceleration(axis);
}

void Producer::setPositionAcceleration(
        address_t address,
        axis_t axis,
        MODULES::STANDARD::PositionVelAccModule_t::acceleration_t value,
        timestamp_t timestamp)
{
    if (!getPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(address)->standardModules.positionVelAcc.setAcceleration(axis, value, timestamp);

    emit updatedPositionAcceleration(address, axis);
}

/* Standard Modules - Rotation */
MODULES::STANDARD::RotationModule_t::rotation_t Producer::getRotation(address_t address, axis_t axis) const
{
    if (!getPoints(address.system, address.group).contains(address.point))
        return MODULES::STANDARD::RotationModule_t::rotation_t();
    return otpNetwork->PointDetails(address)->standardModules.rotation.getRotation(axis);
}

void Producer::setRotation(
        address_t address,
        axis_t axis,
        MODULES::STANDARD::RotationModule_t::rotation_t value,
        timestamp_t timestamp)
{
    if (!getPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(address)->standardModules.rotation.setRotation(axis, value, timestamp);

    emit updatedRotation(address, axis);
}

/* Standard Modules - Position Velocity/Acceleration */
MODULES::STANDARD::RotationVelAccModule_t::velocity_t Producer::getRotationVelocity(address_t address, axis_t axis) const
{
    if (!getPoints(address.system, address.group).contains(address.point))
        return MODULES::STANDARD::RotationVelAccModule_t::velocity_t();
   return otpNetwork->PointDetails(address)->standardModules.rotationVelAcc.getVelocity(axis);
}

void Producer::setRotationVelocity(
        address_t address,
        axis_t axis,
        MODULES::STANDARD::RotationVelAccModule_t::velocity_t value,
        timestamp_t timestamp)
{
    if (!getPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(address)->standardModules.rotationVelAcc.setVelocity(axis, value, timestamp);

    emit updatedRotationVelocity(address, axis);
}

MODULES::STANDARD::RotationVelAccModule_t::acceleration_t Producer::getRotationAcceleration(address_t address, axis_t axis) const
{
    if (!getPoints(address.system, address.group).contains(address.point))
        return MODULES::STANDARD::RotationVelAccModule_t::acceleration_t();
    return otpNetwork->PointDetails(address)->standardModules.rotationVelAcc.getAcceleration(axis);
}

void Producer::setRotationAcceleration(
        address_t address,
        axis_t axis,
        MODULES::STANDARD::RotationVelAccModule_t::acceleration_t value,
        timestamp_t timestamp)
{
    if (!getPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(address)->standardModules.rotationVelAcc.setAcceleration(axis, value, timestamp);

    emit updatedRotationAcceleration(address, axis);
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

void Producer::setupSender()
{
    qDebug() << this << "- Starting OTP Transform Messages" << iface.name();
    auto transformTimer = new QTimer(this);
    connect(transformTimer, &QTimer::timeout, [this]() {
        for (auto system : getProducerSystems())
            sendOTPTransformMessage(system);
    });
    transformTimer->start(OTP_TRANSFORM_TIMING_MAX);
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
            auto cid = moduleAdvert.getRootLayer()->getCID();
            if (!sequenceMap[cid].checkSequence(
                        PDU::VECTOR_OTP_ADVERTISEMENT_MODULE,
                        moduleAdvert.getOTPLayer()->getSequence()))
            {
                qDebug() << this << "- Out of Sequence OTP Module Advertisement Message Request Received From" << datagram.senderAddress();
                return;
            }

            qDebug() << this << "- OTP Module Advertisement Message Request Received From" << datagram.senderAddress();

            otpNetwork->addComponent(
                        moduleAdvert.getRootLayer()->getCID(),
                        datagram.senderAddress(),
                        moduleAdvert.getOTPLayer()->getProducerName(),
                        component_t::type_t::consumer,
                        moduleAdvert.getModuleAdvertisementLayer()->getList());
            return;
        }

        // Name Advertisement Message?
        if (nameAdvert.isValid())
        {
            auto cid = nameAdvert.getRootLayer()->getCID();
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
                    nameAdvert.getRootLayer()->getCID(),
                    datagram.senderAddress(),
                    nameAdvert.getOTPLayer()->getProducerName(),
                    type);

            if (type == component_t::type_t::consumer)
                sendOTPNameAdvertisementMessage(datagram.senderAddress(), nameAdvert.getOTPLayer()->getFolio());
            return;
        }

        // System Advertisement Message?
        if (systemAdvert.isValid() && systemAdvert.getSystemAdvertisementLayer()->getOptions().isRequest())
        {
            auto cid = systemAdvert.getRootLayer()->getCID();
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
                        systemAdvert.getRootLayer()->getCID(),
                        datagram.senderAddress(),
                        systemAdvert.getOTPLayer()->getProducerName(),
                        type);

                otpNetwork->addSystem(
                        systemAdvert.getRootLayer()->getCID(),
                        system);
            }

            if (type == component_t::type_t::consumer)
                sendOTPSystemAdvertisementMessage(datagram.senderAddress(), systemAdvert.getOTPLayer()->getFolio());
            return;
        }
    }
}

void Producer::sendOTPNameAdvertisementMessage(QHostAddress destinationAddr, MESSAGES::OTPNameAdvertisementMessage::folio_t folio)
{
    using namespace ACN::OTP::MESSAGES::OTPNameAdvertisementMessage;

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
    for (int n = 0; n < folioMessages.count(); n++)
    {
        auto datagram = folioMessages[n]->toQNetworkDatagram(
                    destinationAddr,
                    sequenceMap[getProducerCID()].getNextSequence(PDU::VECTOR_OTP_ADVERTISEMENT_NAME),
                    folio,
                    n,
                    folioMessages.count() - 1);
        if (SocketManager::getInstance(iface, destinationAddr.protocol())->writeDatagram(datagram))
            qDebug() << this << "- OTP Name Advertisement Message Response Sent to" << destinationAddr;
        else
            qDebug() << this << "- OTP Name Advertisement Message Response Failed";
    }
}

void Producer::sendOTPSystemAdvertisementMessage(QHostAddress destinationAddr, MESSAGES::OTPNameAdvertisementMessage::folio_t folio)
{
    using namespace ACN::OTP::MESSAGES::OTPSystemAdvertisementMessage;

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
    for (int n = 0; n < folioMessages.count(); n++)
    {
        auto datagram = folioMessages[n]->toQNetworkDatagram(
                    destinationAddr,
                    sequenceMap[getProducerCID()].getNextSequence(PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM),
                    folio,
                    n,
                    folioMessages.count() - 1);

        if (SocketManager::getInstance(iface, destinationAddr.protocol())->writeDatagram(datagram))
            qDebug() << this << "- OTP System Advertisement Message Response Sent To" << datagram.destinationAddress();
        else
            qDebug() << this << "- OTP System Advertisement Message Response Failed";
    }
}

void Producer::sendOTPTransformMessage(system_t system)
{
    using namespace ACN::OTP::MESSAGES::OTPTransformMessage;
    auto folio = TransformMessage_Folio++;

    // Establish requested modules for system
    QVector<PDU::OTPModuleLayer::vector_t> requestedModules;
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
        for (auto module : requestedModules)
        {
            switch (module.ManufacturerID)
            {
                case ESTA_MANUFACTURER_ID:
                {
                    folioModuleData.append(
                        {address,
                         MODULES::STANDARD::getTimestamp(module, otpNetwork->PointDetails(address)),
                         {module.ManufacturerID, module.ModuleNumber},
                         MODULES::STANDARD::getAdditional(module, otpNetwork->PointDetails(address))});
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
        folioMessages.append(std::make_shared<Message>(getProducerCID(), name, system, this));

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
    for (int n = 0; n < folioMessages.count(); n++)
    {
        auto datagrams = folioMessages[n]->toQNetworkDatagrams(
                    transport,
                    sequenceMap[getProducerCID()].getNextSequence(PDU::VECTOR_OTP_TRANSFORM_MESSAGE),
                    folio,
                    n,
                    folioMessages.count() - 1);

        if (!SocketManager::writeDatagrams(iface, datagrams))
            qDebug() << this << "- OTP Transform Message Failed";
    }
}
