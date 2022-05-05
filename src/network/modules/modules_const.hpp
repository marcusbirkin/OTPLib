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

namespace OTP::MODULES {
    /* Section 16 Standard Modules */
    namespace STANDARD {
        constexpr static const MODULES::moduleNumber_t POSITION = 0x0001;
        constexpr static const MODULES::moduleNumber_t POSITION_VELOCITY_ACCELERATION = 0x0002;
        constexpr static const MODULES::moduleNumber_t ROTATION = 0x0003;
        constexpr static const MODULES::moduleNumber_t ROTATION_VELOCITY_ACCELERATION = 0x0004;
        constexpr static const MODULES::moduleNumber_t SCALE = 0x0005;
        constexpr static const MODULES::moduleNumber_t REFERENCE_FRAME = 0x0006;

        static const QMap<moduleNumber_t, module_t> modules{
            {POSITION, {{QObject::tr("ESTA"), QObject::tr("Position")},{ESTA_MANUFACTURER_ID, POSITION}}},
            {POSITION_VELOCITY_ACCELERATION, {{QObject::tr("ESTA"), QObject::tr("Position Velocity/Acceleration")},{ESTA_MANUFACTURER_ID, POSITION_VELOCITY_ACCELERATION}}},
            {ROTATION, {{QObject::tr("ESTA"), QObject::tr("Rotation")}, {ESTA_MANUFACTURER_ID, ROTATION}}},
            {ROTATION_VELOCITY_ACCELERATION, {{QObject::tr("ESTA"), QObject::tr("Rotation Velocity/Acceleration")}, {ESTA_MANUFACTURER_ID, ROTATION_VELOCITY_ACCELERATION}}},
            {SCALE, {{QObject::tr("ESTA"), QObject::tr("Scale")}, {ESTA_MANUFACTURER_ID, SCALE}}},
            {REFERENCE_FRAME, {{QObject::tr("ESTA"), QObject::tr("Reference Frame")}, {ESTA_MANUFACTURER_ID, REFERENCE_FRAME}}},
        };

        namespace VALUES {
            typedef enum {
                POSITION,
                POSITION_VELOCITY,
                POSITION_ACCELERATION,
                ROTATION,
                ROTATION_VELOCITY,
                ROTATION_ACCELERATION,
                SCALE,
                REFERENCE_FRAME
            } moduleValue_t;

            namespace UNITS {
                namespace unicode {
                    constexpr static const QChar superscript2 = char16_t(0x00b2);
                    constexpr static const QChar degrees = char16_t(0x00B0);
                    constexpr static const QChar micro = char16_t(0x03BC);

                    static const QMap<moduleValue_t, QString> baseUnits{
                        {POSITION, QString("m")},
                        {POSITION_VELOCITY, QString("%1m/sec")
                                    .arg(micro)},
                        {POSITION_ACCELERATION, QString("%1m/sec%2")
                                    .arg(micro)
                                    .arg(superscript2)},
                        {ROTATION, QString("%1%2")
                                    .arg(micro)
                                    .arg(degrees)},
                        {ROTATION_VELOCITY, QString("%1%2/sec")
                                    .arg(micro)
                                    .arg(degrees)},
                        {ROTATION_ACCELERATION, QString("%1%2/sec%3")
                                    .arg(micro)
                                    .arg(degrees)
                                    .arg(superscript2)}
                    };

                    static QMap<PositionModule_t::scale_t, QString> scale{
                        {PositionModule_t::mm, QString("m")},
                        {PositionModule_t::um, QString("%1").arg(micro)}
                    };
                }
                namespace html {
                    constexpr static const std::string_view superscript2 = "&sup2;";
                    constexpr static const std::string_view degrees = "&deg;";
                    constexpr static const std::string_view micro = "&mu;";

                    static QMap<moduleValue_t, QString> baseUnits{
                        {POSITION, QString("m")},
                        {POSITION_VELOCITY, QString("%1m/sec")
                                    .arg(std::string(micro).c_str())},
                        {POSITION_ACCELERATION, QString("%1m/sec%2")
                                    .arg(std::string(micro).c_str())
                                    .arg(std::string(superscript2).c_str())},
                        {ROTATION, QString("%1%2")
                                    .arg(std::string(micro).c_str())
                                    .arg(std::string(degrees).c_str())},
                        {ROTATION_VELOCITY, QString("%1%2/sec")
                                    .arg(std::string(micro).c_str())
                                    .arg(std::string(degrees).c_str())},
                        {ROTATION_ACCELERATION, QString("%1%2/sec%3")
                                    .arg(std::string(micro).c_str())
                                    .arg(std::string(degrees).c_str())
                                    .arg(std::string(superscript2).c_str())}
                    };

                    static QMap<PositionModule_t::scale_t, QString> scale{
                        {PositionModule_t::mm, QString("m")},
                        {PositionModule_t::um, QString("%1").arg(std::string(micro).c_str())}
                    };
                }

                QString OTP_LIB_EXPORT getUnitString(moduleValue_t value, bool html = false);
                QString OTP_LIB_EXPORT getScaleString(PositionModule_t::scale_t value, bool html = false);
            }

            namespace RANGES {
                static QMap<moduleValue_t, range_t> ranges{
                    {POSITION, {STANDARD::PositionModule_t::position_t(-2147483648LL),PositionModule_t::position_t(2147483647)}},
                    {POSITION_VELOCITY, {STANDARD::PositionVelAccModule_t::velocity_t(-2147483648LL),STANDARD::PositionVelAccModule_t::velocity_t(2147483647)}},
                    {POSITION_ACCELERATION, {STANDARD::PositionVelAccModule_t::acceleration_t(-2147483648LL),STANDARD::PositionVelAccModule_t::acceleration_t(2147483647)}},

                    {ROTATION, {STANDARD::RotationModule_t::rotation_t(0),STANDARD::RotationModule_t::rotation_t(359999999)}},
                    {ROTATION_VELOCITY, {STANDARD::RotationVelAccModule_t::velocity_t(-360000000),STANDARD::RotationVelAccModule_t::velocity_t(360000000)}},
                    {ROTATION_ACCELERATION, {STANDARD::RotationVelAccModule_t::acceleration_t(-360000000),STANDARD::RotationVelAccModule_t::acceleration_t(360000000)}},

                    {SCALE, {STANDARD::ScaleModule_t::scale_t(-2147483648LL),STANDARD::ScaleModule_t::scale_t(2147483647)}}
                };
                range_t OTP_LIB_EXPORT getRange(moduleValue_t value);
            }
        }
    }

    MESSAGES::OTPModuleAdvertisementMessage::list_t const getSupportedModules();
    module_t::moduleDescription_t const OTP_LIB_EXPORT getModuleDescription(PDU::OTPModuleLayer::ident_t ident);
}

#endif // MODULES_CONST_HPP
