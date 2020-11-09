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
        cid_t CID,
        name_t name,
        std::chrono::milliseconds transformRate,
        QObject *parent) :
    Component(
        iface,
        transport,
        CID,
        name,
        parent)
{    
    // Group Signals
    connect(otpNetwork.get(), &Container::newGroup,
        [this](cid_t cid, system_t system, group_t group) {
            if (cid == getLocalCID()) emit newLocalGroup(system, group);
        });
    connect(otpNetwork.get(), &Container::removedGroup,
        [this](cid_t cid, system_t system, group_t group) {
            if (cid == getLocalCID()) emit removedLocalGroup(system, group);
        });

    // Point Signals
    connect(otpNetwork.get(), &Container::newPoint,
        [this](cid_t cid, system_t system, group_t group, point_t point) {
            if (cid == getLocalCID()) emit newLocalPoint(system, group, point);
        });
    connect(otpNetwork.get(), &Container::removedPoint,
        [this](cid_t cid, system_t system, group_t group, point_t point) {
            if (cid == getLocalCID()) emit removedLocalPoint(system, group, point);
        });

    setupListener();
    auto startSenderTimeout = new QTimer;
    startSenderTimeout->setSingleShot(true);
    connect(startSenderTimeout, &QTimer::timeout, this, [this, transformRate]() {this->setupSender(transformRate); });
    connect(startSenderTimeout, SIGNAL(timeout()), startSenderTimeout, SLOT(deleteLater()));
    startSenderTimeout->start(OTP_ADVERTISEMENT_STARTUP_WAIT);
}

/* Local Groups */
QList<group_t> Producer::getLocalGroups(system_t system) const
{
    return otpNetwork->getGroupList(getLocalCID(), system);
}
void Producer::addLocalGroup(system_t system, group_t group)
{
    otpNetwork->addGroup(getLocalCID(), system, group);
}
void Producer::removeLocalGroup(system_t system, group_t group)
{
    otpNetwork->removeGroup(getLocalCID(), system, group);
}

