/**
 * @file        modules_const.hpp
 * @brief       Constants used relating to modules in the OTP standard
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
#ifndef MODULES_CONST_HPP
#define MODULES_CONST_HPP
#include "../../const.hpp"
#include "modules_types.hpp"
#include "../messages/message_types.hpp"

namespace OTP::MODULES {
    /**
     * @brief ANSI E1.59:2021 Section 16 Standard Modules
     * 
     */
    namespace STANDARD {
        /**
         * @internal
         * @name ANSI E1.59:2021 Table 16-1: Standard Module Types
         * 
         * @{
         */ 
        constexpr static const MODULES::moduleNumber_t POSITION = 0x0001;
        constexpr static const MODULES::moduleNumber_t POSITION_VELOCITY_ACCELERATION = 0x0002;
        constexpr static const MODULES::moduleNumber_t ROTATION = 0x0003;
        constexpr static const MODULES::moduleNumber_t ROTATION_VELOCITY_ACCELERATION = 0x0004;
        constexpr static const MODULES::moduleNumber_t SCALE = 0x0005;
        constexpr static const MODULES::moduleNumber_t REFERENCE_FRAME = 0x0006;
        
        /** @internal @}*/ // ANSI E1.59:2021 Table 16-1: Standard Module Types

        /**
         * @internal
         * @brief Lookup table for getModuleDescription()
         * 
         */
        inline const QMap<moduleNumber_t, module_t> modules{
            {POSITION, {{QObject::tr("ESTA"), QObject::tr("Position")},{ESTA_MANUFACTURER_ID, POSITION}}},
            {POSITION_VELOCITY_ACCELERATION, {{QObject::tr("ESTA"), QObject::tr("Position Velocity/Acceleration")},{ESTA_MANUFACTURER_ID, POSITION_VELOCITY_ACCELERATION}}},
            {ROTATION, {{QObject::tr("ESTA"), QObject::tr("Rotation")}, {ESTA_MANUFACTURER_ID, ROTATION}}},
            {ROTATION_VELOCITY_ACCELERATION, {{QObject::tr("ESTA"), QObject::tr("Rotation Velocity/Acceleration")}, {ESTA_MANUFACTURER_ID, ROTATION_VELOCITY_ACCELERATION}}},
            {SCALE, {{QObject::tr("ESTA"), QObject::tr("Scale")}, {ESTA_MANUFACTURER_ID, SCALE}}},
            {REFERENCE_FRAME, {{QObject::tr("ESTA"), QObject::tr("Reference Frame")}, {ESTA_MANUFACTURER_ID, REFERENCE_FRAME}}},
        };

        /**
         * @brief Values relating to Standard Modules
         * 
         */
        namespace VALUES {
            /**
             * @brief Standard module values
             * @details These are expanded beyond the standard modules
             * e.g. Position Velocity/Acceleration is expanded into "Position Velocity" and "Position Acceleration"
             * 
             */
            typedef enum {
                POSITION, /**< Position */
                POSITION_VELOCITY, /**< Position Velocity */
                POSITION_ACCELERATION, /**< Position Acceleration*/
                ROTATION, /**< Rotation */
                ROTATION_VELOCITY, /**< Rotation Velocity */
                ROTATION_ACCELERATION, /**< Rotation Acceleration*/
                SCALE, /**< Scale */
                REFERENCE_FRAME /**< Reference Frame */
            } moduleValue_t;

            /**
             * @brief Units relating to Standard Modules
             * 
             */
            namespace UNITS {
                namespace unicode {
                    /** Unicode Power of 2 (&sup2;) character */
                    inline const QChar superscript2 = char16_t(0x00b2);

                    /** Unicode Degrees (&deg;) character */
                    inline const QChar degrees = char16_t(0x00B0);

                    /** Unicode Micro (&mu;) character */
                    inline const QChar micro = char16_t(0x03BC);

                    /**
                     * @internal
                     * @brief Lookup table for getUnitString()
                     * 
                     */
                    inline const QMap<moduleValue_t, QString> baseUnits{
                        {POSITION, QString("m")},
                        {POSITION_VELOCITY, QString("%1m/sec")
                                    .arg(micro)},
                        {POSITION_ACCELERATION, QString("%1m/sec%2")
                                    .arg(micro, superscript2)},
                        {ROTATION, QString("%1%2")
                                    .arg(micro, degrees)},
                        {ROTATION_VELOCITY, QString("%1%2/sec")
                                    .arg(micro, degrees)},
                        {ROTATION_ACCELERATION, QString("%1%2/sec%3")
                                    .arg(micro, degrees, superscript2)},
                        // Unitless
                        {SCALE, QString("")},
                        {REFERENCE_FRAME, QString("")}
                    };

                    /**
                     * @brief Lookup table for scales type
                     * 
                     */
                    inline const QMap<PositionModule_t::scale_t, QString> scale{
                        {PositionModule_t::mm, QString("m")},
                        {PositionModule_t::um, QString("%1").arg(micro)}
                    };
                }
                namespace html {
                    /** HTML Power of 2 (&sup2;) character */
                    inline const std::string_view superscript2 = "&sup2;";

                    /** HTML Degrees (&deg;) character */
                    inline const std::string_view degrees = "&deg;";

                    /** HTML Micro (&mu;) character */
                    inline const std::string_view micro = "&mu;";

                    /**
                     * @internal
                     * @brief Lookup table for getUnitString()
                     * 
                     */
                    inline const QMap<moduleValue_t, QString> baseUnits{
                        {POSITION, QString("m")},
                        {POSITION_VELOCITY, QString("%1m/sec")
                                    .arg(std::string(micro).c_str())},
                        {POSITION_ACCELERATION, QString("%1m/sec%2")
                                    .arg(std::string(micro).c_str(),
                                         std::string(superscript2).c_str())},
                        {ROTATION, QString("%1%2")
                                    .arg(std::string(micro).c_str(),
                                        std::string(degrees).c_str())},
                        {ROTATION_VELOCITY, QString("%1%2/sec")
                                    .arg(std::string(micro).c_str(),
                                        std::string(degrees).c_str())},
                        {ROTATION_ACCELERATION, QString("%1%2/sec%3")
                                    .arg(std::string(micro).c_str(),
                                        std::string(degrees).c_str(),
                                        std::string(superscript2).c_str())},
                        // Unitless
                        {SCALE, QString("")},
                        {REFERENCE_FRAME, QString("")}
                    };

                    /**
                     * @internal
                     * @brief Lookup table for getScaleString()
                     * 
                     */
                    inline const QMap<PositionModule_t::scale_t, QString> scale{
                        {PositionModule_t::mm, QString("m")},
                        {PositionModule_t::um, QString("%1").arg(std::string(micro).c_str())}
                    };
                }

                /**
                 * @brief Get the unit string value of a module value
                 * 
                 * @param value Module value
                 * @param html Format for HTML?
                 * @return QString 
                 */
                QString OTP_LIB_EXPORT getUnitString(moduleValue_t value, bool html = false);

                /**
                 * @brief Get the scale string value of a scale value
                 * 
                 * @param value Scale value
                 * @param html Format for HTML?
                 * @return QString 
                 */
                QString OTP_LIB_EXPORT getScaleString(PositionModule_t::scale_t value, bool html = false);
            }

            /**
             * @brief Valid Ranges relating to Standard Modules
             * 
             */
            namespace RANGES {
                /**
                 * @internal
                 * @brief Look up table used by OTP::MODULES::STANDARD::VALUES::getRange()
                 * @details Contains valid ranges for Standard module values
                 * 
                 */
                inline const QMap<moduleValue_t, range_t> ranges{
                    {POSITION, {STANDARD::PositionModule_t::position_t(-2147483648LL),PositionModule_t::position_t(2147483647)}},
                    {POSITION_VELOCITY, {STANDARD::PositionVelAccModule_t::velocity_t(-2147483648LL),STANDARD::PositionVelAccModule_t::velocity_t(2147483647)}},
                    {POSITION_ACCELERATION, {STANDARD::PositionVelAccModule_t::acceleration_t(-2147483648LL),STANDARD::PositionVelAccModule_t::acceleration_t(2147483647)}},

                    {ROTATION, {STANDARD::RotationModule_t::rotation_t(0),STANDARD::RotationModule_t::rotation_t(359999999)}},
                    {ROTATION_VELOCITY, {STANDARD::RotationVelAccModule_t::velocity_t(-360000000),STANDARD::RotationVelAccModule_t::velocity_t(360000000)}},
                    {ROTATION_ACCELERATION, {STANDARD::RotationVelAccModule_t::acceleration_t(-360000000),STANDARD::RotationVelAccModule_t::acceleration_t(360000000)}},

                    {SCALE, {STANDARD::ScaleModule_t::scale_t(-2147483648LL),STANDARD::ScaleModule_t::scale_t(2147483647)}}
                };
                /**
                 * @brief Get value range for a Standard module value
                 * 
                 * @param value Standard module value
                 * @return Valid range
                 */
                range_t OTP_LIB_EXPORT getRange(moduleValue_t value);
            }
        }
    }

    /**
     * @brief Get a list of supported modules by this library
     * 
     * @return List of support modules
     */
    MESSAGES::OTPModuleAdvertisementMessage::list_t const getSupportedModules();

    /**
     * @brief Get Human readable Module Identifier (Manufacturer ID and Module Name)
     * 
     * @param ident Module Identifier
     * @return Module Description
     */
    module_t::moduleDescription_t const OTP_LIB_EXPORT getModuleDescription(PDU::OTPModuleLayer::ident_t ident);
}

#endif // MODULES_CONST_HPP
