/*
    OTPLib
    A QT interface for E1.59  (Entertainment  Technology  Object  Transform  Protocol  (OTP))
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
#ifndef MODULES_CONST_HPP
#define MODULES_CONST_HPP
#include "../../const.hpp"
#include "modules_types.hpp"
#include "../messages/message_types.hpp"

namespace ACN::OTP::MODULES {
    /* Section 16 Standard Modules */
    namespace STANDARD {
        enum : MODULES::moduleNumber_t
        {
            POSITION = 0x0001,
            POSITION_VELOCITY_ACCELERATION = 0x0002,
            ROTATION = 0x0003,
            ROTATION_VELOCITY_ACCELERATION = 0x0004,
            ORIENTATION = 0x0005,
            ORIENTATION_VELOCITY_ACCELERATION = 0x0006
        };

        static QMap<moduleNumber_t, module_t> modules{
            {POSITION, {{QObject::tr("ESTA"), QObject::tr("Position")},{ESTA_MANUFACTURER_ID, POSITION}}},
            {POSITION_VELOCITY_ACCELERATION, {{QObject::tr("ESTA"), QObject::tr("Position Velocity/Acceleration")},{ESTA_MANUFACTURER_ID, POSITION_VELOCITY_ACCELERATION}}},
            {ROTATION, {{QObject::tr("ESTA"), QObject::tr("Rotation")}, {ESTA_MANUFACTURER_ID, ROTATION}}},
            {ROTATION_VELOCITY_ACCELERATION, {{QObject::tr("ESTA"), QObject::tr("Rotation Velocity/Acceleration")}, {ESTA_MANUFACTURER_ID, ROTATION_VELOCITY_ACCELERATION}}},
            {ORIENTATION, {{QObject::tr("ESTA"), QObject::tr("Orientation")}, {ESTA_MANUFACTURER_ID, ORIENTATION}}},
            {ORIENTATION_VELOCITY_ACCELERATION, {{QObject::tr("ESTA"), QObject::tr("Orientation Velocity/Acceleration")}, {ESTA_MANUFACTURER_ID, ORIENTATION_VELOCITY_ACCELERATION}}},
        };

        namespace VALUES {
            typedef enum {
                POSITION,
                POSITION_VELOCITY,
                POSITION_ACCELERATION,
                ROTATION,
                ROTATION_VELOCITY,
                ROTATION_ACCELERATION,
            } moduleValue_t;

            namespace UNITS {
                constexpr static const QChar superscript2 = char16_t(0x00b2);
                constexpr static const QChar degrees = char16_t(0x00B0);
                constexpr static const QChar micro = char16_t(0x03BC);

                static QMap<moduleValue_t, QString> baseUnits{
                    {POSITION, QString("m")},
                    {POSITION_VELOCITY, QString("m/sec")},
                    {POSITION_ACCELERATION, QString("m/sec%1").arg(superscript2)},
                    {ROTATION, QString("%1%2").arg(micro).arg(degrees)},
                    {ROTATION_VELOCITY, QString("%1%2/sec").arg(micro).arg(degrees)},
                    {ROTATION_ACCELERATION, QString("%1%2/sec%3").arg(micro).arg(degrees).arg(superscript2)}
                };
                QString getUnitString(moduleValue_t value);

                static QMap<PositionModule_t::scale_t, QString> scale{
                    {PositionModule_t::mm, QString("m")},
                    {PositionModule_t::um, QString("%1").arg(micro)}
                };
                QString getScaleString(PositionModule_t::scale_t value);
            }

            namespace RANGES {
                static QMap<moduleValue_t, range_t> ranges{
                    {POSITION, {STANDARD::PositionModule_t::location_t(-2147483648),PositionModule_t::location_t(2147483647)}},
                    {POSITION_VELOCITY, {STANDARD::PositionVelAccModule_t::velocity_t(-2147483648),STANDARD::PositionVelAccModule_t::velocity_t(2147483647)}},
                    {POSITION_ACCELERATION, {STANDARD::PositionVelAccModule_t::acceleration_t(-2147483648),STANDARD::PositionVelAccModule_t::acceleration_t(2147483647)}},

                    {ROTATION, {STANDARD::RotationModule_t::rotation_t(0),STANDARD::RotationModule_t::rotation_t(359999999)}},
                    {ROTATION_VELOCITY, {STANDARD::RotationVelAccModule_t::velocity_t(-360000000),STANDARD::RotationVelAccModule_t::velocity_t(360000000)}},
                    {ROTATION_ACCELERATION, {STANDARD::RotationVelAccModule_t::acceleration_t(-360000000),STANDARD::RotationVelAccModule_t::acceleration_t(360000000)}}
                };
                range_t getRange(moduleValue_t value);
            }
        }
    }

    MESSAGES::OTPModuleAdvertisementMessage::list_t const getSupportedModules();
    module_t::moduleDescription_t const getModuleDescription(PDU::OTPModuleLayer::vector_t vector);
}

#endif // MODULES_CONST_HPP
