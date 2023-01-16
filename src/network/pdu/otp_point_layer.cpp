/**
 * @file        otp_point_layer.cpp
 * @brief       Point PDU Layer
 * @details     Part of OTPLib - A QT interface for E1.59
 * @authors     Marcus Birkin
 * @copyright   Copyright (C) 2019 Marcus Birkin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "otp_point_layer.hpp"

using namespace OTP::PDU;
using namespace OTP::PDU::OTPPointLayer;

Layer::Layer(
        pduLength_t PDULength,
        priority_t Priority,
        group_t Group,
        point_t Point,
        timestamp_t Timestamp,
        QObject *parent) :
    QObject(parent),
    Vector(VECTOR),
    PDULength(PDULength),
    Priority(Priority),
    Group(Group),
    Point(Point),
    Timestamp(Timestamp),
    Options(OPTIONS),
    Reserved(RESERVED)
{}

Layer::Layer(
        OTP::PDU::PDUByteArray layer,
        QObject *parent) :
    QObject(parent),
    Vector(0),
    PDULength(0),
    Priority(0),
    Group(0),
    Point(0),
    Options(0),
    Reserved(0)
{
    if (layer.size() != Layer().toPDUByteArray().size())
        return;

    layer >> Vector
        >> PDULength
        >> Priority
        >> Group
        >> Point
        >> Timestamp
        >> Options
        >> Reserved;
}

bool Layer::isValid()
{
    if (Vector != VECTOR) return false;
    if (PDULength <= toPDUByteArray().size() - LENGTHOFFSET) return false;
    if (!Priority.isValid()) return false;
    if (!Group.isValid()) return false;
    if (!Point.isValid()) return false;
    if (Timestamp == 0) return false;
    return true;
}

OTP::PDU::PDUByteArray Layer::toPDUByteArray()
{
    PDUByteArray ret;
    return ret << Vector
        << PDULength
        << Priority
        << Group
        << Point
        << Timestamp
        << Options
        << Reserved;
}

void Layer::fromPDUByteArray(OTP::PDU::PDUByteArray layer)
{
    Vector = 0;
    PDULength = 0;
    Priority = 0;
    Group = 0;
    Point = 0;
    Timestamp = 0;
    Options = 0;
    Reserved = 0;

    if (layer.size() != Layer().toPDUByteArray().size())
        return;

    layer >> Vector
        >> PDULength
        >> Priority
        >> Group
        >> Point
        >> Timestamp
        >> Options
        >> Reserved;
}