/* Local Points */
QList<point_t> Producer::getLocalPoints(system_t system, group_t group) const
{
    return otpNetwork->getPointList(getLocalCID(), system, group);
}
void Producer::addLocalPoint(address_t address, priority_t priority)
{
    otpNetwork->addPoint(getLocalCID(), address, priority);
    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.referenceFrame.setSystem(address.system, 0);
    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.referenceFrame.setGroup(address.group, 0);
    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.referenceFrame.setPoint(address.point, 0);
}
void Producer::removeLocalPoint(address_t address)
{
    otpNetwork->removePoint(getLocalCID(), address);
}
QString Producer::getLocalPointName(address_t address) const
{
    if (!getLocalPoints(address.system, address.group).contains(address.point)) return QString();
    return otpNetwork->PointDetails(getLocalCID(), address)->getName();
}
void Producer::setLocalPointName(address_t address, QString name)
{
    if (!getLocalPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(getLocalCID(), address)->setName(name);
    emit updatedLocalPointName(address);
}
priority_t Producer::getLocalPointPriority(address_t address) const
{
    if (!getLocalPoints(address.system, address.group).contains(address.point)) return priority_t();
    return otpNetwork->PointDetails(getLocalCID(), address)->getPriority();
}
void Producer::setLocalPointPriority(address_t address, priority_t priority)
{
    if (!getLocalPoints(address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(getLocalCID(), address)->setPriority(priority);
    emit updatedLocalPointPriority(address);
}

/* Local Addesses */
QList<address_t> Producer::getLocalAddresses()
{
    QList<address_t> ret;
    for (auto system : getLocalSystems())
        ret.append(getAddresses(system));

    return ret;
}
QList<address_t> Producer::getLocalAddresses(system_t system)
{
    QList<address_t> ret;
    for (auto group : getLocalGroups(system))
        ret.append(getAddresses(system, group));

    return ret;
}
QList<address_t> Producer::getLocalAddresses(system_t system, group_t group)
{
    QList<address_t> ret;
    for (auto point : getLocalPoints(system, group))
        ret.append(address_t(system, group, point));

    return ret;
}

/* Standard Modules - Position */
Producer::PositionValue_t Producer::getLocalPosition(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::PositionValue_t ret;
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.position.getPosition(axis);
    ret.scale = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.position.getScaling();
    ret.unit = getUnitString(ret.scale, VALUES::POSITION);
    ret.timestamp = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.position.getTimestamp();
    return ret;
}

void Producer::setLocalPosition(address_t address, axis_t axis, PositionValue_t position)
{
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point)) return;
    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.position.setPosition(
                axis, position.value, position.timestamp);
    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.position.setScaling(
                position.scale);

    emit updatedPosition(address, axis);
}

/* Standard Modules - Position Velocity/Acceleration */
Producer::PositionVelocity_t Producer::getLocalPositionVelocity(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::PositionVelocity_t ret;
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.positionVelAcc.getVelocity(axis);
    ret.unit = getUnitString(VALUES::POSITION_VELOCITY);
    ret.timestamp = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.positionVelAcc.getTimestamp();
    return ret;
}

void Producer::setLocalPositionVelocity(address_t address, axis_t axis, PositionVelocity_t positionVel)
{
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.positionVelAcc.setVelocity(
                axis, positionVel.value, positionVel.timestamp);

    emit updatedPositionVelocity(address, axis);
}

Producer::PositionAcceleration_t Producer::getLocalPositionAcceleration(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::PositionAcceleration_t ret;
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.positionVelAcc.getAcceleration(axis);
    ret.unit = getUnitString(VALUES::POSITION_ACCELERATION);
    ret.timestamp = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.positionVelAcc.getTimestamp();
    return ret;
}

void Producer::setLocalPositionAcceleration(address_t address, axis_t axis, PositionAcceleration_t positionAccel)
{
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.positionVelAcc.setAcceleration(
                axis, positionAccel.value, positionAccel.timestamp);

    emit updatedPositionAcceleration(address, axis);
}

/* Standard Modules - Rotation */
Producer::RotationValue_t Producer::getLocalRotation(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::RotationValue_t ret;
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.rotation.getRotation(axis);
    ret.unit = getUnitString(VALUES::ROTATION);
    ret.timestamp = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.rotation.getTimestamp();
    return ret;
}

void Producer::setLocalRotation(address_t address, axis_t axis, RotationValue_t rotation)
{
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.rotation.setRotation(
                axis, rotation.value, rotation.timestamp);

    emit updatedRotation(address, axis);
}

/* Standard Modules - Position Velocity/Acceleration */
Producer::RotationVelocity_t Producer::getLocalRotationVelocity( address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::RotationVelocity_t ret;
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.rotationVelAcc.getVelocity(axis);
    ret.unit = getUnitString(VALUES::ROTATION_VELOCITY);
    ret.timestamp = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.rotationVelAcc.getTimestamp();
    return ret;
}

void Producer::setLocalRotationVelocity(address_t address, axis_t axis, RotationVelocity_t rotationVel)
{
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.rotationVelAcc.setVelocity(
                axis, rotationVel.value, rotationVel.timestamp);

    emit updatedRotationVelocity(address, axis);
}

Producer::RotationAcceleration_t Producer::getLocalRotationAcceleration(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::RotationAcceleration_t ret;
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.rotationVelAcc.getAcceleration(axis);
    ret.unit = getUnitString(VALUES::ROTATION_VELOCITY);
    ret.timestamp = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.rotationVelAcc.getTimestamp();
    return ret;
}

void Producer::setLocalRotationAcceleration(address_t address, axis_t axis, RotationAcceleration_t rotationAccel)
{
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.rotationVelAcc.setAcceleration(
                axis, rotationAccel.value, rotationAccel.timestamp);

    emit updatedRotationAcceleration(address, axis);
}

Producer::Scale_t Producer::getLocalScale(address_t address, axis_t axis) const
{
    using namespace MODULES::STANDARD;
    Producer::Scale_t ret;
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point))
        return ret;

    ret.value = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.scale.getScale(axis);
    ret.timestamp = otpNetwork->PointDetails(getLocalCID(), address)->standardModules.scale.getTimestamp();
    return ret;
}

void Producer::setLocalScale(address_t address, axis_t axis, Scale_t scale)
{
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point)) return;

    otpNetwork->PointDetails(getLocalCID(), address)->standardModules.scale.setScale(
                axis, scale.value, scale.timestamp);

    emit updatedScale(address, axis);
}

