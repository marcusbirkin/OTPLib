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

using namespace ACN::OTP;

Consumer::Consumer(
        QNetworkInterface iface,
        QList<system_t> systems,
        name_t name,
        cid_t CID,
        QObject *parent) :
    QObject(parent),
    otpNetwork(new Container(this)),
    iface(iface),
    CID(CID),
    name(name)
{
    // Signals
    connect(otpNetwork.get(), &Container::newComponent, this, &Consumer::newComponent);
    connect(otpNetwork.get(), &Container::removedComponent, this, &Consumer::removedComponent);

    connect(otpNetwork.get(), qOverload<const cid_t&, const name_t&>(&Container::updatedComponent),
            this, qOverload<const cid_t&, const name_t&>(&Consumer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const ACN::OTP::cid_t&, const QHostAddress&>(&Container::updatedComponent),
            this, qOverload<const ACN::OTP::cid_t&, const QHostAddress&>(&Consumer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const ACN::OTP::cid_t&, const moduleList_t &>(&Container::updatedComponent),
            this, qOverload<const ACN::OTP::cid_t&, const moduleList_t &>(&Consumer::updatedComponent));
    connect(otpNetwork.get(), qOverload<const ACN::OTP::cid_t&, ACN::OTP::component_t::type_t>(&Container::updatedComponent),
            this, qOverload<const ACN::OTP::cid_t&, ACN::OTP::component_t::type_t>(&Consumer::updatedComponent));

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

    auto groupAddress = OTP_Advertisement_Message_IPv4;
    if (SocketManager::getInstance(iface)->joinMulticastGroup(groupAddress))
        qDebug() << this << "- Listening to Advertisement Messages" << groupAddress;

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

/* Component CID */
void Consumer::setConsumerCID(cid_t value)
{
    if (CID == value) return;
    CID = value;
    emit newConsumerCID(getConsumerCID());
}

/* Component Name */
void Consumer::setConsumerName(name_t value)
{
    if (name == value) return;
    name = value;
    emit newConsumerName(name);
}

/* Component Network Interface */
void Consumer::setConsumerNetworkInterface(QNetworkInterface value)
{
    if (iface.name() == value.name()) return;
    iface = value;
    setupListener();
    emit newConsumerNetworkInterface(iface);
}

/* Component Systems */
QList<system_t> Consumer::getConsumerSystems() const
{
    return otpNetwork->getSystemList(getConsumerCID());
}

void Consumer::addConsumerSystem(system_t system)
{
    if (system.isValid())
    {
        if (!getConsumerSystems().contains(system)) otpNetwork->addSystem(getConsumerCID(), system);
        auto groupAddress = QHostAddress(OTP_Transform_Message_IPv4.toIPv4Address() + system);
        if (SocketManager::getInstance(iface)->joinMulticastGroup(groupAddress))
        {
            qDebug() << this << "- Listening to Transform Messages for System" << system << groupAddress;
            emit newConsumerSystem(system);
        }
    }
}

void Consumer::removeConsumerSystem(system_t system)
{
    if (system.isValid())
    {
        otpNetwork->removeSystem(getConsumerCID(), system);
        auto groupAddress = QHostAddress(OTP_Transform_Message_IPv4.toIPv4Address() + system);
        if (SocketManager::getInstance(iface)->leaveMulticastGroup(groupAddress))
        {
            qDebug() << this << "- Stopping listening to Transform Messages for System" << system << groupAddress;
            removedConsumerSystem(system);
        }
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
        if (cid.isNull())
        {
            if (!otpNetwork->PointDetails(address)->isExpired()) return false;
        } else {
            if (!otpNetwork->PointDetails(cid, address)->isExpired()) return false;
        }
    }
    return true;
}

/* Points */
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
    if (cid.isNull()) return otpNetwork->PointDetails(address)->getName();
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return QString();
    return otpNetwork->PointDetails(cid, address)->getName();
}

QDateTime Consumer::getPointLastSeen(cid_t cid, address_t address) const
{
    if (cid.isNull()) return otpNetwork->PointDetails(address)->getLastSeen();
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return QDateTime();
    return otpNetwork->PointDetails(cid, address)->getLastSeen();
}

bool Consumer::isPointExpired(cid_t cid, address_t address) const
{
    if (cid.isNull()) return otpNetwork->PointDetails(address)->isExpired();
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return true;
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
QString Consumer::getScaleString(cid_t cid, address_t address, MODULES::STANDARD::VALUES::moduleValue_t moduleValue) const
{
    using namespace MODULES::STANDARD::VALUES;
    switch (moduleValue)
    {
        case POSITION: return UNITS::getScaleString(getPositionScale(cid, address));
        default: return QString();
    }
}

QString Consumer::getUnitString(cid_t cid, address_t address, MODULES::STANDARD::VALUES::moduleValue_t moduleValue) const
{
    using namespace MODULES::STANDARD::VALUES;
    return QString ("%1%2")
            .arg(getScaleString(cid, address, moduleValue))
            .arg(UNITS::getUnitString(moduleValue));
}

/* Standard Modules - Position */
MODULES::STANDARD::PositionModule_t::scale_t Consumer::getPositionScale(cid_t cid, address_t address) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point))
        return MODULES::STANDARD::PositionModule_t::scale_t();
    return otpNetwork->PointDetails(cid, address)->standardModules.position.getScaling();
}

