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
#ifndef OTP_POINT_LAYER_HPP
#define OTP_POINT_LAYER_HPP

#include <QObject>
#include <QDateTime>
#include "pdu_types.hpp"
#include "pdu_const.hpp"

namespace OTP::PDU::OTPPointLayer
{

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(
            pduLength_t PDULength = 0,
            priority_t Priority = 100,
            group_t Group = 0,
            point_t Point = 0,
            timestamp_t Timestamp = static_cast<timestamp_t>(QDateTime::currentMSecsSinceEpoch() * 1000),
            QObject *parent = nullptr);
    explicit Layer(
            PDUByteArray layer,
            QObject *parent = nullptr);
    bool isValid();
    PDUByteArray toPDUByteArray();
    void fromPDUByteArray(PDUByteArray layer);

    const vector_t &getVector() { return Vector; }
    const pduLength_t &getPDULength() const { return PDULength; }
    void setPDULength(pduLength_t value) { PDULength = value; }
    const priority_t &getPriority() { return Priority; }
    void setPriority(priority_t value) { Priority = value; }
    const group_t &getGroup() { return Group; }
    void setGroup(group_t value) { Group = value; }
    const point_t &getPoint() { return Point; }
    void setPoint(point_t value) { Point = value; }
    const timestamp_t &getTimestamp() { return Timestamp; }
    void setTimestamp(timestamp_t value) { Timestamp = value; }
    const options_t &getOptions() { return Options; }
    const reserved_t &getReserved() { return Reserved; }

private:
    vector_t Vector;
    pduLength_t PDULength;
    priority_t Priority;
    group_t Group;
    point_t Point;
    timestamp_t Timestamp;
    options_t Options;
    reserved_t Reserved;
};

}; // namespace

#endif // OTP_POINT_LAYER_HPP