Producer::ReferenceFrame_t Producer::getLocalReferenceFrame(address_t address) const
{
    using namespace MODULES::STANDARD;
    Producer::ReferenceFrame_t ret;
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point))
        return ret;

    auto module = &otpNetwork->PointDetails(getLocalCID(), address)->standardModules.referenceFrame;
    ret.value = {module->getSystem(), module->getGroup(), module->getPoint()};
    ret.timestamp = module->getTimestamp();
    return ret;
}

void Producer::setLocalReferenceFrame(address_t address, ReferenceFrame_t referenceFrame)
{
    if (!getPoints(getLocalCID(), address.system, address.group).contains(address.point)) return;

    auto module = &otpNetwork->PointDetails(getLocalCID(), address)->standardModules.referenceFrame;
    module->setSystem(referenceFrame.value.system, referenceFrame.timestamp);
    module->setGroup(referenceFrame.value.group, referenceFrame.timestamp);
    module->setPoint(referenceFrame.value.point, referenceFrame.timestamp);
    emit updatedReferenceFrame(address);
}

void Producer::setupSender(std::chrono::milliseconds transformRate)
{
    qDebug() << this << "- Starting OTP Transform Messages" << iface.name();
    connect(&transformMsgTimer, &QTimer::timeout, [this]() {
        for (auto system : getLocalSystems())
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
            if (!folioMap.checkSequence(
                        cid,
                        PDU::VECTOR_OTP_ADVERTISEMENT_MODULE,
                        moduleAdvert.getOTPLayer()->getFolio()))
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
            if (!folioMap.checkSequence(
                        cid,
                        PDU::VECTOR_OTP_ADVERTISEMENT_NAME,
                        nameAdvert.getOTPLayer()->getFolio()))
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
            if (!folioMap.checkSequence(
                        cid,
                        PDU::VECTOR_OTP_ADVERTISEMENT_SYSTEM,
                        systemAdvert.getOTPLayer()->getFolio()))
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
    for (auto address : getLocalAddresses())
    {
        item_t item(address.system, address.group, address.point,
                    getLocalPointName(address));
        list.append(item);
    }

    // Generate messages
    QVector<std::shared_ptr<Message>> folioMessages;
    while (list.count()) {
        folioMessages.append(
                    std::make_shared<Message>(
                        mode_e::Producer,
                        getLocalCID(),
                        getLocalName(),
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
                    folio,
                    page,
                    lastPage);
        if (sockets.value(destinationAddr.protocol())->writeDatagram(datagram))
            qDebug() << this << "- OTP Name Advertisement Message Response Sent to" << destinationAddr;
        else
            qDebug() << this << "- OTP Name Advertisement Message Response Failed";
    }
}

void Producer::sendOTPSystemAdvertisementMessage(QHostAddress destinationAddr, MESSAGES::OTPNameAdvertisementMessage::folio_t folio)
{
    using namespace OTP::MESSAGES::OTPSystemAdvertisementMessage;

    // Get list of systems
    list_t list = otpNetwork->getSystemList(getLocalCID());

    // Generate messages
    QVector<std::shared_ptr<Message>> folioMessages;
    while (list.count()) {
        folioMessages.append(
                    std::make_shared<Message>(
                        mode_e::Producer,
                        getLocalCID(),
                        getLocalName(),
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
                    folio,
                    page,
                    lastPage);

        if (sockets.value(destinationAddr.protocol())->writeDatagram(datagram))
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
//        if (otpNetwork->getSystemList(cid).contains(system))
//        {
            auto component = otpNetwork->getComponent(cid);
            for (auto module : component.getModuleList())
            {
                if (!requestedModules.contains(module))
                    requestedModules.append(module);
            }
//        }
    }
    if (requestedModules.isEmpty()) return;

    // Get each requested module
    QVector<Message::addModule_t> folioModuleData;
    for (auto address: getLocalAddresses(system))
    {
        auto pointDetails = otpNetwork->PointDetails(getLocalCID(), address);
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
        folioMessages.append(std::make_shared<Message>(getLocalCID(), getLocalName(), system, true, this));

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
                    folio,
                    page,
                    lastPage);

        if (!SocketManager::writeDatagrams(iface, datagrams))
            qDebug() << this << "- OTP Transform Message Failed";
    }
}