MODULES::STANDARD::PositionModule_t::location_t Consumer::getPositionLocation(cid_t cid, address_t address, axis_t axis) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point))
        return MODULES::STANDARD::PositionModule_t::location_t();
    return otpNetwork->PointDetails(cid, address)->standardModules.position.getLocation(axis);
}

timestamp_t Consumer::getRotationTimestamp(cid_t cid, address_t address) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return 0;
    return otpNetwork->PointDetails(cid, address)->standardModules.rotation.getTimestamp();
}

/* Standard Modules - Position Velocity/Acceleration */
MODULES::STANDARD::PositionVelAccModule_t::velocity_t Consumer::getPositionVelocity(cid_t cid, address_t address, axis_t axis) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point))
        return MODULES::STANDARD::PositionVelAccModule_t::velocity_t();
    return otpNetwork->PointDetails(cid, address)->standardModules.positionVelAcc.getVelocity(axis);
}

timestamp_t Consumer::getPositionVelocityTimestamp(cid_t cid, address_t address) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return 0;
    return otpNetwork->PointDetails(cid, address)->standardModules.positionVelAcc.getTimestamp();
}

MODULES::STANDARD::PositionVelAccModule_t::acceleration_t Consumer::getPositionAcceleration(cid_t cid, address_t address, axis_t axis) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point))
        return MODULES::STANDARD::PositionVelAccModule_t::acceleration_t();
    return otpNetwork->PointDetails(cid, address)->standardModules.positionVelAcc.getAcceleration(axis);
}

timestamp_t Consumer::getPositionAccelerationTimestamp(cid_t cid, address_t address) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return 0;
    return otpNetwork->PointDetails(cid, address)->standardModules.positionVelAcc.getTimestamp();
}

/* Standard Modules - Rotation */
MODULES::STANDARD::RotationModule_t::rotation_t Consumer::getRotation(cid_t cid, address_t address, axis_t axis) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point))
        return MODULES::STANDARD::RotationModule_t::rotation_t();
    return otpNetwork->PointDetails(cid, address)->standardModules.rotation.getRotation(axis);
}

timestamp_t Consumer::getPositionTimestamp(cid_t cid, address_t address) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return 0;
    return otpNetwork->PointDetails(cid, address)->standardModules.position.getTimestamp();
}

/* Standard Modules - Position Velocity/Acceleration */
MODULES::STANDARD::RotationVelAccModule_t::velocity_t Consumer::getRotationVelocity(cid_t cid, address_t address, axis_t axis) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point))
        return MODULES::STANDARD::RotationVelAccModule_t::velocity_t();
    return otpNetwork->PointDetails(cid, address)->standardModules.rotationVelAcc.getVelocity(axis);
}

timestamp_t Consumer::getRotationVelocityTimestamp(cid_t cid, address_t address) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return 0;
    return otpNetwork->PointDetails(cid, address)->standardModules.rotationVelAcc.getTimestamp();
}

MODULES::STANDARD::RotationVelAccModule_t::acceleration_t Consumer::getRotationAcceleration(cid_t cid, address_t address, axis_t axis) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point))
        return MODULES::STANDARD::RotationVelAccModule_t::acceleration_t();
    return otpNetwork->PointDetails(cid, address)->standardModules.rotationVelAcc.getAcceleration(axis);
}

