/**
 * @file        modules_types.hpp
 * @brief       Custom types for OTP Modules
 * @details     Part of OTPLib - A QT interface for E1.59
 * @authors     Marcus Birkin
 * @copyright   Copyright (C) 2020 Marcus Birkin
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
#ifndef MODULES_TYPES_HPP
#define MODULES_TYPES_HPP

#include "../../bugs.hpp"
#include <QDateTime>
#include <QRegularExpression>
#include <bitset>
#include "../messages/message_types.hpp"
#include "../pdu/pdu_types.hpp"

using namespace OTP::PDU;

namespace OTP::MODULES {
    /*! Creates a type name for OTP::PDU::OTPTransformLayer::timestamp_t */ 
    typedef OTP::PDU::OTPTransformLayer::timestamp_t timestamp_t;

    /**
     * @brief Module identification type
     * 
     */
    typedef struct module_t
    {
        /**
         * @brief Human readable Module Description Type
         * 
         */
        typedef struct moduleDescription_t
        {
            QString Manufactuer; /**< Manufacturer Name (e.g. ESTA) */
            QString Name; /**< Module Name (e.g. Position)*/
        } moduleDescription_t;

        /**
         * @brief Human readable Module Description
         * 
         */
        moduleDescription_t description;

        /**
         * @brief Module Identifier
         * @details
         * Modules are uniquely identified using a Module Identifier, 
         * which is composed of a Manufacturer ID and a 16-bit Module Number.
         * 
         */
        PDU::OTPModuleLayer::ident_t ident;
    } module_t;

    /*! Creates a type name for PDU::OTPModuleLayer::moduleNumber_t */ 
    typedef PDU::OTPModuleLayer::moduleNumber_t moduleNumber_t;

    /**
     * @brief These Standard Modules each define different sets of information about a Point.
     * @details As defined by E1-59 Section 16 "Standard Modules"
     * 
     */
    namespace STANDARD {
        /*! Creates a type name for OTP::PDU::OTPModuleLayer::additional_t */ 
        typedef OTP::PDU::OTPModuleLayer::additional_t additional_t;

        /**
         * @brief Valid axes
         * 
         */
        typedef enum axis_e
        {
            first,
            X = first, /**< X Axis */
            Y, /**< Y Axis */
            Z, /**< Z Axis */
            last = Z,
            count
        } axis_t;
        /**
         * @brief Axis post-increment operator
         * @details Does not roll over
         * 
         * @param a Axis object to increase
         * 
         * @return Next axis
         */
        inline axis_e operator++(axis_e& a,int){
            axis_e temp=a;
            a = std::clamp(static_cast<axis_t>(static_cast<unsigned int>(a) + 1), axis_t::first, axis_t::count);
            return temp;
        }
        /**
         * @brief Axis pre-increment operator
         * @details Does not roll over
         * 
         * @param a Axis object to increase
         * 
         * @return Next axis
         */
        inline axis_e operator++ (axis_e& a) {
            return a++;
        }

        /**
         * @internal
         * @brief This data structure contains the current position of a Point in all three linear directions.
         * @details As defined by E1-59 Section 16.1 "Position Module"
         * 
         */
        class PositionModule_t {
        public:
            /**
             * @brief Option flags
             * 
             */
            typedef struct options_t
            {
                options_t() : data(0) {}

                /**
                 * @brief Is the scaling option set
                 * @details 
                 * This bit, when set to 1, indicates that the remaining fields in this Module provide values in mm. 
                 * When set to 0, the remaining fields use μm.
                 * 
                 * @return true Scaling option set
                 * @return false Scaling option not set
                 */
                bool isScaling() const { return data[SCALING_BIT]; }
                
                /**
                 * @brief Set scaling option
                 * @details 
                 * This bit, when set to 1, indicates that the remaining fields in this Module provide values in mm. 
                 * When set to 0, the remaining fields use μm.
                 * 
                 * @param value New value of scaling option
                 */
                void setScaling(bool value) { data[SCALING_BIT] = value; }

                friend additional_t& operator<<(additional_t &l, const options_t &r) {
                    l << type(r.data.to_ulong());
                    return l;
                }
                friend additional_t& operator>>(additional_t &l, options_t &r) {
                    type temp;
                    l >> temp;
                    r.data = std::bitset<bitWidth>(temp);
                    return l;
                }
            private:
                typedef quint8 type;
                static const quint8 bitWidth = sizeof(type) * 8;
                enum {
                    SCALING_BIT = 7 /**< Value scabled in mm (1) or μm (0) */
                };
                std::bitset<bitWidth> data;
            } options_t;

            /**
             * @brief Point position type
             * 
             */
            typedef qint32 position_t;

            /**
             * @brief Scale of position value
             * 
             */
            typedef enum scale_e : quint8
            {
                mm = 0, /**< Millimeters (mm) */
                um = 1 /**< Micrometers (μm) */
            } scale_t;

            PositionModule_t() : options(), timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(position), std::end(position), 0);
            }

            /**
             * @brief Construct a new Position Module object
             * 
             * @param additional Additional fields
             * @param timestamp Sample timestamp
             */
            PositionModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                PositionModule_t temp;
                additional >> temp;
                options = temp.options;
                std::copy(std::begin(temp.position), std::end(temp.position), std::begin(position));
            }

            /**
             * @brief Get the sample timestamp for the most recent point data
             * 
             * @return Position data sample time
             */
            timestamp_t getTimestamp() const { return timestamp; }

            /**
             * @brief Get last point activity time for the module
             * 
             * @return Last activity time 
             */
            QDateTime getLastSeen() const { return lastSeen; }

            /**
             * @brief Set the scaling of the value
             * 
             * @param scale New value scale
             */
            void setScaling(scale_t scale) {
                switch (scale)
                {
                    case mm: options.setScaling(true); break;
                    case um: options.setScaling(false); break;
                }
                updateLastSeen();
            }

            /**
             * @brief Get the scaling of the value
             * 
             * @return Current value scale
             */
            scale_t getScaling() const { return options.isScaling() ? mm : um; }

            /**
             * @brief Is the value scaling current set to Millimeters (mm)
             * 
             * @return true Value is in Millimeters (mm)
             * @return false Value is not in Millimeters (mm)
             */
            bool isScalingMM() const { return getScaling() == mm; }

            /**
             * @brief Is the value scaling current set to Micrometers (μm)
             * 
             * @return true Value is in Micrometers (μm)
             * @return false Value is not in Micrometers (μm)
             */
            bool isScalingUM() const { return getScaling() == um; }

            /**
             * @brief Get the current position of an axis
             * 
             * @param axis Axis to query
             * @return Current position 
             */
            position_t getPosition(axis_t axis) const { return position[std::clamp(axis, axis_t::first, axis_t::last)]; }

            /**
             * @brief Set the current position of an axis
             * 
             * @param axis Axis to set
             * @param value New position
             * @param time Sample time
             */
            void setPosition(axis_t axis, position_t value, timestamp_t time = 0)
            {
                position[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                timestamp = time;
                updateLastSeen();
            }

            friend additional_t& operator<<(additional_t &l, const PositionModule_t &r)
            {
                return l << r.options << r.position[axis_e::X] << r.position[axis_e::Y] << r.position[axis_e::Z];
            }
            friend additional_t& operator>>(additional_t &l, PositionModule_t &r)
            {
                l >> r.options >> r.position[axis_e::X] >> r.position[axis_e::Y] >> r.position[axis_e::Z];
                return l;
            }
            friend bool operator==(const PositionModule_t& l, const PositionModule_t& r)
            {
                additional_t la, ra;
                la << l;
                ra << r;
                return (ra == la);
            }
            friend bool operator!=(const PositionModule_t& l, const PositionModule_t& r) { return !(l == r); }
            friend PositionModule_t& operator+=(PositionModule_t& l, const PositionModule_t& r) {
                for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                {
                    if (l.getScaling() == r.getScaling())
                    {
                        l.position[axis] += r.position[axis];
                    } else {
                        switch (l.getScaling())
                        {
                            case mm: l.position[axis] += (r.position[axis] * 1000); break;
                            case um: l.position[axis] += (r.position[axis] / 1000); break;
                        }
                    }
                }
                return l;
            }
        private:
            options_t options;
            position_t position[axis_t::count];
            timestamp_t timestamp = 0;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };

        /**
         * @internal
         * @brief This data structure contains the positional velocity and acceleration of a Point.
         * @details As defined by E1-59 Section 16.2 "Position Velocity/Acceleration Module"
         * 
         */
        class PositionVelAccModule_t {
        public:
            /**
             * @brief Point velocity type
             * 
             */
            typedef qint32 velocity_t;

            /**
             * @brief Point acceleration type
             * 
             */
            typedef qint32 acceleration_t;

            PositionVelAccModule_t() : timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(velocity), std::end(velocity), 0);
                std::fill(std::begin(acceleration), std::end(acceleration), 0);
            }

            /**
             * @brief Construct a new Position Velocity/Acceleration Module object
             * 
             * @param additional Additional fields
             * @param timestamp Sample timestamp
             */
            PositionVelAccModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                PositionVelAccModule_t temp;
                additional >> temp;
                std::copy(std::begin(temp.velocity), std::end(temp.velocity), std::begin(velocity));
                std::copy(std::begin(temp.acceleration), std::end(temp.acceleration), std::begin(acceleration));
            }

            /**
             * @brief Get the sample timestamp for the most recent point data
             * 
             * @return Position data sample time
             */
            timestamp_t getTimestamp() const { return timestamp; }

            /**
             * @brief Get last point activity time for the module
             * 
             * @return Last activity time 
             */
            QDateTime getLastSeen() const { return lastSeen; }

            /**
             * @brief Get the Velocity of axis
             * 
             * @param axis Axis to query
             * @return Current Velocity
             */
            velocity_t getVelocity(axis_t axis) const 
            { 
                return velocity[std::clamp(axis, axis_t::first, axis_t::last)]; 
            }

            /**
             * @brief Set the Velocity of the axis
             * 
             * @param axis Axis to set
             * @param value New Velocity value
             * @param time Sample timestamp
             */
            void setVelocity(axis_t axis, velocity_t value, timestamp_t time = 0)
            {
                velocity[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                timestamp = time;
                updateLastSeen();
            }

            /**
             * @brief Get the Acceleration of axis
             * 
             * @param axis Axis to query
             * @return Current Acceleration
             */
            acceleration_t getAcceleration(axis_t axis) const 
            { 
                return acceleration[std::clamp(axis, axis_t::first, axis_t::last)]; 
            }

            /**
             * @brief Set the Acceleration of the axis
             * 
             * @param axis Axis to set
             * @param value New Acceleration value
             * @param time Sample timestamp
             */
            void setAcceleration(axis_t axis, acceleration_t value, timestamp_t time = 0)
            {
                acceleration[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                timestamp = time;
                updateLastSeen();
            }

            friend additional_t& operator<<(additional_t &l, const PositionVelAccModule_t &r)
            {
                l << r.velocity[axis_e::X] << r.velocity[axis_e::Y] << r.velocity[axis_e::Z];
                return l << r.acceleration[axis_e::X] << r.acceleration[axis_e::Y] << r.acceleration[axis_e::Z];
            }
            friend additional_t& operator>>(additional_t &l, PositionVelAccModule_t &r)
            {
                l >> r.velocity[axis_e::X] >> r.velocity[axis_e::Y] >> r.velocity[axis_e::Z];
                l >> r.acceleration[axis_e::X] >> r.acceleration[axis_e::Y] >> r.acceleration[axis_e::Z];
                return l;
            }
            friend bool operator==(const PositionVelAccModule_t& l, const PositionVelAccModule_t& r)
            {
                additional_t la, ra;
                la << l;
                ra << r;
                return (ra == la);
            }
            friend bool operator!=(const PositionVelAccModule_t& l, const PositionVelAccModule_t& r) { return !(l == r); }
            friend PositionVelAccModule_t& operator+=(PositionVelAccModule_t& l, const PositionVelAccModule_t& r) {
                for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                {
                    l.velocity[axis] += r.velocity[axis];
                    l.acceleration[axis] += r.acceleration[axis];
                }
                return l;
            }

        private:
            velocity_t velocity[axis_t::count];
            acceleration_t acceleration[axis_t::count];
            timestamp_t timestamp = 0;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };

        /**
         * @internal
         * @brief This data structure contains the current rotation of the Point 
         * using intrinsic Euler rotation calculated in the x-convention (the Tait-Bryan ZYX convention)
         * @details As defined by E1-59 Section 16.3 "Rotation Module"
         * 
         */
        class RotationModule_t {
        public:
            /**
             * @brief Point rotation type
             * 
             */
            class rotation_t {
            private:
                typedef quint32 type;

            public:
                rotation_t() : data(0) {}

                /**
                 * @brief Construct a new Rotation object
                 * 
                 * @param value Rotation value
                 */
                rotation_t(type value) : data(value) {}

                /**
                 * @brief Return the current Rotation value
                 * 
                 * @return Rotation value 
                 */
                operator type() const {
                    return data;
                }

                friend additional_t& operator<<(additional_t &l, const rotation_t &r) {
                    return l << r.data;
                }
                friend additional_t& operator>>(additional_t &l, rotation_t &r)
                {
                    l >> r.data;
                    return l;
                }
                friend bool operator==(const rotation_t& l, const rotation_t& r)
                {
                    return l.data == r.data;
                }
                friend bool operator!=(const rotation_t& l, const rotation_t& r) { return !(l == r); }
                friend rotation_t& operator+=(rotation_t& l, const rotation_t& r);

            private:
                type data;
            };

            RotationModule_t() : timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(rotation), std::end(rotation), 0);
            }

            /**
             * @brief Construct a new Rotation Module object
             * 
             * @param additional Additional fields
             * @param timestamp Sample timestamp
             */
            RotationModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                RotationModule_t temp;
                additional >> temp;
                std::copy(std::begin(temp.rotation), std::end(temp.rotation), std::begin(rotation));
            }

            /**
             * @brief Get the sample timestamp for the most recent point data
             * 
             * @return Position data sample time
             */
            timestamp_t getTimestamp() const { return timestamp; }

            /**
             * @brief Get last point activity time for the module
             * 
             * @return Last activity time 
             */
            QDateTime getLastSeen() const { return lastSeen; }

            /**
             * @brief Get the Rotation of axis
             * 
             * @param axis Axis to query
             * @return Current Rotation
             */
            rotation_t getRotation(axis_t axis) const 
            { 
                return rotation[std::clamp(axis, axis_t::first, axis_t::last)]; 
            }
            
            /**
             * @brief Set the Rotation of the axis
             * 
             * @param axis Axis to set
             * @param value New Rotation value
             * @param time Sample timestamp
             */
            void setRotation(axis_t axis, rotation_t value, timestamp_t time = 0)
            {
                rotation[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                timestamp = time;
                updateLastSeen();
            }

            friend additional_t& operator<<(additional_t &l, const RotationModule_t &r)
            {
                return l << r.rotation[axis_e::X] << r.rotation[axis_e::Y] << r.rotation[axis_e::Z];
            }
            friend additional_t& operator>>(additional_t &l, RotationModule_t &r)
            {
                l >> r.rotation[axis_e::X] >> r.rotation[axis_e::Y] >> r.rotation[axis_e::Z];
                return l;
            }
            friend bool operator==(const RotationModule_t& l, const RotationModule_t& r)
            {
                additional_t la, ra;
                la << l;
                ra << r;
                return (ra == la);
            }
            friend bool operator!=(const RotationModule_t& l, const RotationModule_t& r) { return !(l == r); }
            friend RotationModule_t& operator+=(RotationModule_t& l, const RotationModule_t& r) {
                for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                    l.rotation[axis] += r.rotation[axis];
                return l;
            }

        private:
            rotation_t rotation[axis_t::count];
            timestamp_t timestamp = 0;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };

        /**
         * @internal
         * @brief This data structure contains the rotational velocity and acceleration of the Point 
         * using intrinsic Euler rotation calculated in the x-convention (the Tait-Bryan ZYX convention)
         * @details As defined by E1-59 Section 16.4 "Rotation Velocity/Acceleration Module"
         * 
         */
        class RotationVelAccModule_t {
        public:
            /*! Velocity value type */ 
            typedef qint32 velocity_t;
            /*! Acceleration value type */ 
            typedef qint32 acceleration_t;

            RotationVelAccModule_t() : timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(velocity), std::end(velocity), 0);
                std::fill(std::begin(acceleration), std::end(acceleration), 0);
            }

            /**
             * @brief Construct a new Rotation Velocity/Acceleration Module object
             * 
             * @param additional Additional fields
             * @param timestamp Sample timestamp
             */
            RotationVelAccModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                RotationVelAccModule_t temp;
                additional >> temp;
                std::copy(std::begin(temp.velocity), std::end(temp.velocity), std::begin(velocity));
                std::copy(std::begin(temp.acceleration), std::end(temp.acceleration), std::begin(acceleration));
            }

            /**
             * @brief Get the sample timestamp for the most recent point data
             * 
             * @return Position data sample time
             */
            timestamp_t getTimestamp() const { return timestamp; }

            /**
             * @brief Get last point activity time for the module
             * 
             * @return Last activity time 
             */
            QDateTime getLastSeen() const { return lastSeen; }

            /**
             * @brief Get the Velocity of axis
             * 
             * @param axis Axis to query
             * @return Current Velocity
             */
            velocity_t getVelocity(axis_t axis) const 
            { 
                return velocity[std::clamp(axis, axis_t::first, axis_t::last)];
            }
            
            /**
             * @brief Set the Velocity of the axis
             * 
             * @param axis Axis to set
             * @param value New Velocity value
             * @param time Sample timestamp
             */
            void setVelocity(axis_t axis, velocity_t value, timestamp_t time = 0)
            {
                velocity[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                timestamp = time;
                updateLastSeen();
            }

            /**
             * @brief Get the Acceleration of axis
             * 
             * @param axis Axis to query
             * @return Current Acceleration
             */
            acceleration_t getAcceleration(axis_t axis) const 
            {
                return acceleration[std::clamp(axis, axis_t::first, axis_t::last)];
            }

            /**
             * @brief Set the Acceleration of the axis
             * 
             * @param axis Axis to set
             * @param value New Acceleration value
             * @param time Sample timestamp
             */
            void setAcceleration(axis_t axis, acceleration_t value, timestamp_t time = 0)
            {
                acceleration[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                timestamp = time;
                updateLastSeen();
            }

            friend additional_t& operator<<(additional_t &l, const RotationVelAccModule_t &r)
            {
                l << r.velocity[axis_e::X] << r.velocity[axis_e::Y] << r.velocity[axis_e::Z];
                return l << r.acceleration[axis_e::X] << r.acceleration[axis_e::Y] << r.acceleration[axis_e::Z];
            }
            friend additional_t& operator>>(additional_t &l, RotationVelAccModule_t &r)
            {
                l >> r.velocity[axis_e::X] >> r.velocity[axis_e::Y] >> r.velocity[axis_e::Z];
                l >> r.acceleration[axis_e::X] >> r.acceleration[axis_e::Y] >> r.acceleration[axis_e::Z];
                return l;
            }
            friend bool operator==(const RotationVelAccModule_t& l, const RotationVelAccModule_t& r)
            {
                additional_t la, ra;
                la << l;
                ra << r;
                return (ra == la);
            }
            friend bool operator!=(const RotationVelAccModule_t& l, const RotationVelAccModule_t& r) { return !(l == r); }
            friend RotationVelAccModule_t& operator+=(RotationVelAccModule_t& l, const RotationVelAccModule_t& r) {
                for (auto axis = axis_t::first; axis < axis_t::count; axis++)
                {
                    l.velocity[axis] += r.velocity[axis];
                    l.acceleration[axis] += r.acceleration[axis];
                }
                return l;
            }

        private:
            velocity_t velocity[axis_t::count];
            acceleration_t acceleration[axis_t::count];
            timestamp_t timestamp = 0;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };

        /**
         * @internal
         * @brief This data structure describes the unitless, absolute scale of the Point 
         * in the X, Y, and Z directions. 
         * @details As defined by E1-59 Section 16.5 "Scale"
         * 
         * The Scale Module may be used for description of Points that have the ability to change size.
         * 
         */
        class OTP_LIB_EXPORT ScaleModule_t {
        public:
            /**
             * @brief Integer scale type
             * 
             */
            typedef qint32 scale_t;

            /**
             * @brief Floating point scale percentage
             * 
             */
            typedef qreal percent_t;

            ScaleModule_t() : timestamp(0), lastSeen(QDateTime())
            {
                std::fill(std::begin(scale), std::end(scale), fromPercent(100));
            }

            /**
             * @brief Construct a new Scale Module object
             * 
             * @param additional Additional fields
             * @param timestamp Sample timestamp
             */
            ScaleModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                ScaleModule_t temp;
                additional >> temp;
                std::copy(std::begin(temp.scale), std::end(temp.scale), std::begin(scale));
            }

            /**
             * @brief Get the sample timestamp for the most recent point data
             * 
             * @return Position data sample time
             */
            timestamp_t getTimestamp() const { return timestamp; }

            /**
             * @brief Get last point activity time for the module
             * 
             * @return Last activity time 
             */
            QDateTime getLastSeen() const { return lastSeen; }

            /**
             * @brief Get the Scale of the axis
             * 
             * @param axis Axis to query
             * @return Scale of axis 
             */
            scale_t getScale(axis_t axis) const 
            {
                return scale[std::clamp(axis, axis_t::first, axis_t::last)];
            }
            
            /**
             * @brief Set the Scale of the axis
             * 
             * @param axis Axis to set
             * @param value New scale of axis
             * @param time Sample timestamp
             */
            void setScale(axis_t axis, scale_t value, timestamp_t time = 0)
            {
                scale[std::clamp(axis, axis_t::first, axis_t::last)] = value;
                timestamp = time;
                updateLastSeen();
            }

            /**
             * @brief Convert scale to percent
             * 
             * @param scale Scale to convert
             * @return Percentage 
             */
            static percent_t toPercent(scale_t scale);

            /**
             * @brief Convert scale to human readable percentage
             * 
             * @param scale Scale to convert
             * @param precision Precision of returned percentage
             * @return Percentage string 
             */
            static QString toPercentString(scale_t scale, int precision = 4) {
                auto ret = QString::number(toPercent(scale), 'f', precision);
                // Remove trailing fractional zeros. i.e. 10.0100 becomes 10.01
                static auto trailingZeros = QRegularExpression("\\.?0+$");
                ret.remove(trailingZeros);
                return ret;
            }

            /**
             * @brief Convert percentage to scale
             * 
             * @param percent Percentage to convert
             * @return Scale 
             */
            static scale_t fromPercent(percent_t percent);

            friend additional_t& operator<<(additional_t &l, const ScaleModule_t &r)
            {
                l << r.scale[axis_e::X] << r.scale[axis_e::Y] << r.scale[axis_e::Z];
                return l;
            }
            friend additional_t& operator>>(additional_t &l, ScaleModule_t &r)
            {
                l >> r.scale[axis_e::X] >> r.scale[axis_e::Y] >> r.scale[axis_e::Z];
                return l;
            }
            friend bool operator==(const ScaleModule_t& l, const ScaleModule_t& r)
            {
                additional_t la, ra;
                la << l;
                ra << r;
                return (ra == la);
            }
            friend bool operator!=(const ScaleModule_t& l, const ScaleModule_t& r) { return !(l == r); }

        private:
            scale_t scale[axis_t::count];
            timestamp_t timestamp = 0;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };

        /**
         * @internal
         * @brief This data structure contains the Address of the Reference Frame of the Point.
         * @details As defined by E1-59 Section 16.6 "Reference Frame"
         * 
         * If a Reference Frame Module is included in a Point Layer, 
         * any other Modules included in this Point shall
         * contain transform information relative to that of the Reference Frame.
         * 
         */
        class ReferenceFrameModule_t {
        public:
            /*! Creates a type name for OTPTransformLayer::system_t */ 
            typedef OTPTransformLayer::system_t system_t;
            /*! Creates a type name for OTPTransformLayer::point_t */ 
            typedef OTPPointLayer::point_t point_t;
            /*! Creates a type name for OTPTransformLayer::group_t */ 
            typedef OTPPointLayer::group_t group_t;

            ReferenceFrameModule_t() : timestamp(0), lastSeen(QDateTime()) { }

            /**
             * @brief Construct a new Reference Frame Module object
             * 
             * @param additional Additional fields
             * @param timestamp Sample timestamp
             */
            ReferenceFrameModule_t(additional_t additional, timestamp_t timestamp) : timestamp(timestamp)
            {
                updateLastSeen();
                ReferenceFrameModule_t temp;
                additional >> temp;
                system = temp.system;
                group = temp.group;
                point = temp.point;
            }

            /**
             * @brief Get the sample timestamp for the most recent point data
             * 
             * @return Position data sample time
             */
            timestamp_t getTimestamp() const { return timestamp; }

            /**
             * @brief Get last point activity time for the module
             * 
             * @return Last activity time 
             */
            QDateTime getLastSeen() const { return lastSeen; }

            /**
             * @brief Get the System number
             * 
             * @return System number 
             */
            system_t getSystem() const { return system; }

            /**
             * @brief Set the System number
             * 
             * @param value New system number
             * @param time Sample timestamp
             */
            void setSystem(system_t value, timestamp_t time)
            {
                system = value;
                timestamp = time;
                updateLastSeen();
            }

            /**
             * @brief Get the Group number
             * 
             * @return Group number 
             */
            group_t getGroup() const { return group; }

            /**
             * @brief Set the Group number
             * 
             * @param value New Group number
             * @param time Sample timestamp
             */
            void setGroup(group_t value, timestamp_t time)
            {
                group = value;
                timestamp = time;
                updateLastSeen();
            }

            /**
             * @brief Get the Point number
             * 
             * @return Point number 
             */
            point_t getPoint() const { return point; }

            /**
             * @brief Set the Point number
             * 
             * @param value New Point number
             * @param time Sample timestamp
             */
            void setPoint(point_t value, timestamp_t time)
            {
                point = value;
                timestamp = time;
                updateLastSeen();
            }

            friend additional_t& operator<<(additional_t &l, const ReferenceFrameModule_t &r)
            {
                return l << r.system << r.group << r.point;
            }
            friend additional_t& operator>>(additional_t &l, ReferenceFrameModule_t &r)
            {
                l >> r.system >> r.group >> r.point;
                return l;
            }
            friend bool operator==(const ReferenceFrameModule_t& l, const ReferenceFrameModule_t& r)
            {
                additional_t la, ra;
                la << l;
                ra << r;
                return (ra == la);
            }
            friend bool operator!=(const ReferenceFrameModule_t& l, const ReferenceFrameModule_t& r) { return !(l == r); }

        private:
            system_t system;
            group_t group;
            point_t point;
            timestamp_t timestamp = 0;
            void updateLastSeen() { lastSeen = QDateTime::currentDateTime(); }
            QDateTime lastSeen;
        };
    }
}

#endif // MODULES_TYPES_HPP