timestamp_t Consumer::getRotationAccelerationTimestamp(cid_t cid, address_t address) const
{
    if (!getPoints(cid, address.system, address.group).contains(address.point)) return 0;
    return otpNetwork->PointDetails(cid, address)->standardModules.rotationVelAcc.getTimestamp();
}


void Consumer::setupListener()
{
    qDebug() << this << "- Starting on interface" << iface.humanReadableName() << iface.hardwareAddress();
    connect(SocketManager::getInstance(iface).get(), &SocketManager::newDatagram, this, &Consumer::newDatagram);

    for (auto system : getConsumerSystems())
        addConsumerSystem(system);
}

void Consumer::newDatagram(QNetworkDatagram datagram)
{
    // Transform Message
    if ((datagram.destinationAddress().toIPv4Address() >= OTP_Transform_Message_IPv4.toIPv4Address()) &&
            (datagram.destinationAddress().toIPv4Address() <= OTP_Transform_Message_IPv4.toIPv4Address() + RANGES::System.getMax()))
    {
        MESSAGES::OTPTransformMessage::Message transformMessage(datagram);
        if (transformMessage.isValid())
        {
            auto cid = transformMessage.getRootLayer()->getCID();
            if (!sequenceMap[cid].checkSequence(
                        PDU::VECTOR_OTP_TRANSFORM_MESSAGE,
                        transformMessage.getOTPLayer()->getSequence()))
            {
                qDebug() << this << "- Out of Sequence OTP Name Advertisement Message Request Received From" << datagram.senderAddress();
                return;
            }

            for (auto pointLayer : transformMessage.getPointLayers())
            {
                auto address = address_t{
                    transformMessage.getTransformLayer()->getSystem(),
                    pointLayer->getGroup(),
                    pointLayer->getPoint()};
                auto timestamp = pointLayer->getTimestamp();
                otpNetwork->addPoint(cid, address);
                for (auto moduleLayer : transformMessage.getModuleLayers().values(address))
                {
                    switch (moduleLayer->getVector().ManufacturerID)
                    {
                        case ESTA_MANUFACTURER_ID:
                        {
                            switch (moduleLayer->getVector().ModuleNumber) {
                                case MODULES::STANDARD::POSITION:
                                {
                                    auto oldModule = otpNetwork->PointDetails(cid, address)->standardModules.position;
                                    auto newModule = MODULES::STANDARD::PositionModule_t(moduleLayer->getAdditional(), timestamp);
                                    otpNetwork->PointDetails(cid, address)->standardModules.position = newModule;
                                    for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                                        if (oldModule.getLocation(axis) != newModule.getLocation(axis))
                                            emit updatedPosition(cid, address, axis);
                                } break;

                                case MODULES::STANDARD::POSITION_VELOCITY_ACCELERATION:
                                {
                                    auto oldModule = otpNetwork->PointDetails(cid, address)->standardModules.positionVelAcc;
                                    auto newModule = MODULES::STANDARD::PositionVelAccModule_t(moduleLayer->getAdditional(), timestamp);
                                    otpNetwork->PointDetails(cid, address)->standardModules.positionVelAcc = newModule;
                                    for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                                    {
                                        if (oldModule.getVelocity(axis) != newModule.getVelocity(axis))
                                            emit updatedPositionVelocity(cid, address, axis);
                                        if (oldModule.getAcceleration(axis) != newModule.getAcceleration(axis))
                                            emit updatedPositionAcceleration(cid, address, axis);
                                    }
                                } break;

                                case MODULES::STANDARD::ROTATION:
                                {
                                    auto oldModule = otpNetwork->PointDetails(cid, address)->standardModules.rotation;
                                    auto newModule = MODULES::STANDARD::RotationModule_t(moduleLayer->getAdditional(), timestamp);
                                    otpNetwork->PointDetails(cid, address)->standardModules.rotation = newModule;
                                    for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                                        if (oldModule.getRotation(axis) != newModule.getRotation(axis))
                                            emit updatedRotation(cid, address, axis);
                                } break;

                                case MODULES::STANDARD::ROTATION_VELOCITY_ACCELERATION:
                                {
                                    auto oldModule = otpNetwork->PointDetails(cid, address)->standardModules.rotationVelAcc;
                                    auto newModule = MODULES::STANDARD::RotationVelAccModule_t(moduleLayer->getAdditional(), timestamp);
                                    otpNetwork->PointDetails(cid, address)->standardModules.rotationVelAcc = newModule;
                                    for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                                    {
                                        if (oldModule.getVelocity(axis) != newModule.getVelocity(axis))
                                            emit updatedRotationVelocity(cid, address, axis);
                                        if (oldModule.getAcceleration(axis) != newModule.getAcceleration(axis))
                                            emit updatedRotationAcceleration(cid, address, axis);
                                    }
                                } break;

                                default:
                                {
                                qDebug() << this << "Unknown module ID"
                                         << moduleLayer->getVector().ManufacturerID << moduleLayer->getVector().ModuleNumber
                                         << "From" << datagram.senderAddress();
                                } break;
                            }

                        } break;
                        default:
                        {
                            qDebug() << this << "Unknown module Manufacturer ID"
                                     << moduleLayer->getVector().ManufacturerID
                                     << "From" << datagram.senderAddress();
                        } break;
                    }

                }
            }
        }
        return;
    }

    //  Advertisement Message
    MESSAGES::OTPNameAdvertisementMessage::Message nameAdvert(datagram);
    MESSAGES::OTPSystemAdvertisementMessage::Message systemAdvert(datagram);

    // Name Advertisement Message?
    if (nameAdvert.isValid())
    {
        auto cid = nameAdvert.getRootLayer()->getCID();
        if (cid != getConsumerCID() && !sequenceMap[cid].checkSequence(
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

        // Add page to folio map
        folioMap.addPage(
                    nameAdvert.getRootLayer()->getCID(),
                    PDU::VECTOR_OTP_ADVERTISEMENT_NAME,
                    nameAdvert.getOTPLayer()->getFolio(),
                    nameAdvert.getOTPLayer()->getPage(),
                    datagram);

        // Last page?
        if (folioMap.checkAllPages(
                    nameAdvert.getRootLayer()->getCID(),
                    PDU::VECTOR_OTP_ADVERTISEMENT_NAME,
                    nameAdvert.getOTPLayer()->getFolio(),
                    nameAdvert.getOTPLayer()->getLastPage()))
        {
            // Process all pages
            MESSAGES::OTPNameAdvertisementMessage::list_t list;
            for (auto datagram : folioMap.getDatagrams(nameAdvert.getRootLayer()->getCID(),
                                                       PDU::VECTOR_OTP_ADVERTISEMENT_NAME,
                                                       systemAdvert.getOTPLayer()->getFolio()))
            {
                MESSAGES::OTPNameAdvertisementMessage::Message nameAdvert(datagram);
                list.append(nameAdvert.getNameAdvertisementLayer()->getList());
            }

            // Add names
            for (auto point : list)
            {
                auto cid = nameAdvert.getRootLayer()->getCID();
                address_t address = address_t(point.System, point.Group, point.Point);
                otpNetwork->addPoint(cid, address);
                otpNetwork->PointDetails(cid, address)->setName(point.PointName);
            }
        }
        return;
    }

    // System Advertisement Message?
    if (systemAdvert.isValid())
    {
        auto cid = systemAdvert.getRootLayer()->getCID();
        if (cid != getConsumerCID() && !sequenceMap[cid].checkSequence(
                    PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM,
                    systemAdvert.getOTPLayer()->getSequence()))
        {
            qDebug() << this << "- Out of Sequence OTP Name Advertisement Message Request Received From" << datagram.senderAddress();
            return;
        }

        auto type = (systemAdvert.getSystemAdvertisementLayer()->getOptions().isResponse()) ? component_t::type_t::produder : component_t::type_t::consumer;
        if (type == component_t::type_t::produder)
            qDebug() << this << "- OTP System Advertisement Message Response Received From" << datagram.senderAddress();
        else if (type == component_t::type_t::consumer)
            qDebug() << this << "- OTP System Advertisement Message Request Received From" << datagram.senderAddress();

        otpNetwork->addComponent(
                systemAdvert.getRootLayer()->getCID(),
                datagram.senderAddress(),
                systemAdvert.getOTPLayer()->getProducerName(),
                type);

        // Add page to folio map
        folioMap.addPage(
                    systemAdvert.getRootLayer()->getCID(),
                    PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM,
                    systemAdvert.getOTPLayer()->getFolio(),
                    systemAdvert.getOTPLayer()->getPage(),
                    datagram);

        // Last page?
        if (folioMap.checkAllPages(
                    systemAdvert.getRootLayer()->getCID(),
                    PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM,
                    systemAdvert.getOTPLayer()->getFolio(),
                    systemAdvert.getOTPLayer()->getLastPage()))
        {
            // Process all pages
            MESSAGES::OTPSystemAdvertisementMessage::list_t list;
            for (auto datagram : folioMap.getDatagrams(systemAdvert.getRootLayer()->getCID(),
                                                       PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM,
                                                       systemAdvert.getOTPLayer()->getFolio()))
            {
                MESSAGES::OTPSystemAdvertisementMessage::Message systemAdvert(datagram);
                list.append(systemAdvert.getSystemAdvertisementLayer()->getList());
            }

            // Add new systems
            for (auto system : list)
            {
                otpNetwork->addSystem(
                        systemAdvert.getRootLayer()->getCID(),
                        system);
            }

            // Prune old
            for (auto system : otpNetwork->getSystemList(systemAdvert.getRootLayer()->getCID()))
            {
                if (!list.contains(system))
                    otpNetwork->removeSystem(
                            systemAdvert.getRootLayer()->getCID(),
                            system);
            }
        }
        return;
    }
}

void Consumer::sendOTPModuleAdvertisementMessage()
{
    using namespace ACN::OTP::MESSAGES::OTPModuleAdvertisementMessage;

    // Get list of supported modules
    list_t list = ACN::OTP::MODULES::getSupportedModules();

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
    for (int n = 0; n < folioMessages.count(); n++)
    {
        auto datagram = folioMessages[n]->toQNetworkDatagram(
                            sequenceMap[getConsumerCID()].getNextSequence(PDU::VECTOR_OTP_ADVERTISEMENT_MODULE),
                            ModuleAdvertisementMessage_Folio++,
                            n,
                            folioMessages.count() - 1);

        if (SocketManager::getInstance(iface)->writeDatagram(datagram) == datagram.data().size())
            qDebug() << this << "- OTP Module Advertisement Message Request Sent To" << datagram.destinationAddress();
        else
            qDebug() << this << "- OTP Module Advertisement Message Request Failed";
    }
}

void Consumer::sendOTPNameAdvertisementMessage()
{
    // Name Advertisement Message
    using namespace ACN::OTP::MESSAGES::OTPNameAdvertisementMessage;
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

    auto datagram = nameAdvertisementMessage.toQNetworkDatagram(
                sequenceMap[CID].getNextSequence(PDU::VECTOR_OTP_ADVERTISEMENT_NAME),
                NameAdvertisementMessage_Folio++,0,0);
    if (SocketManager::getInstance(iface)->writeDatagram(datagram) == datagram.data().size())
        qDebug() << this << "- OTP Name Advertisement Message Request Sent";
    else
        qDebug() << this << "- OTP Name Advertisement Message Request Failed";
}

void Consumer::sendOTPSystemAdvertisementMessage()
{
    // System Advertisement Message
    using namespace ACN::OTP::MESSAGES::OTPSystemAdvertisementMessage;
    Message systemAdvertisementMessage(
                mode_e::Consumer,
                getConsumerCID(),
                name,
                otpNetwork->getSystemList(getConsumerCID()),
                this);

    if (!systemAdvertisementMessage.isValid())
    {
        qDebug() << this << "- OTP System Advertisement Message Request Not Valid";
        return;
    }

    auto datagram = systemAdvertisementMessage.toQNetworkDatagram(
                sequenceMap[CID].getNextSequence(PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM),
                SystemAdvertisementMessage_Folio++,0,0);
    if (SocketManager::getInstance(iface)->writeDatagram(datagram) == datagram.data().size())
        qDebug() << this << "- OTP System Advertisement Message Request Sent";
    else
        qDebug() << this << "- OTP System Advertisement Message Request Failed";
}
